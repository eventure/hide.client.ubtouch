#include "systemdmanager.h"

#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QDBusReply>
#include <QProcess>

#include "constants.h"
#include "logging.h"

SystemDManager::SystemDManager(const QString &serviceName, QObject *parent)
    : QObject{parent}
    , m_serviceName(serviceName)
#ifdef SYSTEMD_WITH_ROOT
    , m_systemDDirPath("/etc/systemd/system/")
    , m_systemDTarget("graphical")
    , m_systemdInterface(s_serviceName, s_dbusPath, s_managerIface, QDBusConnection::systemBus())
    , m_dBusConnection(QDBusConnection::systemBus())
#else
    , m_systemDDirPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/systemd/user/")
    , m_systemDTarget("ubuntu-touch-session")
    , m_systemdInterface(s_serviceName, s_dbusPath, s_managerIface, QDBusConnection::sessionBus())
    , m_dBusConnection(QDBusConnection::sessionBus())
#endif
{
    if (!m_systemdInterface.isValid()) {
        qFatal("Failed to connect to systemd bus!");
    }

    if (!m_dBusConnection.isConnected()) {
        qFatal("Can't connect to dbus");
    }

    QFileSystemWatcher *serviceWatcher = new QFileSystemWatcher();
    serviceWatcher->addPath(m_systemDDirPath);
    connect(serviceWatcher, &QFileSystemWatcher::directoryChanged, this, &SystemDManager::onServiceDirChanged);

    if(isServiceFileInstalled()) {
        dbusConnect();
    }

    calcServiceStatus();
}

SystemDManager::~SystemDManager()
{
}

bool SystemDManager::isServiceFileInstalled() const
{
    if(!isServiceFileIsActual(
                  #ifdef WITH_CLICK
                          "/opt/click.ubuntu.com/hideme.eventure/current/hideme.service"
                  #else
                          "/usr/share/hideme/hideme.service"
                  #endif
                )) {
        return false;
    }
    return QFile::exists(servicePath());
}

bool SystemDManager::isServiceFileIsActual(const QString actualFilePath) const
{
    if(!QFile::exists(actualFilePath)) {
        Logging::instance()->add("actial " + actualFilePath + " file patch not exists");
        return false;
    }
    QByteArray systemServiceHash = fileChecksum(servicePath());
    QByteArray applicationServiceHash = fileChecksum(actualFilePath);
    if(systemServiceHash == applicationServiceHash) {
        Logging::instance()->add("File is actual");
        return true;
    }
    Logging::instance()->add("File is NOT actual");
    return false;
}

bool SystemDManager::isServiceRunning()
{
    QString status;
    QDBusReply<QDBusObjectPath> unitPath = m_systemdInterface.call("GetUnit", m_serviceName + ".service");

    QDBusInterface serviceInterface("org.freedesktop.systemd1",
                                    unitPath.value().path(),
                                    "org.freedesktop.DBus.Properties",
#ifdef SYSTEMD_WITH_ROOT
                                    QDBusConnection::systemBus());
#else
                                    QDBusConnection::sessionBus());
#endif

    QDBusReply<QDBusVariant> activeState = serviceInterface.call("Get", "org.freedesktop.systemd1.Unit", "ActiveState");
    status = activeState.value().variant().toString();

    if (status == "active") {
        return true;
    } else {
        return false;
    }
    // We shouldn't be here, but if we are, something went wrong...
    return false;
}

bool SystemDManager::installServiceFile(const QString currentPath)
{
    if(currentPath.isEmpty() || !QFile::exists(currentPath)) {
        return false;
    }

#ifdef SYSTEMD_WITH_ROOT
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add("Empty root passrord");
        return false;
    }
    QProcess *serviceInstallProcess = new QProcess();
    serviceInstallProcess->start("/bin/bash" , QStringList());
    serviceInstallProcess->write(QString("echo '%1' | sudo -S cp %2 %3 && /usr/bin/systemctl daemon-reload\n").arg(m_rootPassword).arg(currentPath).arg(m_systemDDirPath).toUtf8());
#else
    if(QFile::exists(servicePath())) {
        QFile::remove(servicePath());
    }

    QDir().mkpath(m_systemDDirPath);

    if(!QFile::copy(currentPath, servicePath())) {
        Logging::instance()->add("can't copy service from " + currentPath + " to " + servicePath());
        return false;
    }

    // Reload the systemd daemon
    m_systemdInterface.call("Reload");
#endif
    dbusConnect();
    calcServiceStatus();
    return true;
}

bool SystemDManager::removeServiceFile()
{
#ifdef SYSTEMD_WITH_ROOT
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return false;
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    bool out = myProcess->write(QString("echo '%1' | sudo -S /bin/rm -f %2 && /usr/bin/systemctl daemon-reload\n").arg(m_rootPassword).arg(servicePath()).toUtf8());
#else
    bool out = QFile::remove(servicePath());
    // Reload the systemd daemon
    m_systemdInterface.call("Reload");
#endif
    m_currentStatus = SystemDServiceStatus::NOT_INSTALLED;
    dbusDisconnect();
    emit serviceStatusChanged();
    return out;
}

void SystemDManager::startService()
{
#ifdef SYSTEMD_WITH_ROOT
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return;
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl start %2\n").arg(m_rootPassword).arg(m_serviceName).toUtf8());
#else
    m_systemdInterface.call("StartUnit", m_serviceName + ".service", "fail");
#endif
    Logging::instance()->add( "Service start" );
}

