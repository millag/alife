#include "NGLWindow.h"

#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/TransformStack.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

#include <QMouseEvent>
#include <QGuiApplication>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLWindow::NGLWindow(QWindow *_parent) : OpenGLWindow(_parent), m_scene(NULL)
{
    // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
    m_rotate=false;
    // mouse rotation values set to 0
    m_spinXFace=0;
    m_spinYFace=0;
    setTitle("Alife Demo");
}


NGLWindow::~NGLWindow()
{
    ngl::NGLInit *Init = ngl::NGLInit::instance();
    std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
    if (m_scene != NULL)
    {
        delete m_scene;
    }
    Init->NGLQuit();
}

const Scene* NGLWindow::getScene() const
{
    return m_scene;

}

void NGLWindow::setScene(Scene* _scene)
{
    m_scene = _scene;
}

void NGLWindow::resizeEvent(QResizeEvent *_event )
{
    if(isExposed())
    {
        int w=_event->size().width();
        int h=_event->size().height();
        // set the viewport for openGL
        glViewport(0,0,w,h);
        // now set the camera size values as the screen size has changed
        m_scene->getCamera()->setShape(45,(float)w/h,0.05,350);
        renderLater();
    }
}

void NGLWindow::buildVAOs()
{
    assert(m_VAOList.size() == 0);

    std::vector<Mesh*> geomList = m_scene->getMeshList();
    m_VAOList.resize(geomList.size(), NULL);
    unsigned i = 0;
    for (std::vector<Mesh*>::const_iterator it = geomList.begin(); it != geomList.end(); ++it)
    {
        if ((*it)->getNFaces() == 0)
        {
            continue;
        }

        // create a vao as a series of GL_TRIANGLES
        ngl::VertexArrayObject* vao = ngl::VertexArrayObject::createVOA(GL_TRIANGLES);
        m_VAOList[i++] = vao;
        // feed data to GPU
        feedVAO((*it), *vao);
    }
}

void NGLWindow::feedVAO(const Mesh* _geometry, ngl::VertexArrayObject& vao)
{
    vao.bind();
    // in this case we are going to set our data as the vertices above
    // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
    vao.setIndexedData(_geometry->m_vertices.size() * sizeof(ngl::Vec4),
                        _geometry->m_vertices[0].m_x,
                        _geometry->m_vindices.size(),
                        &_geometry->m_vindices[0],
                        GL_UNSIGNED_INT);

    vao.setVertexAttributePointer(0,4,GL_FLOAT,0,0);

    // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
    vao.setIndexedData(_geometry->m_normals.size() * sizeof(ngl::Vec4),
                        _geometry->m_normals[0].m_x,
                        _geometry->m_nindices.size(),
                        &_geometry->m_nindices[0],
                        GL_UNSIGNED_INT);
    vao.setVertexAttributePointer(2,4,GL_FLOAT,0,0);
    vao.setNumIndices(_geometry->m_vindices.size());

    // now unbind
    vao.unbind();
}

void NGLWindow::initialize()
{
    // we must call this first before any other GL commands to load and link the
    // gl commands from the lib, if this is not done program will crash
    ngl::NGLInit::instance();

    glClearColor(0.73f, 0.7f, 0.7f, 1.0f);			   // Grey Background
    // enable depth testing for drawing
    glEnable(GL_DEPTH_TEST);
    // enable multisampling for smoother drawing
    glEnable(GL_MULTISAMPLE);
    // now to load the shader and set the values
    // grab an instance of shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    // we are creating a shader called Phong
    shader->createShaderProgram("Phong");
    // now we are going to create empty shaders for Frag and Vert
    shader->attachShader("PhongVertex",ngl::VERTEX);
    shader->attachShader("PhongFragment",ngl::FRAGMENT);
    // attach the source
    shader->loadShaderSource("PhongVertex","shaders/PhongVertex.glsl");
    shader->loadShaderSource("PhongFragment","shaders/PhongFragment.glsl");
    // compile the shaders
    shader->compileShader("PhongVertex");
    shader->compileShader("PhongFragment");
    // add them to the program
    shader->attachShaderToProgram("Phong","PhongVertex");
    shader->attachShaderToProgram("Phong","PhongFragment");
    // now bind the shader attributes for most NGL primitives we use the following
    // layout attribute 0 is the vertex data (x,y,z)
    shader->bindAttribute("Phong",0,"inVert");
    // attribute 1 is the UV data u,v (if present)
    shader->bindAttribute("Phong",1,"inUV");
    // attribute 2 are the normals x,y,z
    shader->bindAttribute("Phong",2,"inNormal");

    // now we have associated this data we can link the shader
    shader->linkProgramObject("Phong");
    // and make it active ready to load values
    (*shader)["Phong"]->use();
    // the shader will use the currently active material and light0 so set them
    ngl::Material m(ngl::BRONZE);
    // load our material values to the shader into the structure material (see Vertex shader)
    m.loadToShader("material");

    glViewport(0,0,width(),height());

    if ( m_scene == NULL )
        return;


    // now load to our camera
    ngl::Camera* camera = m_scene->getCamera();
    shader->setShaderParam3f("viewerPos",camera->getEye().m_x,camera->getEye().m_y,camera->getEye().m_z);

    // load these values to the shader as well
    m_scene->getLight()->loadToShader("light");
    // as re-size is not explicitly called we need to do this.

    // now build VAO for each geometry in scene
    buildVAOs();
}


