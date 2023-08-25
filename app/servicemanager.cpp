#include "servicemanager.h"
#include "socektconnector.h"

#include <QFile>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDBusMessage>
#include <QDBusPendingReply>

ServiceManager::ServiceManager(QObject *parent)
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
    , m_currentStatus(ServiceStatus::UNKNOW)
    , m_systemDBusConnection(QDBusConnection::systemBus())
{
    m_settings = new QSettings("hideconfig.ini");

    m_url = m_settings->value("url", "127.0.0.1").toString();
    m_port = m_settings->value("port", 5050).toInt();

    m_serviceProcess = new QProcess(this);

    m_connector = new SocektConnector(m_url, m_port, this);
    connect(m_connector, &SocektConnector::codeChanged, this, &ServiceManager::socketCodeChangedHandler);

    QFile cli(m_program);
    m_cliAvailable = cli.exists();

    if (!m_systemDBusConnection.isConnected()) {
        qFatal("Can't connect to system bus");
    }

    if(!QFile::exists("/etc/systemd/system/hideme.service")) {
        m_currentStatus = ServiceStatus::NOT_INSTALLED;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(
                s_serviceName,
                s_servicePath,
                s_propertiesIface,
                "Get");
    msg.setArguments({ s_unitIface, s_propertyActiveState });
    QDBusPendingReply<QVariant> msgPending = QDBusConnection::systemBus().asyncCall(msg);
    QDBusPendingCallWatcher* msgWatcher = new QDBusPendingCallWatcher(msgPending);
    connect(msgWatcher, &QDBusPendingCallWatcher::finished, this, &ServiceManager::checkServerStatus);

    m_systemDBusConnection.connect(
                QString(),
                s_servicePath,
                s_propertiesIface,
                "PropertiesChanged",
                this,
                SLOT(propertiesChanged(QString, QVariantMap, QStringList)));

    QFileSystemWatcher *serviceWatcher = new QFileSystemWatcher();
    serviceWatcher->addPath("/etc/systemd/system/");
    connect(serviceWatcher, &QFileSystemWatcher::directoryChanged, this, &ServiceManager::onServiceFileChanged);

    connect(this, &ServiceManager::currentStatusChanged, this, &ServiceManager::initServiceSetup);
}

ServiceManager::~ServiceManager()
{
}

void ServiceManager::onServiceFileChanged(const QString &path)
{
    qDebug() << Q_FUNC_INFO << path;
    ServiceStatus currentStatus;

    if(!QFile::exists("/etc/systemd/system/hideme.service")) {
        currentStatus = ServiceStatus::NOT_INSTALLED;
    } else {
        currentStatus = ServiceStatus::NOT_STARTED;
    }

    if(currentStatus != m_currentStatus) {
        m_currentStatus = currentStatus;
        emit currentStatusChanged();
    }
}

void ServiceManager::propertiesChanged(const QString &, const QVariantMap &properties, const QStringList &)
{
    if(m_currentStatus == ServiceManager::NOT_INSTALLED) {
        return;
    }

    ServiceManager::ServiceStatus newStatus;
    QString activeStateStr = properties.value(s_propertyActiveState, QString()).toString();
    if(activeStateStr == "active") {
        newStatus = ServiceStatus::STARTED;
    } else {
        newStatus = ServiceStatus::NOT_STARTED;
    }

    if(newStatus != m_currentStatus) {
        m_currentStatus = newStatus;
        emit currentStatusChanged();
    }
}

void ServiceManager::socketCodeChangedHandler()
{
    if(m_currentStatus == ServiceManager::NOT_INSTALLED || m_currentStatus == ServiceManager::NOT_STARTED) {
        return;
    }

    QString state = m_connector->code();
    ServiceManager::ServiceStatus newStatus;

    if(state == "routed") {
        newStatus = ServiceStatus::CONNECTING;
        makeRoute();
    } else if (state == "clean") {
        newStatus = ServiceStatus::STARTED;
    } else if (state == "connecting") {
        newStatus = ServiceStatus::CONNECTING;
    } else if (state == "connected") {
        newStatus = ServiceStatus::CONNECTED;
    }

    if(newStatus != m_currentStatus) {
        m_currentStatus = newStatus;
        emit currentStatusChanged();
    }
}

void ServiceManager::makeRoute()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/route").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);
}

QString ServiceManager::url() const
{
    return m_url;
}

void ServiceManager::setUrl(const QString &newUrl)
{
    if (m_url == newUrl)
        return;
    m_url = newUrl;
    emit urlChanged();
}

