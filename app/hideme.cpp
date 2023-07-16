#include "hideme.h"
#include "config.h"

#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>

HideMe::HideMe(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_view(nullptr)
    , m_isLogined(false)
    , m_connected(false)
    , m_cliConnector(new CliToolConnector(this))
{
    setOrganizationName("Hideme");
    setOrganizationDomain("hide.com");
    setApplicationName("HideMe VPN");

    m_settings = new QSettings(this);

    m_isLogined = ( !m_settings->value("user").toString().isEmpty() && !m_settings->value("password").toString().isEmpty());

    connect(m_cliConnector, &CliToolConnector::loginFailed, this, &HideMe::onLoginFailed);
    connect(m_cliConnector, &CliToolConnector::loginSuccess, this, &HideMe::onLoginSucces);
}

HideMe::~HideMe()
{
    if (m_view) {
        delete m_view;
    }
}

bool HideMe::setup()
{
    m_view = new QQuickView();
    m_view->setColor(Qt::white);
    m_view->setResizeMode(QQuickView::SizeViewToRootObject);
    m_view->setFlags(static_cast<Qt::WindowFlags>(0x00800000));
    m_view->setTitle(tr("Hide me VPN"));

    m_view->rootContext()->setContextProperty("mApplication", this);
    m_view->rootContext()->setContextProperty("haveTools", m_cliConnector->cliAvailable());
    QUrl source(appDirectory() + "/hide.client.ubtouch.qml");
    m_view->setSource(source);
    m_view->show();

    return true;
}

void HideMe::tryLogin(QString user, QString pass)
{
    if(user.isEmpty() || pass.isEmpty()) {
        return;
    }

    m_settings->setValue("user", user);
    m_settings->setValue("password", pass);
    m_settings->sync();

    m_cliConnector->getToken();
}

void HideMe::logout()
{
    m_settings->setValue("user", "");
    m_settings->setValue("password", "");
    m_settings->sync();

    QFile(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/accessToken.txt").remove();

    m_isLogined = false;
    emit isLoginedChanged();
}

void HideMe::onLoginFailed()
{
    m_settings->setValue("user", "");
    m_settings->setValue("password", "");
    m_settings->sync();
}

void HideMe::onLoginSucces()
{
    m_isLogined = true;
    emit isLoginedChanged();
}
