#include <QGuiApplication>
#include "hideme.h"

int main(int argc, char** argv)
{
    unsetenv("QML_FIXED_ANIMATION_STEP");
    HideMe application(argc, argv);
    if (!application.setup()) {
        return 1;
    }

    return application.exec();
}
