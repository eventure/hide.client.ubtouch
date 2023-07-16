#include "clitoolconnector.h"

#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QSettings>
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
    QFile cli(m_program);
    m_cliAvailable = cli.exists();
    m_cli = new QProcess(this);

    connect(m_cli, &QProcess::readyReadStandardOutput, this, &CliToolConnector::getTokenHandler);
#ifdef WITH_CLICK
    m_baseArgumets << "-ca" << "/CA.pem";
#else
    m_baseArgumets << "-ca" << "/usr/share/hideme/CA.pem";
#endif
    QDir dataLocation(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if(!dataLocation.exists()) {
        dataLocation.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    }

    m_accessTokenFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/accessToken.txt";
}

bool CliToolConnector::cliAvailable() const
{
    return m_cliAvailable;
}

void CliToolConnector::getToken()
{
    if(!m_cliAvailable) {
        return;
    }

    QSettings settings;
    QString user = settings.value("user").toString();
    QString pass = settings.value("pass").toString();
    QString server = settings.value("server", "de.hideservers.net").toString();

    if(user.isEmpty() || pass.isEmpty()) {
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
