#include "NGLWindow.h"

#include "ngl/Camera.h"
#include "ngl/Light.h"
#include "ngl/TransformStack.h"
#include "ngl/Material.h"
#include "ngl/NGLInit.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/ShaderLib.h"

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
        delete m_camera;
        delete m_light;
        delete m_boundingVolume;
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
        if (m_scene)
        {
            m_camera->setShape(45,(float)w/h,0.05,350);
            renderLater();
        }
    }
}

void NGLWindow::buildVAOs()
{
    assert(m_VAOList.size() == 0);

    std::vector<Mesh*> meshList = m_scene->getMeshes();
    m_VAOList.resize(meshList.size(), NULL);
    unsigned i = 0;
    for (std::vector<Mesh*>::const_iterator it = meshList.begin(); it != meshList.end(); ++it)
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

void NGLWindow::feedVAO(const Mesh* _mesh, ngl::VertexArrayObject& o_vao)
{
    o_vao.bind();
    // in this case we are going to set our data as the vertices above
    // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
    const std::vector<ngl::Vec4>& v = _mesh->getVertices();
    const std::vector<unsigned>& vi = _mesh->getVIndices();
    o_vao.setIndexedData(v.size() * sizeof(ngl::Vec4),
                        v[0].m_x,
                        vi.size(),
                        &vi[0],
                        GL_UNSIGNED_INT);

    o_vao.setVertexAttributePointer(0,4,GL_FLOAT,0,0);

    // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
    const std::vector<ngl::Vec4>& vn = _mesh->getNormals();
    const std::vector<unsigned>& vni = _mesh->getNIndices();
    o_vao.setIndexedData(vn.size() * sizeof(ngl::Vec4),
                        vn[0].m_x,
                        vni.size(),
                        &vni[0],
                        GL_UNSIGNED_INT);
    o_vao.setVertexAttributePointer(2,4,GL_FLOAT,0,0);
    o_vao.setNumIndices(vi.size());

    // now unbind
    o_vao.unbind();
}

void NGLWindow::createBoundingVolume()
{
//    create scene bounding volume
    m_boundingVolume = new ngl::BBox(m_scene->getBoundingVolume().getCenter().toVec3(),
                                     m_scene->getBoundingVolume().getWidth(),
                                     m_scene->getBoundingVolume().getHeight(),
                                     m_scene->getBoundingVolume().getDepth());
    m_boundingVolume->setDrawMode(GL_LINE);

//    create reference grid
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
    prim->createLineGrid("grid", 10, 10, 10);

//    ngl::Vec4 vmin = m_scene->getBoundingVolume().getBottomLeft();
//    ngl::Vec4 vmax = m_scene->getBoundingVolume().getTopRight();
//    std::vector<ngl::Vec4> v(8);
//    v[0] = ngl::Vec4(vmin.m_x, vmin.m_y, vmin.m_z);
//    v[1] = ngl::Vec4(vmax.m_x, vmin.m_y, vmin.m_z);
//    v[2] = ngl::Vec4(vmax.m_x, vmax.m_y, vmin.m_z);
//    v[3] = ngl::Vec4(vmin.m_x, vmax.m_y, vmin.m_z);

//    v[4] = ngl::Vec4(vmin.m_x, vmin.m_y, vmax.m_z);
//    v[5] = ngl::Vec4(vmax.m_x, vmin.m_y, vmax.m_z);
//    v[6] = ngl::Vec4(vmax.m_x, vmax.m_y, vmax.m_z);
//    v[7] = ngl::Vec4(vmin.m_x, vmax.m_y, vmax.m_z);

//    unsigned vidx[] = { 0, 2, 3, 2, 0, 1,
//                        0, 3, 4, 4, 3, 7,
//                        4, 7, 6, 4, 6, 5,
//                        3, 2, 7, 7, 2, 6,
//                        2, 1, 5, 2, 5, 6,
//                        1, 0, 4, 1, 4, 5 };
//    std::vector<unsigned> vi(vidx, vidx + 24);



//    ngl::VertexArrayObject* vao = ngl::VertexArrayObject::createVOA(GL_LINE);
//    m_VAOList.push_back(vao);
//    vao->bind();

//    // in this case we are going to set our data as the vertices above
//    // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
//    vao->setIndexedData(v.size() * sizeof(ngl::Vec4),
//                        v[0].m_x,
//                        vi.size(),
//                        &vi[0],
//                        GL_UNSIGNED_INT);

//    vao->setVertexAttributePointer(0,4,GL_FLOAT,0,0);
//    vao->setNumIndices(vi.size());

//    // now unbind
//    vao->unbind();
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
    // as re-size is not explicitly called we need to do this.
    glViewport(0,0,width(),height());

    if ( m_scene == NULL )
        return;

//    ngl::Vec3 eye(m_scene->getBoundingVolume().getBottomLeft().m_x / 2, m_scene->getBoundingVolume().getTopRight().m_y / 2, -m_scene->getBoundingVolume().getBottomLeft().m_z);
//    eye += eye * 2;
    ngl::Vec3 eye(0, 20, -20);
    ngl::Vec3 at(0,0,0);
    ngl::Vec3 up(0,1,0);
    m_camera = new ngl::Camera(eye, at, up);
    // set the shape using FOV 45 Aspect Ratio based on Width and Height
    // The final two are near and far clipping planes of 0.5 and 10
    m_camera->setShape(45, (float)720.0 / 576.0, 0.001, 350);
    shader->setShaderParam3f("viewerPos",m_camera->getEye().m_x,m_camera->getEye().m_y,m_camera->getEye().m_z);

    // now create our light this is done after the camera so we can pass the
    // transpose of the projection matrix to the light to do correct eye space
    // transformations
    ngl::Mat4 iv = m_camera->getViewMatrix();
    iv.transpose();
    iv=iv.inverse();

    m_light = new ngl::Light(ngl::Vec3(0,1,0),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT);
    m_light->setTransform(iv);
    // load these values to the shader as well
    m_light->loadToShader("light");

    // build VAO for each geometry in scene
    buildVAOs();
    // build VAO to render scene bounding volume
    createBoundingVolume();
}


void NGLWindow::loadMatricesToShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 M;
    M=m_transformStack.getCurrAndGlobal().getMatrix();
    MV=  m_transformStack.getCurrAndGlobal().getMatrix()*m_camera->getViewMatrix();
    MVP= M*m_camera->getVPMatrix();
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

    if (m_scene == NULL)
    {
        return;
    }
    ngl::VertexArrayObject* vao;

    // draw
    loadMatricesToShader();
    const std::vector<RenderObject*>& roList = m_scene->getRenderObjects();
    typedef std::vector<RenderObject*>::const_iterator ROIter;
    for (ROIter it = roList.begin(); it != roList.end(); ++it)
    {
        RenderObject* ro = (*it);
        m_transformStack.pushTransform();
        {
//            place object in world
            m_transformStack.setCurrent(ro->getTransform());
            loadMatricesToShader();

//            actual draw
            vao =  m_VAOList.at(ro->getMeshId());
            if (vao == NULL)
            {
                continue;
            }
            vao->bind();
            vao->draw();
            vao->unbind();
        }
        m_transformStack.popTransform();
    }

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    loadMatricesToShader();
    prim->draw("grid");
    m_boundingVolume->draw();
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
