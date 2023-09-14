#include "serverselectionmodel.h"
#include "qjsonobject.h"

#include <QJsonArray>
#include <QJsonDocument>

ServerSelectionModel::ServerSelectionModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_nam(new QNetworkAccessManager(this))
{
    m_hash.insert(Qt::UserRole ,QByteArray("serverIdenty"));
    m_hash.insert(Qt::UserRole+1 ,QByteArray("hostname"));
    m_hash.insert(Qt::UserRole+2 ,QByteArray("countryFlag"));
    m_hash.insert(Qt::UserRole+3 ,QByteArray("displayName"));
    m_hash.insert(Qt::UserRole+4 ,QByteArray("lat"));
    m_hash.insert(Qt::UserRole+5 ,QByteArray("lon"));
    m_hash.insert(Qt::UserRole+6 ,QByteArray("cityName"));
    m_hash.insert(Qt::UserRole+7 ,QByteArray("countryCode"));
    m_hash.insert(Qt::UserRole+8 ,QByteArray("continent"));
    m_hash.insert(Qt::UserRole+9 ,QByteArray("childrenServers"));
    m_hash.insert(Qt::UserRole+10 ,QByteArray("stared"));
    m_hash.insert(Qt::UserRole+11 ,QByteArray("is10g"));
    m_hash.insert(Qt::UserRole+12 ,QByteArray("isfree"));
    m_hash.insert(Qt::UserRole+13 ,QByteArray("upgrade"));

    connect(m_nam, &QNetworkAccessManager::finished, this, &ServerSelectionModel::parsePassepartoutHandler);
    m_nam->get(QNetworkRequest(QUrl(API_URL)));

    m_settings = new QSettings("hideconfig.ini");
}

QVariant ServerSelectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() >= m_serverList.size()) {
        return QVariant();
    }

    Server s = m_serverList.at(index.row());
    QVariantList childrenList;
    if(role == Qt::UserRole+9 && s.children.count() > 0) {
        for(Server childServer: s.children) {
            QVariantMap sMap = serverToVariantMap(childServer);
            childrenList.push_back(sMap);
        }
    }

    switch (role) {
    case Qt::UserRole:
        return s.id;
    case Qt::UserRole+1:
        return s.hostname;
    case Qt::UserRole+2:
        return s.flag;
    case Qt::UserRole+3:
        return s.displayName;
    case Qt::UserRole+4:
        return s.lat;
    case Qt::UserRole+5:
        return s.lon;
    case Qt::UserRole+6:
        return s.cityName;
    case Qt::UserRole+7:
        return s.countryCode;
    case Qt::UserRole+8:
        return s.continent;
    case Qt::UserRole+9:
        return childrenList;
    case Qt::UserRole+10:
        return s.stared;
    case Qt::UserRole+11:
        return s.is10g;
    case Qt::UserRole+12:
        return s.isfree;
    case Qt::UserRole+13:
        return s.isupgrade;
    default:
        return QVariant();
    }
}

int ServerSelectionModel::rowCount(const QModelIndex &parent) const
{
    return m_serverList.count();
}

void ServerSelectionModel::activate(int serverId)
{
    QVariantMap selectedServer = get(serverId);
    if(!selectedServer.empty()) {
        emit serverActivated(selectedServer);
    }
}

QVariantMap ServerSelectionModel::get(int serverId)
{
    for(Server s: m_serverList) {
        if(s.id == serverId) {
            return serverToVariantMap(s);
        }
        for(Server sc: s.children) {
            if(sc.id == serverId) {
                return serverToVariantMap(sc);
            }
        }
    }
    qWarning() << "Cant find server with id " << serverId;
    return QVariantMap();
}

QVariantMap ServerSelectionModel::get(QString hostname)
{
    for(Server s: m_serverList) {
        if(s.hostname == hostname) {
            return serverToVariantMap(s);
        }
        for(Server sc: s.children) {
            if(sc.hostname == hostname) {
                return serverToVariantMap(sc);
            }
        }
    }
    qWarning() << "Cant find server with nostname " << hostname;
    return QVariantMap();
}

void ServerSelectionModel::parsePassepartoutHandler(QNetworkReply *reply)
{
    beginResetModel();
    m_serverList.clear();
    if(reply->error() == QNetworkReply::NoError){
        QJsonArray jsonResponse = QJsonDocument::fromJson(reply->readAll()).array();
        for(const QJsonValue &server: jsonResponse) {
            Server s = jsonObjectToServer(server.toObject());
            QList<Server> childrenList;
            for(QJsonValue value : server.toObject().value("children").toArray()) {
                Server cs = jsonObjectToServer(value.toObject());
                childrenList.push_back(cs);
            }
            s.children      = childrenList;
            m_serverList.push_back(s);
        }
    } else {
        qWarning() << reply->errorString();
    }
    emit rowCountChanged();
    endResetModel();
}

void ServerSelectionModel::resetInternalData()
{
    //m_serverList.clear();
}

QVariantMap ServerSelectionModel::serverToVariantMap(Server s) const
{
    QVariantMap varmapServer;
    varmapServer["serverId"] = s.id;
    varmapServer["hostname"] = s.hostname;
    varmapServer["flag"] = s.flag;
    varmapServer["displayName"] = s.displayName;
    varmapServer["lat"] = s.lat;
    varmapServer["lon"] = s.lon;
    varmapServer["cityName"] = s.cityName;
    varmapServer["countryCode"] = s.countryCode;
    varmapServer["continent"] = s.continent;
    varmapServer["is10g"] = s.is10g;
    varmapServer["isfree"] = s.isfree;
    varmapServer["isupgrade"] = s.isupgrade;
    varmapServer["stared"] = s.stared;

    return varmapServer;
}

ServerSelectionModel::Server ServerSelectionModel::jsonObjectToServer(QJsonObject o)
{
    Server s;
    s.id            = o.value("id").toInt();
    s.hostname      = o.value("hostname").toString();
    s.flag          = o.value("flag").toString();
    s.displayName   = o.value("displayName").toString();
    s.lat           = o.value("lat").toDouble();
    s.lon           = o.value("lon").toDouble();
    s.cityName      = o.value("cityName").toString();
    s.countryCode   = o.value("countryCode").toString();
    s.continent     = o.value("continent").toString();
    s.is10g         = o.value("tags").toArray().contains("10g");
    s.isfree        = o.value("tags").toArray().contains("free");
    s.isupgrade     = o.value("tags").toArray().contains("upgrade");
    s.stared        = m_settings->value("favoriteServers").toString().split(";").contains(QString::number(s.id));

    return s;
}
