#ifndef HIDEME_H
#define HIDEME_H

#include <QApplication>
#include <QQuickView>

class HideMe : public QApplication {
    Q_OBJECT
    Q_PROPERTY(bool isLogined READ isLogined NOTIFY isLoginedChanged)
public:
    HideMe(int& argc, char** argv);
    virtual ~HideMe();
    bool setup();

    bool isLogined() { return m_isLogined; }

signals:
    void isLoginedChanged();

private:
    QQuickView* m_view;
    bool m_isLogined;
};

#endif // HIDEME_H
