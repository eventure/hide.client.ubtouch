#ifndef SOCEKTCONNECTOR_H
#define SOCEKTCONNECTOR_H

#include <QNetworkReply>
#include <QObject>
#include <QTimer>

class SocektConnector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString code READ code NOTIFY codeChanged)

public:
    explicit SocektConnector(QString url, int port, QObject *parent = nullptr);
    QString code() const {return m_code;}
    void start();
    void stop();

signals:
    void codeChanged();

private:
    void getState();
    QString m_url;
    int m_port;

    QNetworkReply *m_reply;
    QTimer *m_timer;
    QString m_code;
};

#endif // SOCEKTCONNECTOR_H
