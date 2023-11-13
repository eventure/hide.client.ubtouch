#include "logging.h"
#include <QStandardPaths>
#include <QDebug>
#include <QFile>

static Logging* loggingInstance = 0;

Logging::Logging(QObject *parent)
    : QObject{parent}
    , m_maxLenght(200)
{
    QMutexLocker locker(&m_lock);
}

Logging *Logging::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (!loggingInstance) {
        loggingInstance = new Logging();
    }
    return loggingInstance;
}

void Logging::add(QString message)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if(m_loggingEntryes.count() >= m_maxLenght) {
        m_loggingEntryes.erase(m_loggingEntryes.begin());
    }
    m_loggingEntryes.insert(currentDateTime, message);

    qDebug() << "LOG:" << message;

    emit Logging::instance()->entryAdded();
}

bool Logging::storeToFile(QString path)
{
    if(path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/hideme_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".log";
    }

    QFile file(path);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << getLogString();
        file.close();
        return true;
    }

    return false;
}

QString Logging::getLogString()
{
    QString out;

    foreach (QDateTime key, m_loggingEntryes.keys())
    {
        out += key.toString("yyyy-MM-dd  HH:mm:ss") + ":" + m_loggingEntryes.value(key) + "\n";
    }

    return out;
}


int Logging::maxLenght() const
{
    return m_maxLenght;
}

void Logging::setMaxLenght(int newMaxLenght)
{
    if (m_maxLenght == newMaxLenght)
        return;
    m_maxLenght = newMaxLenght;
    emit maxLenghtChanged();
}

int Logging::lenght() const
{
    return m_loggingEntryes.count();
}
