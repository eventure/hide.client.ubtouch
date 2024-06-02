#ifndef SYSTEMDMANAGER_H
#define SYSTEMDMANAGER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QCryptographicHash>

class SystemDManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rootPassword READ rootPassword WRITE setRootPassword NOTIFY rootPasswordChanged)

public:
    enum SystemDServiceStatus{
        NOT_INSTALLED = 1,
        NOT_STARTED,
        STARTED,
        UNKNOW = 256
    };

    SystemDManager(const QString &serviceName, QObject *parent = nullptr);
    virtual ~SystemDManager();

    bool installServiceFile(const QString currentPath = "");
    bool removeServiceFile();

    void startService();
    void stopService();
    void restartService();

    void startOnBoot(bool start);
    bool isStartOnBoot() const;

    SystemDServiceStatus currentStatus() {return m_currentStatus;}

    QString rootPassword() const;
    void setRootPassword(const QString &newRootPassword);

signals:
    void serviceRunningChanged();
    void serviceStatusChanged();

    void rootPasswordChanged();

private slots:
    void onServiceDirChanged(const QString &path);
    void checkServerStatus(QDBusPendingCallWatcher* watcher);
    void propertiesChanged(const QString&, const QVariantMap& properties, const QStringList&);

private:
    void calcServiceStatus();
    void dbusConnect();
    void dbusDisconnect();
    bool isServiceFileInstalled() const;
    bool isServiceFileIsActual(const QString actualFilePath = "") const;
    bool isServiceRunning();

    QString servicePath() const;
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm = QCryptographicHash::Md5) const;

    QString m_serviceName;
    const QString m_systemDDirPath;
    const QString m_systemDTarget;
    QDBusInterface m_systemdInterface;
    QDBusConnection m_dBusConnection;
    QDBusPendingCallWatcher* m_msgWatcher;

    SystemDServiceStatus m_currentStatus;
    QString m_rootPassword;
};
#endif // SYSTEMDMANAGER_H
