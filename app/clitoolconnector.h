#ifndef CLITOOLCONNECTOR_H
#define CLITOOLCONNECTOR_H

#include <QObject>
#include <QProcess>
#include <QSettings>

class CliToolConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool isLogined READ isLogined NOTIFY isLoginedChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)

public:
    CliToolConnector(QObject* parent = nullptr);
    virtual ~CliToolConnector();
    bool cliAvailable() const;

    void getTokenRequest(QString user, QString password);
    void getToken(QString user, QString password);
    bool connected() {return m_connected; }

    Q_INVOKABLE void makeConnection(QString sudoPass);
    Q_INVOKABLE void disconnecting(QString sudoPass);

    Q_INVOKABLE QString getDebugOut() {return m_debugOutput.join("\n");}
    QString programString() {return m_program;}

    bool isLogined() const;

    bool isReady() const;

private slots:
    void getTokenRequestHandler();
    void initServiceSetupHandler();
    void initServiceSetup();

signals:
    void loginFailed();
    void loginSuccess();

    void startConnecting();
    void dissconnected();

    void connectedChanged();
    void cliStarted();

    void isLoginedChanged();

    void isReadyChanged();

private:
    bool m_cliAvailable;
    QString m_program;
    QString m_caPath;
    QStringList m_debugOutput;
    QString m_dataDir;
    QString m_errorMessage;

    QSettings* m_settings;

    QString m_userName;
    QString m_password;
    bool m_connected;
    bool m_isReady;
};

#endif // CLITOOLCONNECTOR_H
