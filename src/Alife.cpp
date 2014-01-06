#include "Alife.h"

#include <iostream>
#include <QSurfaceFormat>
#include "utils.h"
void initSurfaceContextFormat(QSurfaceFormat& _format);

Alife::Alife(QObject *parent) : QObject(parent)
{
    m_scene = new Scene();
    m_window = new NGLWindow();
    m_timer = new QTimer(this);
}

Alife::~Alife()
{
    delete m_scene;
    delete m_timer;
}

void Alife::initialize()
{
    m_scene->initialize();

    // create an OpenGL format specifier and intialize
    QSurfaceFormat format;
    initSurfaceContextFormat(format);
    // now we are going to create our openGL window/renderer

    // and set the OpenGL format
    m_window->setFormat(format);
    //give our renderer the scene we want to render
    m_window->setScene(m_scene);
    // we can now query the version to see if it worked
    std::cout<<"Profile is "<<format.majorVersion()<<" "<<format.minorVersion()<<"\n";

    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

void Alife::go()
{
    // set the window size
    m_window->resize(1024, 720);
    // and finally show
    m_window->show();
    m_timer->start(utils::UPDATE_RATE);
}

void Alife::update()
{
    std::cout << "UPDATE" << std::endl;
//    TODO implement update
    ngl::Real deltaT = 1.0 / utils::FPS;
    m_scene->update(deltaT);
    // re-draw GL
    m_window->renderLater();
}

void initSurfaceContextFormat(QSurfaceFormat& _format)
{
    // set the number of samples for multisampling
    // will need to enable glEnable(GL_MULTISAMPLE); once we have a context
    _format.setSamples(4);
    #if defined( DARWIN)
      // at present mac osx Mountain Lion only supports GL3.2
      // the new mavericks will have GL 4.x so can change
      _format.setMajorVersion(3);
      _format.setMinorVersion(2);
    #else
      // with luck we have the latest GL version so set to this
      _format.setMajorVersion(4);
      _format.setMinorVersion(3);
    #endif
    // now we are going to set to CoreProfile OpenGL so we can't use and old Immediate mode GL
    _format.setProfile(QSurfaceFormat::CoreProfile);
    // now set the depth buffer to 24 bits
    _format.setDepthBufferSize(24);
}
