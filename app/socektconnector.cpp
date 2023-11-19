#include "socektconnector.h"
#include "qjsonobject.h"
#include "logging.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>

SocektConnector::SocektConnector(QString url, int port, QObject *parent)
    : QObject{parent}
    , m_url(url)
    , m_port(port)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &SocektConnector::getState);
    m_timer->start(1000);
}

void SocektConnector::getState()
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request = QNetworkRequest(QUrl(QString("http://%1:%2/state").arg(m_url).arg(m_port)));
    m_reply = mgr->get(request);
    connect(m_reply, &QNetworkReply::finished, this, [=]() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        if(!reply) {
            return;
        }

        if(reply->error()) {
            Logging::instance()->add(reply->errorString());
        }

        QJsonDocument answ = QJsonDocument::fromJson(reply->readAll());

        QString code = answ["result"].toObject().value("code").toString();

        if(code != m_code) {
            Logging::instance()->add("Current state is:" + code);
            m_code = code;
            emit codeChanged();
        }
    });
}
