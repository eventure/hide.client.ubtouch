#include "constants.h"
#include "clitoolconnector.h"
#include "logging.h"

#include <QFile>
#include <QProcess>
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
    , m_connected(false)
    , m_isReady(false)
    , m_caPath(CA_PEM_PATH)
{
    m_settings = new QSettings("hideconfig.ini");
    m_userName = m_settings->value("user").toString();
    m_password = m_settings->value("password").toString();
    m_hostName = m_settings->value("defaultHost", "free-nl-v4.hideservers.net").toString();

    Logging::instance()->add("User:" + m_userName);
    Logging::instance()->add("defaultHost:" + m_hostName);

    if(!m_userName.isEmpty() && !m_password.isEmpty()) {
        getTokenRequest();
    }

    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    connect(this, &CliToolConnector::loginFailed, this, &CliToolConnector::logout);

    loadServiceConfig();
}

CliToolConnector::~CliToolConnector()
{
}

void CliToolConnector::setLoginPass(const QString usermame, const QString password)
{
    if(!usermame.isEmpty() && !password.isEmpty()) {
        m_userName = usermame;
        m_password = password;

        Logging::instance()->add("Login changed to " + m_userName);
    }
}

void CliToolConnector::setParam(const QString param, const QString value)
{
    if(param.isEmpty() || value.isEmpty()) {
        Logging::instance()->add("Empty param or value");
        return;
    }

    QJsonObject obj;
    obj[param] = value;

    QJsonObject rest;
    rest["Rest"] = obj;

    QJsonDocument doc(rest);
    QByteArray data = doc.toJson();

    QString url = m_settings->value("url", "127.0.0.1").toString();
    int port = m_settings->value("port", 5050).toInt();

    Logging::instance()->add("Send configuration: " + data);

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(url).arg(port)));

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::setParamRequestHandler);
}

void CliToolConnector::getTokenRequest()
{
    if(m_userName.isEmpty() || m_password.isEmpty()) {
        Logging::instance()->add("need login first");
        return;
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl("https://free.hideservers.net:432/v1.0.0/accessToken"));
    QSslConfiguration sslconf = QSslConfiguration();

    sslconf.setCaCertificates(QSslCertificate::fromPath(m_caPath));

    request.setSslConfiguration(sslconf);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["domain"] = "hide.me";
    obj["host"] = "free";
    obj["username"] = m_userName.simplified().remove(' ');
    obj["password"] = m_password.simplified().remove(' ');
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

    Logging::instance()->add("Request access token: " + data);

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::getTokenRequestHandler);
}

void CliToolConnector::makeConnection()
{
    QString url = m_settings->value("url", "127.0.0.1").toString();
    int port = m_settings->value("port", 5050).toInt();

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/connect").arg(url).arg(port)));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::requestHandler);
}

void CliToolConnector::makeDisconnection()
{
    QString url = m_settings->value("url", "127.0.0.1").toString();
    int port = m_settings->value("port", 5050).toInt();

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/destroy").arg(url).arg(port)));
    QNetworkReply *reply = mgr->get(request);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::requestHandler);
}

bool CliToolConnector::isDefaultServer(QString hostname)
{
    return m_settings->value("defaultHost").toString() == hostname;
}

void CliToolConnector::logout()
{
    Logging::instance()->add("Logout");

    m_settings->setValue("user", "");
    m_settings->setValue("password", "");
    m_userName = "";
    m_password = "";
    m_token = "";
    emit isLoginedChanged();
}

void CliToolConnector::quit()
{
    qApp->quit();
}

void CliToolConnector::requestHandler() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }
    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());

    Logging::instance()->add("Get from server: " + answ.toJson());

    if(!answ["error"].isUndefined()) {
        QString title = answ["error"].toObject().value("code").toString();
        QString message = answ["error"].toObject().value("message").toString();

        emit error(title, message);
    } else {
        loadServiceConfig();
    }
}

void CliToolConnector::setParamRequestHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }
    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    Logging::instance()->add("Get from server: " + answ.toJson());

    if(!answ["error"].isUndefined()) {
        QString title = answ["error"].toObject().value("code").toString();
        QString message = answ["error"].toObject().value("message").toString();

        emit error(title, message);
    } else {
        loadServiceConfig();
    }
}

void CliToolConnector::loadServiceConfig()
{
    QString url = m_settings->value("url", "127.0.0.1").toString();
    int port = m_settings->value("port", 5050).toInt();

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(url).arg(port)));
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
    Logging::instance()->add("Set config server answer: " + answ.toJson());

    if(!answ["error"].isUndefined()) {
        QString title = answ["error"].toObject().value("code").toString();
        QString message = answ["error"].toObject().value("message").toString();

        emit error(title, message);
    } else {
        setHostName(answ["Rest"].toObject().value("Host").toString());
    }
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
    Logging::instance()->add("Get token data: " + token);

    if(!token.isEmpty() && token != m_token) {
        m_token = token;

        emit tokenChanged();
        loginSuccess();
        m_isReady = true;
        emit isReadyChanged();

        m_settings->setValue("user", m_userName);
        m_settings->setValue("password", m_password);

    }
}

bool CliToolConnector::isLogined() const
{
    return (!m_userName.isEmpty() && !m_password.isEmpty());
}

bool CliToolConnector::isReady() const
{
    return m_isReady;
}

QString CliToolConnector::token() const
{
    return m_token;
}

QString CliToolConnector::defaultHostName() const
{
    return m_settings->value("defaultHost", "free-nl-v4.hideservers.net").toString();
}

void CliToolConnector::setDefaultHostName(const QString &newDefaultHostName)
{
    if (m_settings->value("defaultHost").toString() == newDefaultHostName || newDefaultHostName.isEmpty()) {
        Logging::instance()->add(newDefaultHostName + "not changed or empty");
        return;
    }
    Logging::instance()->add("Changed");
    m_settings->setValue("defaultHost",newDefaultHostName);
    emit defaultHostNameChanged();
}

QString CliToolConnector::hostName() const
{
    return m_hostName;
}

void CliToolConnector::setHostName(const QString &newHostName)
{
    if (m_hostName == newHostName || newHostName.isEmpty()) {
        return;
    }
    m_hostName = newHostName;
    emit hostNameChanged();
}
