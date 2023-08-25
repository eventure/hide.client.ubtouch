#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "socektconnector.h"

#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QObject>
#include <QProcess>
#include <QSettings>

static const QString s_servicePath = QStringLiteral("/org/freedesktop/systemd1/unit/hideme_2eservice");
static const QString s_serviceName = QStringLiteral("org.freedesktop.systemd1");
static const QString s_propertiesIface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString s_unitIface = QStringLiteral("org.freedesktop.systemd1.Unit");
static const QString s_propertyActiveState = QStringLiteral("ActiveState");

class ServiceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString rootPassword READ rootPassword WRITE setRootPassword NOTIFY rootPasswordChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(ServiceStatus currentStatus READ currentStatus NOTIFY currentStatusChanged)

public:
    enum ServiceStatus{
        UNKNOW,
        NOT_INSTALLED,
        NOT_STARTED,
        STARTED,
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        DISCONNECTED
    };
    Q_ENUMS(ServiceStatus)

    ServiceManager(QObject* parent = nullptr);
    virtual ~ServiceManager();

    QString url() const;
    void setUrl(const QString &newUrl);
    int port() const;
    void setPort(int newPort);

    Q_INVOKABLE void installServies();
    Q_INVOKABLE void startServie();

    Q_INVOKABLE void setAccessToken(QString token);
    void checkServerStatus(QDBusPendingCallWatcher* watcher);

    bool cliAvailable() const;

    ServiceStatus currentStatus() const;

    QString rootPassword() const;
    void setRootPassword(const QString &newRootPassword);

signals:
    void urlChanged();
    void portChanged();

    void currentStatusChanged();
    void rootPasswordChanged();

private slots:
    void initServiceSetup();
    void initServiceSetupHandler();
    void loadServiceConfig();
    void loadServiceConfigHandler();

    void startServiceHandler();

    void onServiceFileChanged(const QString &path);
    void propertiesChanged(const QString&, const QVariantMap& properties, const QStringList&);
    void socketCodeChangedHandler();

private:
    void makeRoute();

    QSettings* m_settings;
    QProcess* m_serviceProcess;

    QString m_program;
    QString m_url;
    int m_port;
    bool m_cliAvailable;
    bool m_connected;
    ServiceStatus m_currentStatus;
    QDBusConnection m_systemDBusConnection;
    QString m_rootPassword;
    QString m_accessToken;
    SocektConnector* m_connector;
};
Q_DECLARE_METATYPE(ServiceManager::ServiceStatus)

#endif // SERVICEMANAGER_H