void SystemDManager::stopService()
{
#ifdef SYSTEMD_WITH_ROOT
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return;
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl stop %2\n").arg(m_rootPassword).arg(m_serviceName).toUtf8());
#else
    m_systemdInterface.call("StopUnit", m_serviceName + ".service", "fail");
#endif
    Logging::instance()->add( "Service stop" );
}

void SystemDManager::restartService()
{
#ifdef SYSTEMD_WITH_ROOT
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return;
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl restart %2\n").arg(m_rootPassword).arg(m_serviceName).toUtf8());
#else
    m_systemdInterface.call("RestartUnit", m_serviceName + ".service", "fail");
#endif
    Logging::instance()->add( "Service restart" );
}

void SystemDManager::startOnBoot(bool start)
{
    QString action = "disable";
    if(start) {
        action = "enable";
    }
#ifdef SYSTEMD_WITH_ROOT
    if(m_rootPassword.isEmpty()) {
        Logging::instance()->add( "Empty root passrord" );
        return;
    }

    QProcess *myProcess = new QProcess();
    myProcess->start("/bin/bash" , QStringList());
    myProcess->write(QString("echo '%1' | sudo -S /usr/bin/systemctl %2 %3\n").arg(m_rootPassword).arg(action).arg(m_serviceName).toUtf8());
#else
    if(start) {
        if (!QDir(m_systemDDirPath + m_systemDTarget + ".target.wants").exists()) {
            Logging::instance()->add("creating systemd directory");
            QDir().mkpath(m_systemDDirPath + m_systemDTarget + ".target.wants");
        }

        // Create a link for systemd service to enable automatically
        if(!QFile::link(m_systemDDirPath + m_serviceName + ".service",
                        m_systemDDirPath + m_systemDTarget + ".target.wants/" + m_serviceName + ".service")) {
            Logging::instance()->add("can't link service");
        }

    } else {
        QFile::remove(m_systemDDirPath + m_systemDTarget + ".target.wants/" + m_serviceName + ".service");
    }

    m_systemdInterface.call("Reload");
#endif
    Logging::instance()->add("Service " + action + " on boot");
}

bool SystemDManager::isStartOnBoot() const
{
    return QFile::exists(m_systemDDirPath + m_systemDTarget + ".target.wants/" + m_serviceName + ".service");
}

void SystemDManager::onServiceDirChanged(const QString &path)
{
    Q_UNUSED(path);
    calcServiceStatus();
}

void SystemDManager::checkServerStatus(QDBusPendingCallWatcher *watcher)
{
    watcher->deleteLater();
    QDBusPendingReply<QVariant> reply = *watcher;

    if (reply.isError()) {
        Logging::instance()->add("ERROR" + reply.error().message());
        return;
    }

    Logging::instance()->add("Service status is " + reply.value().toString());
    calcServiceStatus();
}

void SystemDManager::propertiesChanged(const QString &, const QVariantMap &properties, const QStringList &)
{
    calcServiceStatus();
}

void SystemDManager::calcServiceStatus()
{
    SystemDServiceStatus newStatus = SystemDServiceStatus::NOT_INSTALLED;
    if(!isServiceFileInstalled()) {
        Logging::instance()->add("Service file not installed");
        newStatus = SystemDServiceStatus::NOT_INSTALLED;
    } else if(!isServiceFileIsActual(
              #ifdef WITH_CLICK
                      "/opt/click.ubuntu.com/hideme.eventure/current/hideme.service"
              #else
                      "/usr/share/hideme/hideme.service"
              #endif
      )) {
        m_currentStatus = SystemDServiceStatus::NOT_INSTALLED;
        Logging::instance()->add("NOT ACTUAL!!!");
    } else if(isServiceRunning()) {
        Logging::instance()->add("Service is started");
        newStatus = SystemDServiceStatus::STARTED;
    } else {
        Logging::instance()->add("Service not started");
        newStatus = SystemDServiceStatus::NOT_STARTED;
    }

    if(newStatus != m_currentStatus) {
        m_currentStatus = newStatus;
    }
    emit serviceStatusChanged();
}

void SystemDManager::dbusConnect()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
                s_serviceName,
                s_dbusPath+"/unit/"+m_serviceName+"_2eservice",
                s_propertiesIface,
                "Get");
    msg.setArguments({ s_unitIface, s_propertyActiveState });
    QDBusPendingReply<QVariant> msgPending = QDBusConnection::systemBus().asyncCall(msg);
    m_msgWatcher = new QDBusPendingCallWatcher(msgPending);
    connect(m_msgWatcher, &QDBusPendingCallWatcher::finished, this, &SystemDManager::checkServerStatus);


    m_dBusConnection.connect(
                    QString(),
                    s_dbusPath+"/unit/"+m_serviceName+"_2eservice",
                    s_propertiesIface,
                    "PropertiesChanged",
                    this,
                    SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

void SystemDManager::dbusDisconnect()
{
    disconnect(m_msgWatcher, &QDBusPendingCallWatcher::finished, this, &SystemDManager::checkServerStatus);
    m_dBusConnection.disconnect(
                    QString(),
                    s_dbusPath+"/unit/"+m_serviceName+"_2eservice",
                    s_propertiesIface,
                    "PropertiesChanged",
                    this,
                    SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

QByteArray SystemDManager::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm) const
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

QString SystemDManager::servicePath() const
{
    return m_systemDDirPath + m_serviceName + ".service";
}

QString SystemDManager::rootPassword() const
{
    return m_rootPassword;
}

void SystemDManager::setRootPassword(const QString &newRootPassword)
{
    if (m_rootPassword == newRootPassword)
        return;
    m_rootPassword = newRootPassword;
    emit rootPasswordChanged();
}
