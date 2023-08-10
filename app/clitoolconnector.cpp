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
#include <QJsonArray>
#include <QTimer>

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
    , m_isReady(false)
{
    m_settings = new QSettings("hideconfig.ini");
    m_userName = m_settings->value("user").toString();
    m_password = m_settings->value("password").toString();
    m_url = m_settings->value("url", "127.0.0.1").toString();
    m_port = m_settings->value("port", 5050).toInt();

    if(!m_userName.isEmpty() && !m_password.isEmpty()) {
        getTokenRequest(m_userName, m_password);
    }

    QFile cli(m_program);
    m_cliAvailable = cli.exists();

#ifdef WITH_CLICK
    m_caPath = QCoreApplication::applicationDirPath() + "/CA.pem";
#else
    m_caPath = "/usr/share/hideme/CA.pem";
#endif

    m_serviceProcess = new QProcess(this);

    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    m_dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";

    connect(this, &CliToolConnector::loginSuccess, this, &CliToolConnector::initServiceSetup);
    connect(this, &CliToolConnector::isReadyChanged, this, &CliToolConnector::loadServiceConfig);
    connect(m_serviceProcess, &QProcess::readyReadStandardOutput, this, &CliToolConnector::startServiceHandler);

    checkServerStatus();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CliToolConnector::checkServerStatusProto);
    timer->start(1000);
}

CliToolConnector::~CliToolConnector()
{
    m_serviceProcess->close();
}

void CliToolConnector::startService(QString sudoPass)
{
    m_serviceProcess->start("/bin/bash", QStringList());
    QString command = QString("echo %1 | sudo -S %2 --caddr %3:%4 service \n")
            .arg(sudoPass)
            .arg(m_program)
            .arg(m_url)
            .arg(m_port);

    m_serviceProcess->write(command.toUtf8());
    checkServerStatus();
}

void CliToolConnector::startServiceHandler()
{
    qDebug() << m_serviceProcess->readAllStandardOutput();
}

void CliToolConnector::checkServerStatusProto()
{
    checkServerStatus();
}

bool CliToolConnector::cliAvailable() const
{
    return m_cliAvailable;
}

void CliToolConnector::getTokenRequest(QString user, QString password)
{
    if(user.isEmpty() || password.isEmpty()) {
        qWarning() << "Empty login or pass";
        return;
    }

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

    m_userName = user;
    m_password = password;
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

        m_settings->setValue("user", "");
        m_settings->setValue("password", "");

        return;
    }

    QString token = reply->readAll().replace("\"","");
    QFile tokenFile(m_dataDir + "accessToken.txt");

    if (tokenFile.open(QIODevice::WriteOnly)) {
        QTextStream out(&tokenFile); out << token;
        tokenFile.close();
    }

    loginSuccess();

    m_settings->setValue("user", m_userName);
    m_settings->setValue("password", m_password);
}

void CliToolConnector::initServiceSetup()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(m_url).arg(m_port)));
    request.setRawHeader("Content-Type", "application/json; charset=UTF-8");

    QJsonObject obj;
    obj["AccessTokenPath"] = m_dataDir + "accessToken.txt";
    obj["Username"] = m_userName;
    obj["Password"] = m_password;
    obj["CA"] = m_caPath;
    QJsonObject restObj;
    restObj["Rest"] = obj;

    QJsonDocument doc(restObj);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::initServiceSetupHandler);
}

void CliToolConnector::initServiceSetupHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    if(reply->error()) {
        m_isReady = false;
        emit isReadyChanged();
        return;
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    if(answ["result"] != true) {
        qWarning() << "wrong configuration!";
    } else {
        m_isReady = true;
        emit isReadyChanged();
    }
}

void CliToolConnector::loadServiceConfig()
{
    if(!m_isReady) {
        qWarning() << "Not ready";
        return;
    }
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::loadServiceConfigHandler);
}

void CliToolConnector::loadServiceConfigHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    qDebug() << answ;
}

bool CliToolConnector::isLogined() const
{
    qDebug() << m_userName << m_password;
    return (!m_userName.isEmpty() && !m_password.isEmpty());
}

bool CliToolConnector::isReady() const
{
    return m_isReady;
}

void CliToolConnector::setConnected(bool newConnected)
{
    QString url;
    if (m_connected == newConnected) {
        return;
    }

    if(newConnected) {
        url = QString("http://%1:%2/connect").arg(m_url).arg(m_port);
    } else {
        url = QString("http://%1:%2/disconnect").arg(m_url).arg(m_port);
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::setConnectedHandler);
}

void CliToolConnector::setConnectedHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    if(!answ["error"].isUndefined()){
        QJsonObject errorObject = answ["error"].toObject();
        if(errorObject.value("code").toString() == "connect") {
            if(m_connected) {
                m_connected = false;
                emit connectedChanged();
            }
        }

        if(errorObject.value("code").toString() == "disconnect") {
            if(!m_connected) {
                m_connected = true;
                emit connectedChanged();
            }
        }

        emit error(errorObject.value("message").toString());
    }
}

void CliToolConnector::checkServerStatus()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/state").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::checkServerStatusHandler);
}

void CliToolConnector::checkServerStatusHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    if(reply->error()) {
        qWarning() << "Service not started";
        if(m_serverStaerted) {
            m_serverStaerted = false;
            emit serverStaertedChanged();
        }
    } else {
        qDebug() << "Service is started" << reply->readAll();
        if(!m_serverStaerted) {
            m_serverStaerted = true;
            emit serverStaertedChanged();
        }
    }
}

QString CliToolConnector::url() const
{
    return m_url;
}

void CliToolConnector::setUrl(const QString &newUrl)
{
    if (m_url == newUrl)
        return;
    m_url = newUrl;
    emit urlChanged();
}

int CliToolConnector::port() const
{
    return m_port;
}

void CliToolConnector::setPort(int newPort)
{
    if (m_port == newPort)
        return;
    m_port = newPort;
    emit portChanged();
}

bool CliToolConnector::serverStaerted() const
{
    return m_serverStaerted;
}
