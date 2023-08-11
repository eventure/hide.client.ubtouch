#ifndef CLITOOLCONNECTOR_H
#define CLITOOLCONNECTOR_H

#include <QObject>
#include <QProcess>
#include <QSettings>

class CliToolConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool isLogined READ isLogined NOTIFY isLoginedChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)
    Q_PROPERTY(bool serverStaerted READ serverStaerted NOTIFY serverStaertedChanged)

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)

public:
    CliToolConnector(QObject* parent = nullptr);
    virtual ~CliToolConnector();
    bool cliAvailable() const;

    Q_INVOKABLE void getTokenRequest(QString user, QString password);
    bool connected() {return m_connected; }

    Q_INVOKABLE QString getDebugOut() {return m_debugOutput.join("\n");}
    Q_INVOKABLE void startService(QString sudoPass);
    Q_INVOKABLE void checkServerStatus();

    QString programString() {return m_program;}

    bool isLogined() const;
    bool isReady() const;

    void setConnected(bool newConnected);

    QString url() const;
    void setUrl(const QString &newUrl);

    int port() const;
    void setPort(int newPort);

    bool serverStaerted() const;

private slots:
    void getTokenRequestHandler();
    void initServiceSetupHandler();
    void loadServiceConfigHandler();
    void setConnectedHandler();

    void initServiceSetup();
    void loadServiceConfig();

    void checkServerStatusHandler();
    void startServiceHandler();

    void checkServerStatusProto();

signals:
    void loginFailed();
    void loginSuccess();

    void connectedChanged();
    void isLoginedChanged();
    void isReadyChanged();

    void urlChanged();
    void portChanged();
    void error(QString errorMessage);

    void serverStaertedChanged();

private:
    bool m_cliAvailable;
    QString m_program;
    QString m_caPath;
    QStringList m_debugOutput;
    QString m_dataDir;
    QString m_errorMessage;

    QSettings* m_settings;
    QProcess* m_serviceProcess;

    QString m_userName;
    QString m_password;
    bool m_connected;
    bool m_isReady;
    QString m_url;
    int m_port;
    bool m_serverStaerted;
};

#endif // CLITOOLCONNECTOR_H
