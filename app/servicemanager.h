#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "socektconnector.h"

#include <QObject>
#include <QProcess>
#include "settings.h"
#include "systemdmanager.h"

class ServiceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)
    Q_PROPERTY(bool withRoot READ withRoot CONSTANT)
    Q_PROPERTY(bool startOnBoot READ startOnBoot WRITE setStartOnBoot NOTIFY startOnBootChanged)
    Q_PROPERTY(ServiceStatus currentStatus READ currentStatus NOTIFY currentStatusChanged)

public:
    enum ServiceStatus{
        NOT_INSTALLED = 1,
        NOT_STARTED,
        STARTED,
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        DISCONNECTED,
        UNKNOW = 256,
    };
    Q_ENUMS(ServiceStatus)

    ServiceManager(QObject* parent = nullptr);
    virtual ~ServiceManager();

    Q_INVOKABLE void installServices();
    Q_INVOKABLE void removeServices();
    Q_INVOKABLE void startService();
    Q_INVOKABLE void stopService();
#ifdef SYSTEMD_WITH_ROOT
    Q_INVOKABLE void setRootPassword(const QString &newRootPassword);
#endif
    bool withRoot() const;
    bool cliAvailable() const;

    ServiceStatus currentStatus() const;

    bool startOnBoot() const;
    void setStartOnBoot(bool newStartOnBoot);

signals:
    void currentStatusChanged();
    void startOnBootChanged();

private slots:
    void socketCodeChangedHandler();
    void onServiceStatusChanged();

private:
    Settings* m_settings;
    QProcess* m_serviceProcess;

    QString m_program;
    bool m_cliAvailable;
    bool m_connected;
    ServiceStatus m_currentStatus;
    SocektConnector* m_connector;
    SystemDManager* m_sysDmanager;
};
Q_DECLARE_METATYPE(ServiceManager::ServiceStatus)

#endif // SERVICEMANAGER_H
