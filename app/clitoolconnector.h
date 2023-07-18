#ifndef CLITOOLCONNECTOR_H
#define CLITOOLCONNECTOR_H

#include <QObject>
#include <QProcess>
#include <QSettings>

class CliToolConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)

public:
    CliToolConnector(QObject* parent = nullptr);
    bool cliAvailable() const;

    void getToken(QString user, QString password);
private slots:
    void getTokenHandler();

signals:
    void loginFailed();
    void loginSuccess();

private:
    bool m_cliAvailable;
    QProcess *m_cli;
    QString m_program;
    QStringList m_baseArgumets;
    QString m_accessTokenFile;

    QSettings* m_settings;

    QString m_userName;
    QString m_password;
};

#endif // CLITOOLCONNECTOR_H
