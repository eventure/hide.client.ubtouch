#include "hideme.h"
#include "config.h"

#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>

HideMe::HideMe(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_view(nullptr)
    , m_cliConnector(new CliToolConnector(this))
{
    setOrganizationName("HideMe");
    setOrganizationDomain("hideme.com");
    setApplicationName("HideMe VPN");

    m_serviceProcess = new QProcess(this);
    connect(m_serviceProcess, &QProcess::readyReadStandardOutput, this, &HideMe::serviceHandler);

    startService();
}

HideMe::~HideMe()
{
    if (m_view) {
        delete m_view;
    }

    m_serviceProcess->close();
}

bool HideMe::setup()
{
    m_view = new QQuickView();
    m_view->setColor(Qt::white);
    m_view->setResizeMode(QQuickView::SizeViewToRootObject);
    m_view->setFlags(static_cast<Qt::WindowFlags>(0x00800000));
    m_view->setTitle(tr("Hide me VPN"));

    m_view->rootContext()->setContextProperty("mApplication", this);
    m_view->rootContext()->setContextProperty("cli", m_cliConnector);
    m_view->rootContext()->setContextProperty("haveTools", m_cliConnector->cliAvailable());
    QUrl source(appDirectory() + "/hide.client.ubtouch.qml");
    m_view->setSource(source);
    m_view->show();

    return true;
}

void HideMe::startService()
{
    QString cliPath = m_cliConnector->programString();
    m_serviceProcess->start(cliPath, QStringList() << "--caddr" << "127.0.0.1:5050" << "service");
}


void HideMe::serviceHandler()
{
    qDebug() << m_serviceProcess->readAllStandardOutput();
}
