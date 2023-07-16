#include "clitoolconnector.h"

#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

CliToolConnector::CliToolConnector(QObject *parent)
    : QObject(parent)
#ifdef WITH_CLICK
    , m_program(QCoreApplication::applicationDirPath()+"/hide.me")
#else
    , m_program("/usr/bin/hide.me")
#endif
{
    m_settings = new QSettings("hideconfig.ini");
    QFile cli(m_program);
    m_cliAvailable = cli.exists();
    m_cli = new QProcess(this);

    connect(m_cli, &QProcess::readyReadStandardOutput, this, &CliToolConnector::getTokenHandler);
#ifdef WITH_CLICK
    m_baseArgumets << "-ca" << QCoreApplication::applicationDirPath() + "/CA.pem";
#else
    m_baseArgumets << "-ca" << "/usr/share/hideme/CA.pem";
#endif
    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    m_accessTokenFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/accessToken.txt";

    qDebug() << m_accessTokenFile;
}

bool CliToolConnector::cliAvailable() const
{
    return m_cliAvailable;
}

void CliToolConnector::getToken()
{
    if(!m_cliAvailable) {
        qCritical() << "CLI not avaiable" << m_program;
        return;
    }

    QString user = m_settings->value("user").toString();
    QString pass = m_settings->value("pass").toString();
    QString server = m_settings->value("server", "de.hideservers.net").toString();

    if(user.isEmpty() || pass.isEmpty()) {
        qWarning() << "Settings is empty";
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PWD", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)); // Add an environment variable
    m_cli->setProcessEnvironment(env);

    QStringList arguments;
    arguments << m_baseArgumets << "-u" << user << "-P" << pass << "token" << server << "-f" << m_accessTokenFile ;
    m_cli->start(m_program, arguments);
}

void CliToolConnector::getTokenHandler()
{
    QStringList output = QString(m_cli->readAllStandardOutput()).split("\n");
    QString errorMessage;
    foreach (QString line, output) {
        if(line.startsWith("Main")) {
            if(line.contains("[ERR]")) {
                errorMessage = line.split("[ERR]").last();
            }
        }
    }

    if(errorMessage.isEmpty()) {
        QFile localAccessToken("accessToken.txt");
        if(localAccessToken.exists()) {
            localAccessToken.copy(m_accessTokenFile);
            localAccessToken.remove();
        }
        emit loginSuccess();
    } else {
        qDebug() << "Failed";
        emit loginFailed();
    }
}
