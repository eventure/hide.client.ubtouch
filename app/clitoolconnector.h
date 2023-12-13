#ifndef CLITOOLCONNECTOR_H
#define CLITOOLCONNECTOR_H

#include <QObject>
#include <QProcess>
#include "settings.h"
class CliToolConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLogined READ isLogined NOTIFY isLoginedChanged)
    Q_PROPERTY(bool isServiceReady READ isServiceReady WRITE setIsServiceReady NOTIFY isServiceReadyChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)
    Q_PROPERTY(QString defaultHostName READ defaultHostName WRITE setDefaultHostName NOTIFY defaultHostNameChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)

public:
    CliToolConnector(QObject* parent = nullptr);
    virtual ~CliToolConnector();

    Q_INVOKABLE void setLoginPass(const QString usermame, const QString password);
    Q_INVOKABLE void setParam(const QString param, const QString value);
    Q_INVOKABLE void getTokenRequest();
    Q_INVOKABLE void makeConnection();
    Q_INVOKABLE void makeDisconnection();
    Q_INVOKABLE void makeRoute();
    Q_INVOKABLE void initServiceSetup();
    Q_INVOKABLE bool isDefaultServer(QString hostname);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void quit();
    bool connected() {return m_connected; }

    Q_INVOKABLE QString getDebugOut() {return m_debugOutput.join("\n");}

    bool isLogined() const;

    QString token() const;

    QString defaultHostName() const;
    void setDefaultHostName(const QString &newDefaultHostName);

    QString hostName() const;
    void setHostName(const QString &newHostName);

    bool isServiceReady() const;
    void setIsServiceReady(bool newIsServiceReady);

private slots:
    void getTokenRequestHandler();
    void requestHandler();
    void setParamRequestHandler();

    void loadServiceConfig();
    void loadServiceConfigHandler();

    void initServiceSetupHandler();

signals:
    void loginFailed();
    void loginSuccess();

    void connectedChanged();
    void isLoginedChanged();

    void urlChanged();
    void portChanged();
    void error(QString title, QString message);

    void tokenChanged();

    void defaultHostNameChanged();

    void hostNameChanged();

    void isServiceReadyChanged();
    void setupServiceFail();

private:
    QString m_caPath;
    QStringList m_debugOutput;
    QString m_errorMessage;

    Settings* m_settings;

    QString m_userName;
    QString m_password;
    bool m_connected;
    QString m_token;
    QString m_hostName;
    bool m_isServiceReady;

    QString m_url;
    int m_port;
};

#endif // CLITOOLCONNECTOR_H
