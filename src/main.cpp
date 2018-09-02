#include <QApplication>
#include <QtGui>
#include "Interfaceui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    InterfaceUI face;
    face.show();

    return app.exec();
}
