#ifndef HIDEME_H
#define HIDEME_H

#include <QApplication>
#include <QQuickView>
#include <QSettings>

#include "clitoolconnector.h"

class HideMe : public QApplication {
    Q_OBJECT
    Q_PROPERTY(bool isLogined READ isLogined NOTIFY isLoginedChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    HideMe(int& argc, char** argv);
    virtual ~HideMe();
    bool setup();

    bool isLogined() { return m_isLogined; }
    bool connected() { return m_connected; }

    Q_INVOKABLE void tryLogin(QString user, QString pass);
    Q_INVOKABLE void logout();

signals:
    void isLoginedChanged();
    void connectedChanged();

private slots:
    void onLoginFailed();
    void onLoginSucces();

private:
    QQuickView* m_view;
    bool m_isLogined;
    bool m_connected;

    CliToolConnector* m_cliConnector;
    QSettings* m_settings;
};

#endif // HIDEME_H
