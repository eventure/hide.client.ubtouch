#include "hideme.h"
#include "config.h"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>
#include <QQmlEngine>

HideMe::HideMe(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_view(nullptr)
    , m_cliConnector(new CliToolConnector(this))
    , m_serviceManager(new ServiceManager(this))
{
    setOrganizationName("HideMe");
    setOrganizationDomain("hideme.com");
    setApplicationName("HideMe VPN");
}

HideMe::~HideMe()
{
    if (m_view) {
        delete m_view;
    }
}

bool HideMe::setup()
{
    qmlRegisterType<ServiceManager>("hide.me", 1, 0, "ServiceManager");
    qmlRegisterType<CliToolConnector>("hide.me", 1, 0, "CliToolConnector");
    m_view = new QQuickView();
    m_view->setColor(Qt::white);
    m_view->setResizeMode(QQuickView::SizeViewToRootObject);
    m_view->setFlags(static_cast<Qt::WindowFlags>(0x00800000));
    m_view->setTitle(tr("Hide me VPN"));

    m_view->rootContext()->setContextProperty("mApplication", this);
    QUrl source(appDirectory() + "/hide.client.ubtouch.qml");
    m_view->setSource(source);
    m_view->show();

    return true;
}
