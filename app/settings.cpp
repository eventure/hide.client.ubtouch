#include "settings.h"
#include <QStandardPaths>

Settings::Settings(const QString &fileName, Format format, QSettings *parent)
    : QSettings{ QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + fileName, format, parent}
{
    QFileSystemWatcher* settingsFileWatcher = new QFileSystemWatcher(this);
    settingsFileWatcher->addPath(this->fileName());
    connect(settingsFileWatcher, &QFileSystemWatcher::fileChanged, this, &Settings::settingsUpdated);
}
