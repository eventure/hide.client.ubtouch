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
    Q_INVOKABLE void changeFavorite(int serverId);
    Q_INVOKABLE bool isFavoriteServer(int serverId);
    Q_INVOKABLE bool isDefaultServer(QString hostname);
    bool connected() {return m_connected; }

    Q_INVOKABLE QString getDebugOut() {return m_debugOutput.join("\n");}

    bool isLogined() const;
    bool isReady() const;

    QString token() const;

    QString defaultHostName() const;
    void setDefaultHostName(const QString &newDefaultHostName);

    QString hostName() const;
    void setHostName(const QString &newHostName);

private slots:
    void getTokenRequestHandler();
    void requestHandler();
    void setParamRequestHandler();

    void loadServiceConfig();
    void loadServiceConfigHandler();

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

    void defaultHostNameChanged();

    void hostNameChanged();

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
    QString m_hostName;
};

#endif // CLITOOLCONNECTOR_H