int ServiceManager::port() const
{
    return m_port;
}

void ServiceManager::setPort(int newPort)
{
    if (m_port == newPort)
        return;
    m_port = newPort;
    emit portChanged();
}

void ServiceManager::setAccessToken(QString token)
{
    if(!token.isEmpty() != m_accessToken) {
        m_accessToken = token;
        initServiceSetup();
    }
}

void ServiceManager::checkServerStatus(QDBusPendingCallWatcher* watcher)
{
    if(m_currentStatus == ServiceManager::NOT_INSTALLED) {
        qDebug() << "NO!";
        return;
    }

    //TODO fix me
    watcher->deleteLater();
    QDBusPendingReply<QVariant> reply = *watcher;
    if (reply.isError()) {
        qWarning() << "ERROR" << reply.error().message();
        return;
    }

    ServiceManager::ServiceStatus newStatus;
    if(reply.value() == "active") {
        newStatus = ServiceStatus::STARTED;
    } else {
        newStatus = ServiceStatus::NOT_STARTED;
    }
    if(newStatus != m_currentStatus) {
        m_currentStatus = newStatus;
        emit currentStatusChanged();
    }
}

void ServiceManager::installServies()
{
    if(m_rootPassword.isEmpty()) {
        qWarning() << "Empty root passrord";
        return;
    }
    qDebug() << Q_FUNC_INFO;

    /*install services*/
    //FIXME need adapt to nonclick package
    QProcess *serviceInstallProcess = new QProcess();
    serviceInstallProcess->start("/bin/bash" , QStringList());
    serviceInstallProcess->write(QString("echo '%1' | sudo -S cp /opt/click.ubuntu.com/hideme.ubports/current/hideme.service /etc/systemd/system/\n").arg(m_rootPassword).toUtf8());


    /*daemons reload*/
    QProcess *daemoReloadProcess = new QProcess();
    daemoReloadProcess->start("/bin/bash" , QStringList());
    daemoReloadProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl daemon-reload\n").arg(m_rootPassword).toUtf8());
}

void ServiceManager::startServie()
{
    if(m_rootPassword.isEmpty()) {
        qWarning() << "Empty root passrord";
        return;
    } else {
        qDebug() << Q_FUNC_INFO << "Service start";
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl restart hideme\n").arg(m_rootPassword).toUtf8());
}

bool ServiceManager::cliAvailable() const
{
    return m_cliAvailable;
}

void ServiceManager::initServiceSetup()
{
    if(m_currentStatus != ServiceStatus::STARTED || m_accessToken.isEmpty()) {
        qDebug() << "NO! NO!";
        return;
    }

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(m_url).arg(m_port)));
    request.setRawHeader("Content-Type", "application/json; charset=UTF-8");

    QJsonObject obj;
    obj["AccessToken"] = m_accessToken;
    obj["Username"] = m_settings->value("user").toString();
    obj["Password"] = m_settings->value("password").toString();
#ifdef WITH_CLICK
    obj["CA"] = QCoreApplication::applicationDirPath() + "/CA.pem";
#else
    obj["CA"] = "/usr/share/hideme/CA.pem";
#endif
    obj["Host"] = "nl.hideservers.net";

    QJsonObject restObj;
    restObj["Rest"] = obj;

    QJsonDocument doc(restObj);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = mgr->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &ServiceManager::initServiceSetupHandler);
}

void ServiceManager::initServiceSetupHandler()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) {
        return;
    }

    qDebug() << reply->readAll();
}

void ServiceManager::loadServiceConfig()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/configuration").arg(m_url).arg(m_port)));
    QNetworkReply *reply = mgr->get(request);

    connect(reply, &QNetworkReply::finished, this, &ServiceManager::loadServiceConfigHandler);
}

void ServiceManager::loadServiceConfigHandler()
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

void ServiceManager::startServiceHandler()
{
    qDebug() << m_serviceProcess->readAllStandardOutput();
}

ServiceManager::ServiceStatus ServiceManager::currentStatus() const
{
    return m_currentStatus;
}

QString ServiceManager::rootPassword() const
{
    return m_rootPassword;
}

void ServiceManager::setRootPassword(const QString &newRootPassword)
{
    qDebug() << Q_FUNC_INFO << newRootPassword;
    if (m_rootPassword == newRootPassword)
        return;
    m_rootPassword = newRootPassword;
    emit rootPasswordChanged();
}
