#include "Scene.h"
#include "Factory.h"
#include "Obstacle.h"
#include "Flock.h"

RenderObject* createBoid(const Mesh *_mesh);
RenderObject* createObstacle(const Mesh* _mesh);

//===================================== Scene Class Def ===========================================

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
    m_boundingVolume.reshape(ngl::Vec4(-30, -30, -30), ngl::Vec4(30, 30, 30));

    RenderObjectFactory::sRegisterObject("boid", createBoid);
    RenderObjectFactory::sRegisterObject("obstacle", createObstacle);

    Flock* flock = new Flock(*this);
    m_flocks.push_back(flock);

    unsigned meshId = m_meshes.size();
    m_meshMap["boid"] = meshId;
    Mesh* mesh = new BoidMesh(meshId);
    m_meshes.push_back(mesh);

    unsigned nBoids = 100;
    m_renderObjects.reserve(nBoids);
    addBoids(nBoids, 0);

    meshId = m_meshes.size();
    m_meshMap["obstacle"] = meshId;
    mesh = new ObstacleMesh(meshId);
    m_meshes.push_back(mesh);

    unsigned nObstacles = 5;
    for (unsigned i = 0; i < nObstacles; ++i)
    {
        Obstacle* obstacle = (Obstacle*)RenderObjectFactory::sCreateObject("obstacle", mesh);
        m_renderObjects.push_back(obstacle);
        m_obstacles.push_back(obstacle);
    }
}


void Scene::addBoids(unsigned _nCnt, unsigned _flockId)
{
    assert(m_flocks.size() > _flockId);

    Flock* flock = m_flocks[_flockId];
    Mesh* mesh = m_meshes[ m_meshMap["boid"] ];
    for (unsigned i = 0; i < _nCnt; ++i)
    {
        Boid* boid = (Boid*)RenderObjectFactory::sCreateObject("boid", mesh);
        m_renderObjects.push_back(boid);
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


//====================================== utility functions =============================================


RenderObject *createBoid(const Mesh *_mesh)
{
    static unsigned boidCnt = 0;

//    v *= utils::randf(5, 8);
//    ngl::Vec4 p = utils::genRandPointInBox(-10.0, 10.0);

    Boid* boid = new Boid(_mesh, -1);

    ngl::Vec4 v = utils::genRandPointOnSphere(1.0) * 1;
    v.m_w = 0;
    ngl::Vec4 p = ngl::Vec4(0, 0, 0);

    boid->setPosition(p);
    boid->setMass(1.0);
    boid->setMaxSpeed(2.0);
    boid->setMaxTurningAngle(ngl::PI / 4);
    boid->setVelocity(v);

    boid->setPanicDistance(2.0);
    boid->setNeighbourhoodDistance(3.0);
    boid->setNeighbourhoodFOV(ngl::PI);
    boid->setObstacleLookupDistance(10.0);

    boidCnt++;

    return boid;
}

RenderObject* createObstacle(const Mesh* _mesh)
{
    ngl::Real scale = utils::randf(2.0, 10.0);
    ngl::Transformation t;
    t.setPosition(utils::genRandPointInBox(-30.0, 30.0));
    t.setScale(scale, scale, scale);

    return new SphericalObstacle(_mesh, -1, t);
}
