#ifndef SYSTEMDMANAGER_H
#define SYSTEMDMANAGER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QCryptographicHash>

class SystemDManager : public QObject
{
    Q_OBJECT
public:
    enum SystemDServiceStatus{
        NOT_INSTALLED,
        NOT_STARTED,
        STARTED
    };

    SystemDManager(const QString &serviceName, QObject *parent = nullptr);

    bool installServiceFile(const QString currentPath = "");
    bool removeServiceFile();

    void startService();
    void stopService();
    void restartService();

    void startOnBoot(bool start);
    bool isStartOnBoot() const;

    SystemDServiceStatus currentStatus() {return m_currentStatus;}

signals:
    void serviceRunningChanged();
    void serviceStatusChanged();

private slots:
    void onServiceDirChanged(const QString &path);
    void checkServerStatus(QDBusPendingCallWatcher* watcher);
    void propertiesChanged(const QString&, const QVariantMap& properties, const QStringList&);

private:
    void calcServiceStatus();
    bool serviceFileInstalled() const;
    bool serviceFileIsActual(const QString actualFilePath = "") const;
    bool serviceRunning();

    QString servicePath() const;
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm = QCryptographicHash::Md5) const;

    QString m_serviceName;
    QDBusInterface m_systemdInterface;
    QDBusConnection m_sessionDBusConnection;

    SystemDServiceStatus m_currentStatus;
};
#endif // SYSTEMDMANAGER_H
