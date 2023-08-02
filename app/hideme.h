#ifndef HIDEME_H
#define HIDEME_H

#include <QApplication>
#include <QQuickView>
#include <QSettings>

#include "clitoolconnector.h"

class HideMe : public QApplication {
    Q_OBJECT

public:
    HideMe(int& argc, char** argv);
    virtual ~HideMe();
    bool setup();

private slots:
    void serviceHandler();

private:
    void startService();
    QQuickView* m_view;

    CliToolConnector* m_cliConnector;
    QProcess* m_serviceProcess;
};

#endif // HIDEME_H
