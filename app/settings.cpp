#include "settings.h"
#include <QStandardPaths>
#include <QDebug>
#include <QFile>

Settings::Settings(const QString &filePath, Format format, QSettings *parent)
    : QSettings{ QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/" + filePath, format, parent}
{
    QFile settingsFile(fileName());
    if(!settingsFile.exists()) {
        settingsFile.open( QIODevice::WriteOnly|QIODevice::Append );
        settingsFile.close();
    }

    QFileSystemWatcher* settingsFileWatcher = new QFileSystemWatcher(this);
    settingsFileWatcher->addPath(this->fileName());
    connect(settingsFileWatcher, &QFileSystemWatcher::fileChanged, this, &Settings::onSettingsUpdated);
}

void Settings::onSettingsUpdated()
{
    emit settingsUpdated();
}
