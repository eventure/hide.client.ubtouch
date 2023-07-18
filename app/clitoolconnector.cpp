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
    #if defined(__aarch64__) || defined(_M_ARM64)
    , m_program(QCoreApplication::applicationDirPath()+"/lib/aarch64-linux-gnu/bin/hide.me")
    #elif defined(__x86_64__) || defined(_M_X64)
    , m_program(QCoreApplication::applicationDirPath()+"/lib/x86_64-linux-gnu/bin/hide.me")
    #endif
#else
    , m_program("/usr/bin/hide.me")
#endif
{
    qDebug() << m_program;
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

void CliToolConnector::getToken(QString user, QString password)
{
    if(!m_cliAvailable) {
        qCritical() << "CLI not avaiable" << m_program;
        return;
    }
    m_userName = user;
    m_password = password;

    QString server = m_settings->value("server", "de.hideservers.net").toString();

    if(m_userName.isEmpty() || m_password.isEmpty()) {
        qCritical() << "user or password is empty";
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PWD", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)); // Add an environment variable
    m_cli->setProcessEnvironment(env);

    QStringList arguments;
    arguments << m_baseArgumets << "-u" << m_userName << "-P" << m_password << "token" << server << "-f" << m_accessTokenFile ;
    QProcess process;
    process.start(m_program, arguments);

    process.waitForFinished();

    QStringList output = QString(process.readAllStandardOutput()).split("\n");

    qDebug() << output;

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

            m_settings->setValue("user", m_userName);
            m_settings->setValue("password", m_password);
            m_settings->sync();
        }
        qDebug() << "Success";
        emit loginSuccess();
    } else {
        qDebug() << "Failed";
        emit loginFailed();
    }
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

            m_settings->setValue("user", m_userName);
            m_settings->setValue("password", m_password);
            m_settings->sync();
        }
        emit loginSuccess();
    } else {
        qDebug() << "Failed";
        emit loginFailed();
    }
}
