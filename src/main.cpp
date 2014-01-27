#include "Alife.h"
#include <QtGui/QGuiApplication>

#include <iostream>
#include <Scene.h>
#include <typeinfo>

#include <map>

void test();

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    Alife alive;
    alive.initialize();
    alive.go();

    return app.exec();
}

//void test()
//{
//    std::cout << 122 % 1 << std::endl;
//}



