#include "Scene.h"
#include "Stork.h"
#include "Obstacle.h"
#include "flock.h"

Scene::Scene() { }

Scene::~Scene()
{
    Stork::unregisterSpecies("boid");
    Stork::unregisterSpecies("obstacle");

    delete m_camera;
    delete m_light;
//QUESTION: when using vector of pointers do I need to free the memory explicitly
//    delete geometry
    typedef std::vector<Flock*>::const_iterator FIter;
    for (FIter it = m_flocks.begin(); it != m_flocks.end(); ++it) {
        delete (*it);
    }
//    delete scene objects
    typedef std::vector<SceneObject*>::const_iterator SOIter;
    for (SOIter it = m_sceneObjects.begin(); it != m_sceneObjects.end(); ++it) {
        delete (*it);
    }
//    delete geometry
    typedef std::vector<Mesh*>::const_iterator GIter;
    for (GIter it = m_meshes.begin(); it != m_meshes.end(); ++it) {
        delete (*it);
    }
}

void Scene::initialize()
{
    m_camera = new ngl::Camera(ngl::Vec3(0,30,30), ngl::Vec3(0,0,0), ngl::Vec3(0,1,0));
    // set the shape using FOV 45 Aspect Ratio based on Width and Height
    // The final two are near and far clipping planes of 0.5 and 10
    m_camera->setShape(45, (float)720.0 / 576.0, 0.001, 350);

    // now create our light this is done after the camera so we can pass the
    // transpose of the projection matrix to the light to do correct eye space
    // transformations
    ngl::Mat4 iv = m_camera->getViewMatrix();
    iv.transpose();
    iv=iv.inverse();

    m_light = new ngl::Light(ngl::Vec3(0,1,0),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::POINTLIGHT);
    m_light->setTransform(iv);

    Stork::registerSpecies("boid", Boid::create);
    Stork::registerSpecies("obstacle", Obstacle::create);

    Flock* flock = new Flock(*this);
    m_flocks.push_back(flock);

    Mesh* geom = new BoidMesh();
    m_meshes.push_back(geom);
    int meshId = m_meshes.size() - 1;

    unsigned nBoids = 10;
    m_sceneObjects.resize(nBoids, NULL);
    for (unsigned i = 0; i < m_sceneObjects.size(); ++i)
    {
        Boid* boid = (Boid*)Stork::deliverBaby("boid", meshId);
        m_sceneObjects[i] = boid;
        flock->joinBoid(boid);
    }
}


void Scene::update(ngl::Real _deltaT)
{
    typedef std::vector<Flock*>::const_iterator FIter;
    for (FIter it = m_flocks.begin(); it != m_flocks.end(); ++it)
    {
        (*it)->update(_deltaT);
    }
}

const std::vector<SceneObject*>& Scene::getSceneObjectList() const
{
    return m_sceneObjects;
}

const std::vector<Mesh*>& Scene::getMeshList() const
{
    return m_meshes;
}

ngl::Camera *Scene::getCamera()
{
    return m_camera;
}

const ngl::Light *Scene::getLight() const
{
    return m_light;
}
