#ifndef LOGGING_H
#define LOGGING_H

#include <QDateTime>
#include <QMap>
#include <QMutex>
#include <QObject>

class Logging : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maxLenght READ maxLenght WRITE setMaxLenght NOTIFY maxLenghtChanged)
    Q_PROPERTY(int lenght READ lenght NOTIFY lenghtChanged)

public:
    explicit Logging(QObject *parent = nullptr);
    static Logging* instance();

    Q_INVOKABLE void add(QString message);
    Q_INVOKABLE bool storeToFile(QString path = "");
    Q_INVOKABLE QString getLogString();

    int maxLenght() const;
    void setMaxLenght(int newMaxLenght);

    int lenght() const;

signals:
    void maxLenghtChanged();
    void lenghtChanged();
    void entryAdded();

private:
    QMutex m_lock;
    QMap<QDateTime, QString> m_loggingEntryes;
    int m_maxLenght;
};

#endif // LOGGING_H