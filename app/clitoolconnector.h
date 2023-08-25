#ifndef CLITOOLCONNECTOR_H
#define CLITOOLCONNECTOR_H

#include <QObject>
#include <QProcess>
#include <QSettings>

class CliToolConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLogined READ isLogined NOTIFY isLoginedChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY isReadyChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)

public:
    CliToolConnector(QObject* parent = nullptr);
    virtual ~CliToolConnector();

    Q_INVOKABLE void setLoginPass(const QString usermame, const QString password);
    Q_INVOKABLE void getTokenRequest();
    Q_INVOKABLE void makeConnection();
    Q_INVOKABLE void makeDisconnection();
    bool connected() {return m_connected; }

    Q_INVOKABLE QString getDebugOut() {return m_debugOutput.join("\n");}

    bool isLogined() const;
    bool isReady() const;

    QString token() const;

private slots:
    void getTokenRequestHandler();
    void requestHandler();

signals:
    void loginFailed();
    void loginSuccess();

    void connectedChanged();
    void isLoginedChanged();
    void isReadyChanged();

    void urlChanged();
    void portChanged();
    void error(QString title, QString message);

    void tokenChanged();

private:
    QString m_caPath;
    QStringList m_debugOutput;
    QString m_errorMessage;

    QSettings* m_settings;

    QString m_userName;
    QString m_password;
    bool m_connected;
    bool m_isReady;
    QString m_token;
};

#endif // CLITOOLCONNECTOR_H
