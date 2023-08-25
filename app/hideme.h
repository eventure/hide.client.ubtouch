#ifndef HIDEME_H
#define HIDEME_H

#include <QApplication>
#include <QQuickView>
#include <QSettings>

#include "clitoolconnector.h"
#include "servicemanager.h"

class HideMe : public QApplication {
    Q_OBJECT

public:
    HideMe(int& argc, char** argv);
    virtual ~HideMe();
    bool setup();

private:
    QQuickView* m_view;
    CliToolConnector* m_cliConnector;
    ServiceManager* m_serviceManager;
};

#endif // HIDEME_H
