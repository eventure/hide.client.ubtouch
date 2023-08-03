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

    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    m_dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";

    connect(this, &CliToolConnector::loginSuccess, this, &CliToolConnector::initServiceSetup);
}

CliToolConnector::~CliToolConnector()
{
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
    QNetworkRequest request = QNetworkRequest(QUrl("http://127.0.0.1:5050/configuration"));
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
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    if(answ["result"] != true) {
        qWarning() << "wrong configuration!";
    } else {
        m_isReady = true;
        emit isReadyChanged();
    }
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
}

void CliToolConnector::disconnecting(QString sudoPass)
{
}

bool CliToolConnector::isLogined() const
{
    return (!m_userName.isEmpty() && !m_password.isEmpty());
}

bool CliToolConnector::isReady() const
{
    return m_isReady;
}
