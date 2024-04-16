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
#include <QDir>

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
    , m_sysDmanager(new SystemDManager("hideme"))
{
    m_serviceProcess = new QProcess(this);

    m_connector = new SocektConnector(m_settings->value("url", "127.0.0.1").toString()
                                      , m_settings->value("port", 5050).toInt() , this);
    connect(m_connector, &SocektConnector::codeChanged, this, &ServiceManager::socketCodeChangedHandler);

    m_connector->start();

    QFile cli(m_program);
    Logging::instance()->add(m_program);
    m_cliAvailable = cli.exists();

    if(!m_sysDmanager->serviceFileInstalled()) {
        Logging::instance()->add("Service file not installed");
        m_currentStatus = ServiceStatus::NOT_INSTALLED;
        Logging::instance()->add("NOT INSTALLED!!!");
    } else if(!m_sysDmanager->serviceFileIsActual(
                          #ifdef WITH_CLICK
                                  "/opt/click.ubuntu.com/hideme.ubports/current/hideme.service"
                          #else
                                  "/usr/share/hideme/hideme.service"
                          #endif
                  ))
    {
        m_currentStatus = ServiceStatus::NOT_INSTALLED;
        Logging::instance()->add("NOT ACTUAL!!!");
    } else {
        m_currentStatus = ServiceStatus::NOT_STARTED;
        Logging::instance()->add("NOT STARTED!!!");

        if(m_sysDmanager->serviceRunning()) {
            m_currentStatus = ServiceStatus::STARTED;
        }
    }

    connect(m_sysDmanager, &SystemDManager::serviceFileInstalledChanged, this, &ServiceManager::currentStatusChanged);
    connect(m_sysDmanager, &SystemDManager::serviceStatusChanged, this, &ServiceManager::onServiceStatusChanged);
}

ServiceManager::~ServiceManager()
{
}

void ServiceManager::socketCodeChangedHandler()
{
    if(m_currentStatus == ServiceManager::NOT_INSTALLED || m_currentStatus == ServiceManager::NOT_STARTED) {
        return;
    }

    QString state = m_connector->code();
    ServiceManager::ServiceStatus newStatus;

    if(state.isEmpty()) {
        newStatus = ServiceStatus::NOT_STARTED;
    } else if(state == "routed") {
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

void ServiceManager::onServiceStatusChanged()
{
    Logging::instance()->add("Serive status changed!!!");

    ServiceManager::ServiceStatus newStatus;
    if(m_sysDmanager->serviceRunning()) {
        newStatus = ServiceStatus::STARTED;
    } else {
        newStatus = ServiceManager::NOT_STARTED;
    }

    if(newStatus != m_currentStatus) {
        m_currentStatus = newStatus;
        emit currentStatusChanged();
    }
}

void ServiceManager::installServies()
{
    m_sysDmanager->installServiceFile(
            #ifdef WITH_CLICK
                "/opt/click.ubuntu.com/hideme.ubports/current/hideme.service"
            #else
                "/usr/share/hideme/hideme.service"
            #endif
                );
}

void ServiceManager::startServie()
{
    Logging::instance()->add("Start service");
    m_sysDmanager->startService();
}

void ServiceManager::stopServie()
{
    Logging::instance()->add("Stop service");
    m_sysDmanager->stopService();
}

bool ServiceManager::cliAvailable() const
{
    return m_cliAvailable;
}

ServiceManager::ServiceStatus ServiceManager::currentStatus() const
{
    return m_currentStatus;
}

bool ServiceManager::startOnBoot() const
{
    return m_sysDmanager->isStartOnBoot();
}

void ServiceManager::setStartOnBoot(bool newStartOnBoot)
{
    m_sysDmanager->startOnBoot(newStartOnBoot);
}
