#include "CGame.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CGame window;
    window.show();
    return app.exec();
}
