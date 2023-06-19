#ifndef HIDEME_H
#define HIDEME_H

#include <QApplication>
#include <QQuickView>

class HideMe : public QApplication
{
    Q_OBJECT
public:
    HideMe(int& argc, char** argv);
    virtual ~HideMe();
    bool setup();

private:
    QQuickView* m_view;
};

#endif // HIDEME_H
