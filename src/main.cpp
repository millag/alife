#include "Alife.h"
#include <QtGui/QGuiApplication>

#include <iostream>
#include <Scene.h>
#include <typeinfo>

#include <map>

#include <boost/thread.hpp>

//void test()
//{
//    std::cout << "Hello World!" << std::endl;
//}


int main(int argc, char **argv)
{
//    boost::thread thread1(test);
//    thread1.join();
//    return 0;

    QGuiApplication app(argc, argv);

    Alife alive;
    alive.initialize();
    alive.go();

    return app.exec();
}





