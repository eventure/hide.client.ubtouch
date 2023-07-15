#ifndef CLITOOLCONNECTOR_H
#define CLITOOLCONNECTOR_H

#include <QObject>
#include <QProcess>

class CliToolConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool cliAvailable READ cliAvailable)

public:
    CliToolConnector(QObject* parent = nullptr);
    bool cliAvailable() const;

    void getToken();
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
};

#endif // CLITOOLCONNECTOR_H
