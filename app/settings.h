#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QFileSystemWatcher>

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(const QString &fileName, QSettings::Format format = QSettings::Format::NativeFormat, QSettings *parent = nullptr);

signals:
    void settingsUpdated();
};

#endif // SETTINGS_H
