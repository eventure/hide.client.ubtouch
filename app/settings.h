#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QFileSystemWatcher>

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(const QString &filePath, QSettings::Format format = QSettings::Format::NativeFormat, QSettings *parent = nullptr);

private slots:
    void onSettingsUpdated();

signals:
    void settingsUpdated();
};

#endif // SETTINGS_H
