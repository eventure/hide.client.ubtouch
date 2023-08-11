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

private:
    QQuickView* m_view;
    CliToolConnector* m_cliConnector;
};

#endif // HIDEME_H
