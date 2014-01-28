#include "Alife.h"
#include <QtGui/QGuiApplication>

#include <iostream>
#include <Scene.h>
#include <typeinfo>

#include <map>

//void test()
//{
//    std::cout << 122 % 1 << std::endl;
//}


int main(int argc, char **argv)
{
//    test();
//    return 0;

    QGuiApplication app(argc, argv);

    Alife alive;
    alive.initialize();
    alive.go();

    return app.exec();
}





