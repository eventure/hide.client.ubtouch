#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QObject>
static const QString s_dbusPath = QStringLiteral("/org/freedesktop/systemd1");
static const QString s_serviceName = QStringLiteral("org.freedesktop.systemd1");
static const QString s_propertiesIface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString s_managerIface = QStringLiteral("org.freedesktop.systemd1.Manager");
static const QString s_unitIface = QStringLiteral("org.freedesktop.systemd1.Unit");
static const QString s_propertyActiveState = QStringLiteral("ActiveState");

#ifdef WITH_CLICK
    static const QString CA_PEM_PATH = "/opt/click.ubuntu.com/hideme.ubports/current/CA.pem";
#else
    static const QString CA_PEM_PATH = "/usr/share/hideme/CA.pem";
#endif

#endif // CONSTANTS_H
