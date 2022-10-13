#include "AppQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppQt w;
    w.setFixedSize(1300,1000);
    w.show();
    return a.exec();
}
