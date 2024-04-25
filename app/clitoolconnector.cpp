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
    , m_caPath(CA_PEM_PATH)
    , m_isServiceReady(false)
    , m_settings(new Settings("hideconfig.ini"))
{
    m_userName = m_settings->value("user").toString();
    m_password = m_settings->value("password").toString();
    m_hostName = m_settings->value("defaultHost", "free-nl-v4.hideservers.net").toString();

    connect(m_settings
            , &Settings::settingsUpdated
            , this
            , [=] {

        m_userName = m_settings->value("user").toString();
        m_password = m_settings->value("password").toString();
        m_hostName = m_settings->value("defaultHost", "free-nl-v4.hideservers.net").toString();
        isLogined();
    });

    m_url = m_settings->value("url", "127.0.0.1").toString();
    m_port = m_settings->value("port", 5050).toInt();

    Logging::instance()->add("User:" + m_userName);
    Logging::instance()->add("defaultHost:" + m_hostName);

    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    connect(this, &CliToolConnector::loginFailed, this, &CliToolConnector::logout);
    connect(this, &CliToolConnector::hostNameChanged, this, &CliToolConnector::initServiceSetup);
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

        initServiceSetup();
    }
}

void CliToolConnector::setParam(const QString param, const QString value)
{
    if(param.isEmpty() || value.isEmpty()) {
        Logging::instance()->add("Empty param or value");
        return;
    }

    if(!m_isServiceReady) {
        Logging::instance()->add("CliToolConnector::setParam: service not ready");
        return;
    }

    QJsonObject obj;
    obj[param] = value;

    QJsonObject rest;
    rest["Rest"] = obj;

    QJsonDocument doc(rest);
    QByteArray data = doc.toJson();

    Logging::instance()->add("Send configuration: " + data);

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(m_url).arg(m_port)));

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::setParamRequestHandler);
}

void CliToolConnector::getTokenRequest()
{
    if(m_userName.isEmpty() || m_password.isEmpty()) {
        Logging::instance()->add("need login first");
        return;
    }

    if(!m_token.isEmpty()) {
        return;
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl("https://free.hideservers.net:432/v1.0.0/accessToken"));
    QSslConfiguration sslconf = QSslConfiguration();
    if(m_caPath.isEmpty()) {
        qWarning() << "CA.pem is empty";
    }

    if(!QFile::exists(m_caPath)) {
        qWarning() << "CA.pem " << m_caPath << " not found";
    }

    sslconf.setCaCertificates(QSslCertificate::fromPath(m_caPath));

    request.setSslConfiguration(sslconf);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["domain"] = "hide.me";
    obj["host"] = "";
    obj["username"] = m_userName.simplified().remove(' ');
    obj["password"] = m_password.simplified().remove(' ');
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::getTokenRequestHandler);
}

void CliToolConnector::makeConnection()
{
    if(!m_isServiceReady) {
        Logging::instance()->add("CliToolConnector::makeConnection: service not ready");
        return;
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/connect").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::requestHandler);
}

void CliToolConnector::makeDisconnection()
{
    if(!m_isServiceReady) {
        Logging::instance()->add("CliToolConnector::makeDisconnection: service not ready");
        return;
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/destroy").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);
    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::requestHandler);
}

void CliToolConnector::makeRoute()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/route").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);

    Logging::instance()->add("Make route");
    Logging::instance()->add(reply->readAll());
}

void CliToolConnector::initServiceSetup()
{
    if(m_token.isEmpty()) {
        Logging::instance()->add("CliToolConnector::initServiceSetup: token is empty");
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(m_url).arg(m_port)));
    request.setRawHeader("Content-Type", "application/json; charset=UTF-8");

    QJsonObject obj;
    obj["AccessToken"] = m_token;
    obj["AccessTokenPath"] = "";
    obj["Username"] = m_settings->value("user").toString();
    obj["Password"] = m_settings->value("password").toString();
    obj["Host"]= m_hostName;
    obj["CA"] = CA_PEM_PATH;

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
    reply->deleteLater();

    if(reply->error()) {
        Logging::instance()->add("CliToolConnector::initServiceSetupHandler: " + reply->errorString());
        emit setupServiceFail();
    } else {
        Logging::instance()->add("initServiceSetupHandler:" + reply->readAll());
        loadServiceConfig();
    }
}

bool CliToolConnector::isDefaultServer(QString hostname)
{
    return m_settings->value("defaultHost").toString() == hostname;
}

void CliToolConnector::storeLogsToFile()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/log").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &CliToolConnector::storeLogsToFileHandler);
}

void CliToolConnector::storeLogsToFileHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }
    reply->deleteLater();

    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/cli-hideme_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".log";
    QFile file(path);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << reply->readAll();
        file.close();
    }

    Logging::instance()->storeToFile();
}

void CliToolConnector::logout()
{
    Logging::instance()->add("Logout");

    m_settings->setValue("user", "");
    m_settings->setValue("password", "");
    m_userName = "";
    m_password = "";
    m_token = "";

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/token").arg(m_url).arg(m_port)));
    mgr->deleteResource(request);

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

    reply->deleteLater();

    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());

    Logging::instance()->add("CliToolConnector::requestHandler : Get from server: " + answ.toJson());

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

    reply->deleteLater();

    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    Logging::instance()->add("CliToolConnector::setParamRequestHandler : Get from server: " + answ.toJson());

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
    reply->deleteLater();

    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    Logging::instance()->add("auth to: " + answ["Rest"].toObject().value("Host").toString() + " as user " + answ["Rest"].toObject().value("Username").toString());

    if(!answ["error"].isUndefined()) {
        QString title = answ["error"].toObject().value("code").toString();
        QString message = answ["error"].toObject().value("message").toString();

        emit error(title, message);
    } else {
        setHostName(answ["Rest"].toObject().value("Host").toString());
        m_isServiceReady = true;
        emit isServiceReadyChanged();
    }
}

void CliToolConnector::getTokenRequestHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    if(reply->error()) {
        Logging::instance()->add("CliToolConnector::getTokenRequestHandler: " + reply->errorString());
        emit loginFailed();
    } else {

        QString token = reply->readAll().replace("\"","");
        Logging::instance()->add("Get token data: " + token);

        if(!token.isEmpty() && token != m_token) {
            m_token = token;

            emit tokenChanged();
            loginSuccess();

            m_settings->setValue("user", m_userName);
            m_settings->setValue("password", m_password);
            m_settings->sync();
        }
        initServiceSetup();
    }
}

bool CliToolConnector::isLogined() const
{
    return (!m_userName.isEmpty() && !m_password.isEmpty());
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

bool CliToolConnector::isServiceReady() const
{
    return m_isServiceReady;
}

void CliToolConnector::setIsServiceReady(bool newIsServiceReady)
{
    if (m_isServiceReady == newIsServiceReady)
        return;
    m_isServiceReady = newIsServiceReady;
    emit isServiceReadyChanged();
}
