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
    , m_connected(false)
    , m_isReady(false)
{
    m_settings = new QSettings("hideconfig.ini");
    m_userName = m_settings->value("user").toString();
    m_password = m_settings->value("password").toString();

    if(!m_userName.isEmpty() && !m_password.isEmpty()) {
        getTokenRequest();
    }

#ifdef WITH_CLICK
    m_caPath = QCoreApplication::applicationDirPath() + "/CA.pem";
#else
    m_caPath = "/usr/share/hideme/CA.pem";
#endif

    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }
}

CliToolConnector::~CliToolConnector()
{
}

void CliToolConnector::setLoginPass(const QString usermame, const QString password)
{
    if(!usermame.isEmpty() && !password.isEmpty()) {
        m_userName = usermame;
        m_password = password;
    }
}

void CliToolConnector::getTokenRequest()
{
    if(m_userName.isEmpty() || m_password.isEmpty()) {
        qWarning() << "need login first";
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
    obj["username"] = m_userName.simplified().remove(' ');
    obj["password"] = m_password.simplified().remove(' ');
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();

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

void CliToolConnector::requestHandler() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }
    if(reply->error()) {
        qWarning() << reply->errorString();
    }

    QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());
    if(!answ["error"].isUndefined()) {
        QString title = answ["error"].toObject().value("code").toString();
        QString message = answ["error"].toObject().value("message").toString();

        emit error(title, message);
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

        m_settings->setValue("user", "");
        m_settings->setValue("password", "");
        return;
    }

    QString token = reply->readAll().replace("\"","");
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
