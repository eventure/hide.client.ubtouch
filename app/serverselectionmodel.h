#ifndef SERVERSELECTIONMODEL_H
#define SERVERSELECTIONMODEL_H

#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include "settings.h"

static const QString API_URL = "https://api.hide.me/v1/external/passepartout";

class ServerSelectionModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

public:
    struct Server
    {
        int serverId;
        QString hostname;
        QString flag;
        QString displayName;
        qreal lat;
        qreal lon;
        QString cityName;
        QString countryCode;
        QString continent;
        QList<Server*> children;
        bool stared;
        bool is10g;
        bool isfree;
        bool isupgrade;
    };

    ServerSelectionModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const {return m_hash;}
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Q_INVOKABLE void activate(int serverId);
    Q_INVOKABLE QVariantMap get(int serverId);
    Q_INVOKABLE QVariantMap get(QString hostname);

    Q_INVOKABLE void changeFavorite(int serverId);
    Q_INVOKABLE bool isFavoriteServer(const int serverId) const;

signals:
    void rowCountChanged();
    void dataChanged();
    void serverActivated(QVariantMap server);

private slots:
    void parsePassepartoutHandler(QNetworkReply *reply);
    void resetInternalData();

private:
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QHash<int,QByteArray> m_hash;
    QList<Server*> m_serverList;

    QNetworkAccessManager* m_nam;
    Settings* m_settings;
    QVariantMap serverToVariantMap(Server *s) const;
    Server* jsonObjectToServer(QJsonObject o);
};

#endif // SERVERSELECTIONMODEL_H
