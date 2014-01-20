#include "Scene.h"
#include "Factory.h"
#include "Obstacle.h"
#include "Flock.h"

Scene::Scene() { }

Scene::~Scene()
{
    RenderObjectFactory::sUnregisterObject("boid");
    RenderObjectFactory::sUnregisterObject("obstacle");

//QUESTION: when using vector of pointers do I need to free the memory explicitly
//    delete geometry
    typedef std::vector<Flock*>::const_iterator FIter;
    for (FIter it = m_flocks.begin(); it != m_flocks.end(); ++it) {
        delete (*it);
    }
//    delete scene objects
    typedef std::vector<RenderObject*>::const_iterator ROIter;
    for (ROIter it = m_renderObjects.begin(); it != m_renderObjects.end(); ++it) {
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
    m_boundingVolume.reshape(ngl::Vec4(-10, -10, -10), ngl::Vec4(10, 10, 10));

    RenderObjectFactory::sRegisterObject("boid", Boid::sCreate);
    RenderObjectFactory::sRegisterObject("obstacle", Obstacle::sCreate);

    Flock* flock = new Flock(*this);
    m_flocks.push_back(flock);

    int meshId = m_meshes.size();
    Mesh* mesh = new BoidMesh(meshId);
    m_meshes.push_back(mesh);

    unsigned nBoids = 10;
    m_renderObjects.resize(nBoids, NULL);
    for (unsigned i = 0; i < m_renderObjects.size(); ++i)
    {
        Boid* boid = (Boid*)RenderObjectFactory::sCreateObject("boid", mesh);
        m_renderObjects[i] = boid;
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

const std::vector<RenderObject *> &Scene::getRenderObjects() const
{
    return m_renderObjects;
}

const std::vector<Mesh*>& Scene::getMeshes() const
{
    return m_meshes;
}

const AABB& Scene::getBoundingVolume() const
{
    return m_boundingVolume;
}
