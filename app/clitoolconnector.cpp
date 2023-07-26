#include "clitoolconnector.h"

#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

CliToolConnector::CliToolConnector(QObject *parent)
    : QObject(parent)
#ifdef WITH_CLICK
#if defined(__aarch64__) || defined(_M_ARM64)
    , m_program(QCoreApplication::applicationDirPath()+"/lib/aarch64-linux-gnu/bin/hide.me")
#elif defined(__x86_64__) || defined(_M_X64)
    , m_program(QCoreApplication::applicationDirPath()+"/lib/x86_64-linux-gnu/bin/hide.me")
#endif
#else
    , m_program("/usr/bin/hide.me")
#endif
    , m_connected(false)
    , m_cliToolPID(0)
{
    m_settings = new QSettings("hideconfig.ini");
    m_userName = m_settings->value("user").toString();
    m_password = m_settings->value("password").toString();

    if(!m_userName.isEmpty() && !m_password.isEmpty()) {
        getTokenRequest(m_userName, m_password);
    }

    QFile cli(m_program);
    m_cliAvailable = cli.exists();
    m_cli = new QProcess(this);

    connect(m_cli, &QProcess::readyReadStandardOutput, this, &CliToolConnector::getTokenHandler);
    connect(this, &CliToolConnector::cliStarted, this, &CliToolConnector::onCliToolStarted);
#ifdef WITH_CLICK
    m_baseArgumets << "-ca" << QCoreApplication::applicationDirPath() + "/CA.pem";
#else
    m_baseArgumets << "-ca" << "/usr/share/hideme/CA.pem";
#endif
    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    m_dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
}

CliToolConnector::~CliToolConnector()
{
    m_cli->terminate();
}

bool CliToolConnector::cliAvailable() const
{
    return m_cliAvailable;
}

void CliToolConnector::getTokenRequest(QString user, QString password)
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl("https://free.hideservers.net:432/v1.0.0/accessToken"));
    QSslConfiguration sslconf = QSslConfiguration();
#ifdef WITH_CLICK
    sslconf.setCaCertificates(QSslCertificate::fromPath(QCoreApplication::applicationDirPath() + "/CA.pem"));
#else
    sslconf.setCaCertificates(QSslCertificate::fromPath("/usr/share/hideme/CA.pem"));
#endif
    request.setSslConfiguration(sslconf);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["domain"] = "hide.me";
    obj["host"] = "free";
    obj["username"] = user;
    obj["password"] = password;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::getTokenRequestHandler);
}


void CliToolConnector::getTokenRequestHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    if(reply->error()) {
        qWarning() << reply->errorString();
        emit loginFailed();
        return;
    }

    QString token = reply->readAll().replace("\"","");
    QFile tokenFile(m_dataDir + "accessToken.txt");

    if (tokenFile.open(QIODevice::WriteOnly)) {
        QTextStream out(&tokenFile); out << token;
        tokenFile.close();
    }

    loginSuccess();
}

void CliToolConnector::getToken(QString user, QString password)
{
    if(!m_cliAvailable) {
        qCritical() << "CLI not avaiable" << m_program;
        return;
    }
    m_userName = user;
    m_password = password;

    getTokenRequest(m_userName, m_password);
}

void CliToolConnector::makeConnection(QString sudoPass)
{
    m_errorMessage = "";
    QString server = m_settings->value("server", "de.hideservers.net").toString();

    m_cli->start("/bin/bash", QStringList());

    QString command = QString("echo %1 | sudo -S %2 -ca %3 -t %4 -u %5 -P %6 -b %7 connect %8\n")
            .arg(sudoPass)
            .arg(m_program)
            .arg(m_baseArgumets.at(1))
            .arg(m_dataDir + "accessToken.txt")
            .arg(m_userName)
            .arg(m_password)
            .arg(m_dataDir + "resolv.conf.backup.hide.me")
            .arg(server);

    qDebug() << "RUN COMMAND" << command;

    m_cli->write(command.toUtf8());

    emit startConnecting();
}

void CliToolConnector::disconnecting(QString sudoPass)
{
    QProcess* stopAll = new QProcess();
    stopAll->start("/bin/bash", QStringList());
    stopAll->write(QString("echo %1 | sudo -S /bin/kill `/bin/pidof -s hide.me`\n").arg(sudoPass).toUtf8());
}

void CliToolConnector::getTokenHandler()
{
    QStringList output = QString(m_cli->readAllStandardOutput()).split("\n");
    m_debugOutput.append(output);

    foreach (QString line, output) {
        if(line.contains("[ERR]")) {
            m_errorMessage = line.split("[ERR]").last();
        } else if(line.contains("Link: DPD starting")) {
            m_connected = true;
            emit connectedChanged();
            emit cliStarted();
        } else if(line.contains("Link: Interface vpn deactivated")) {
            m_connected = false;
            emit connectedChanged();
        }
    }

    if(!m_errorMessage.isEmpty()) {
        m_connected = false;
        emit connectedChanged();
    }
}

void CliToolConnector::onCliToolStarted()
{
    QStringList processList;
    QProcess* searchCLI = new QProcess();
    searchCLI->start("/bin/pidof", QStringList() << "-s" << "hide.me");
    connect(searchCLI, &QProcess::readyReadStandardOutput, this, [searchCLI, &processList, this]() {
        QString output = searchCLI->readAllStandardOutput();
        if(!output.isEmpty()) {
            qDebug() << output << output.toUInt();
            uint pid =  output.toUInt();
            if(pid > 0) {
                m_cliToolPID = pid;
                qDebug() << "GOT PID" << m_cliToolPID;
                emit connected();
            }
        }
    });
}

