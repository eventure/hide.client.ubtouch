#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "socektconnector.h"

#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QObject>
#include <QProcess>
#include "settings.h"

class ServiceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)
    Q_PROPERTY(bool startOnBoot READ startOnBoot WRITE setStartOnBoot NOTIFY startOnBootChanged)
    Q_PROPERTY(QString rootPassword READ rootPassword WRITE setRootPassword NOTIFY rootPasswordChanged)
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

    Q_INVOKABLE void installServies();
    Q_INVOKABLE void startServie();

    Q_INVOKABLE void setAccessToken(QString token);
    void checkServerStatus(QDBusPendingCallWatcher* watcher);

    bool cliAvailable() const;

    ServiceStatus currentStatus() const;

    QString rootPassword() const;
    void setRootPassword(const QString &newRootPassword);

    bool startOnBoot() const;
    void setStartOnBoot(bool newStartOnBoot);

signals:
    void currentStatusChanged();
    void rootPasswordChanged();

    void startOnBootChanged();

private slots:
    void startServiceHandler();

    void onServiceFileChanged(const QString &path);
    void propertiesChanged(const QString&, const QVariantMap& properties, const QStringList&);
    void socketCodeChangedHandler();

private:
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);

    Settings* m_settings;
    QProcess* m_serviceProcess;

    QString m_program;
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
