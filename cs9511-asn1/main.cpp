// Start-up code for A1 - Sherif Ghali UWO Fall 2015
#include "GameWindow.hpp"
#include <QApplication>

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    GameWindow gw;
    gw.show();
    return app.exec();
}