void NGLWindow::loadMatricesToShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    ngl::Camera* camera = m_scene->getCamera();
    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 M;
    M=m_transformStack.getCurrAndGlobal().getMatrix();
    MV=  m_transformStack.getCurrAndGlobal().getMatrix()*camera->getViewMatrix();
    MVP= M*camera->getVPMatrix();
    normalMatrix=MV;
    normalMatrix.inverse();
    shader->setShaderParamFromMat4("MV",MV);
    shader->setShaderParamFromMat4("MVP",MVP);
    shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
    shader->setShaderParamFromMat4("M",M);
}

void NGLWindow::render()
{
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // grab an instance of the shader manager
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["Phong"]->use();

    // Rotation based on the mouse position for our global transform
    ngl::Transformation trans;
    ngl::Mat4 rotX;
    ngl::Mat4 rotY;
    // create the rotation matrices
    rotX.rotateX(m_spinXFace);
    rotY.rotateY(m_spinYFace);
    // multiply the rotations
    ngl::Mat4 final=rotY*rotX;
    // add the translations
    final.m_m[3][0] = m_modelPos.m_x;
    final.m_m[3][1] = m_modelPos.m_y;
    final.m_m[3][2] = m_modelPos.m_z;
    // set this in the TX stack
    trans.setMatrix(final);
    m_transformStack.setGlobal(trans);

    // draw
    loadMatricesToShader();

    const std::vector<SceneObject*>& soList = m_scene->getSceneObjectList();
    typedef std::vector<SceneObject*>::const_iterator SOIter;
    for (SOIter it = soList.begin(); it != soList.end(); ++it)
    {
        SceneObject* so = (*it);
        m_transformStack.pushTransform();
        {
//            place object in world
            m_transformStack.setCurrent(so->getTransform());
            loadMatricesToShader();

//            actual draw
            ngl::VertexArrayObject* vao =  m_VAOList.at(so->getMeshId());
            if (vao == NULL)
            {
                continue;
            }
            vao->bind();
            vao->draw();
            vao->unbind();
        } // and before a pop
        m_transformStack.popTransform();
    }

//    typedef std::map<Geometry*, ngl::VertexArrayObject*>::const_iterator Iter;
//    for (Iter it = m_geomToVAOMap.begin(); it != m_geomToVAOMap.end() ; ++it) {
//        it->second->bind();
//        it->second->draw();
//        it->second->unbind();
//    }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLWindow::mouseMoveEvent (QMouseEvent * _event)
{
    // note the method buttons() is the button state when event was called
    // this is different from button() which is used to check which button was
    // pressed when the mousePress/Release event is generated
    if(m_rotate && _event->buttons() == Qt::LeftButton)
    {
        int diffx=_event->x()-m_origX;
        int diffy=_event->y()-m_origY;
        m_spinXFace += (float) 0.5f * diffy;
        m_spinYFace += (float) 0.5f * diffx;
        m_origX = _event->x();
        m_origY = _event->y();
        renderLater();

    }
        // right mouse translate code
    else if(m_translate && _event->buttons() == Qt::RightButton)
    {
        int diffX = (int)(_event->x() - m_origXPos);
        int diffY = (int)(_event->y() - m_origYPos);
        m_origXPos=_event->x();
        m_origYPos=_event->y();
        m_modelPos.m_x += INCREMENT * diffX;
        m_modelPos.m_y -= INCREMENT * diffY;
        renderLater();

    }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLWindow::mousePressEvent ( QMouseEvent * _event)
{
    // this method is called when the mouse button is pressed in this case we
    // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
    if(_event->button() == Qt::LeftButton)
    {
        m_origX = _event->x();
        m_origY = _event->y();
        m_rotate =true;
    }
    // right mouse translate mode
    else if(_event->button() == Qt::RightButton)
    {
        m_origXPos = _event->x();
        m_origYPos = _event->y();
        m_translate=true;
    }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLWindow::mouseReleaseEvent ( QMouseEvent * _event )
{
    // this event is called when the mouse button is released
    // we then set Rotate to false
    if (_event->button() == Qt::LeftButton)
    {
        m_rotate=false;
    }
    // right mouse translate mode
    if (_event->button() == Qt::RightButton)
    {
        m_translate=false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLWindow::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	renderLater();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLWindow::keyPressEvent(QKeyEvent *_event)
{
    // this method is called every time the main window recives a key event.
    // we then switch on the key value and set the camera in the GLWindow
    switch (_event->key())
    {
        // escape key to quite
        case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
        // turn on wirframe rendering
        case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
        // turn off wire frame
        case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
        // show full screen
        case Qt::Key_F : showFullScreen(); break;
        // show windowed
        case Qt::Key_N : showNormal(); break;
        default : break;
    }
    // finally update the GLWindow and re-draw
    //if (isExposed())
    renderLater();
}
