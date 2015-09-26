// Start-up code for A1 - Sherif Ghali UWO Fall 2015
#include "GameWindow.hpp"
#include <QApplication>

#include <mainwindow.h>

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    MainWindow mw;
    mw.show();

    return app.exec();
}
