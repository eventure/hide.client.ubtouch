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

private slots:
    void getTokenHandler();
    void onCliToolStarted();
    void getTokenRequestHandler();

signals:
    void loginFailed();
    void loginSuccess();

    void startConnecting();
    void dissconnected();

    void connectedChanged();
    void cliStarted();

private:
    bool m_cliAvailable;
    QProcess *m_cli;
    QString m_program;
    QStringList m_baseArgumets;
    QStringList m_debugOutput;
    QString m_dataDir;
    QString m_errorMessage;

    QSettings* m_settings;

    QString m_userName;
    QString m_password;
    bool m_connected;
    uint m_cliToolPID;
};

#endif // CLITOOLCONNECTOR_H
