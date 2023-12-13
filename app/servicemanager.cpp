#include "constants.h"
#include "servicemanager.h"
#include "socektconnector.h"
#include "logging.h"

#include <QFile>
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
    , m_settings(new Settings("hideconfig.ini"))
    , m_currentStatus(ServiceStatus::UNKNOW)
    , m_systemDBusConnection(QDBusConnection::systemBus())
{
    m_serviceProcess = new QProcess(this);

    m_connector = new SocektConnector(m_settings->value("url", "127.0.0.1").toString()
                                    , m_settings->value("port", 5050).toInt() , this);
    connect(m_connector, &SocektConnector::codeChanged, this, &ServiceManager::socketCodeChangedHandler);

    QFile cli(m_program);
    Logging::instance()->add(m_program);
    m_cliAvailable = cli.exists();

    if (!m_systemDBusConnection.isConnected()) {
        qFatal("Can't connect to system bus");
    }

    if(!QFile::exists("/etc/systemd/system/hideme.service")) {
        Logging::instance()->add("Service file not installed");
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
}

ServiceManager::~ServiceManager()
{
}

void ServiceManager::onServiceFileChanged(const QString &path)
{
    Logging::instance()->add(Q_FUNC_INFO + path);
    ServiceStatus currentStatus;

    if(!QFile::exists("/etc/systemd/system/hideme.service")) {
        Logging::instance()->add("System file not installed");
        currentStatus = ServiceStatus::NOT_INSTALLED;
    } else {
        Logging::instance()->add("System file installed");
        currentStatus = ServiceStatus::NOT_STARTED;
    }

    if(currentStatus != m_currentStatus) {
        m_currentStatus = currentStatus;
        emit currentStatusChanged();
    }

    emit startOnBootChanged();
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
        Logging::instance()->add("Service started");
    } else {
        newStatus = ServiceStatus::NOT_STARTED;
        Logging::instance()->add("Service not started");
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

        Logging::instance()->add("Current state is " + state);
    }
}

void ServiceManager::setAccessToken(QString token)
{
    if(!token.isEmpty() != m_accessToken) {
        m_accessToken = token;
    }
}

void ServiceManager::checkServerStatus(QDBusPendingCallWatcher* watcher)
{
    if(m_currentStatus == ServiceManager::NOT_INSTALLED) {
        Logging::instance()->add("Service not installed");
        return;
    }

    //TODO fix me
    watcher->deleteLater();
    QDBusPendingReply<QVariant> reply = *watcher;
    if (reply.isError()) {
        Logging::instance()->add("ERROR" + reply.error().message());
        return;
    }

    ServiceManager::ServiceStatus newStatus;
    Logging::instance()->add("Server status is" + reply.value().toString());

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
        Logging::instance()->add("Empty root passrord");
        return;
    }
    Logging::instance()->add( Q_FUNC_INFO );

    /*install services*/
    //FIXME need adapt to nonclick package
    QProcess *serviceInstallProcess = new QProcess();
    serviceInstallProcess->start("/bin/bash" , QStringList());
#ifdef WITH_CLICK
    serviceInstallProcess->write(QString("echo '%1' | sudo -S cp /opt/click.ubuntu.com/hideme.ubports/current/hideme.service /etc/systemd/system/\n").arg(m_rootPassword).toUtf8());
#else
    serviceInstallProcess->write(QString("echo '%1' | sudo -S cp /usr/share/hideme/hideme.service /etc/systemd/system/\n").arg(m_rootPassword).toUtf8());
#endif

    /*daemons reload*/
    QProcess *daemoReloadProcess = new QProcess();
    daemoReloadProcess->start("/bin/bash" , QStringList());
    daemoReloadProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl daemon-reload\n").arg(m_rootPassword).toUtf8());
}

void ServiceManager::startServie()
{
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return;
    } else {
        Logging::instance()->add( "Service start" );
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl restart hideme\n").arg(m_rootPassword).toUtf8());
}

bool ServiceManager::cliAvailable() const
{
    return m_cliAvailable;
}

void ServiceManager::startServiceHandler()
{
    Logging::instance()->add("startServiceHandler:" + m_serviceProcess->readAllStandardOutput());
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
    if (m_rootPassword == newRootPassword)
        return;
    m_rootPassword = newRootPassword;
    emit rootPasswordChanged();

/*Check and update service file*/
    if(QFile::exists("/etc/systemd/system/hideme.service")) {
        QByteArray systemServiceHash = fileChecksum("/etc/systemd/system/hideme.service", QCryptographicHash::Md5);
        QByteArray applicationServiceHash = fileChecksum(
#ifdef WITH_CLICK
                    "/opt/click.ubuntu.com/hideme.ubports/current/hideme.service"
#else
                    "/usr/share/hideme/hideme.service"
#endif
                    , QCryptographicHash::Md5);


        if(systemServiceHash != applicationServiceHash) {
            Logging::instance()->add("service file outdated. updating service file");
            installServies();
        }
    }
}

bool ServiceManager::startOnBoot() const
{
    return QFile("/etc/systemd/system/graphical.target.wants/hideme.service").exists();
}

void ServiceManager::setStartOnBoot(bool newStartOnBoot)
{
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return;
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    if(newStartOnBoot) {
        myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl enable hideme\n").arg(m_rootPassword).toUtf8());
        Logging::instance()->add( "Enable start service on boot" );
    } else {
        myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl disable hideme\n").arg(m_rootPassword).toUtf8());
        Logging::instance()->add( "Disable start service on boot" );
    }
    emit startOnBootChanged();

    /*daemons reload*/
    QProcess *daemoReloadProcess = new QProcess();
    daemoReloadProcess->start("/bin/bash" , QStringList());
    daemoReloadProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl daemon-reload\n").arg(m_rootPassword).toUtf8());
}

QByteArray ServiceManager::fileChecksum(const QString &fileName,
                        QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}
