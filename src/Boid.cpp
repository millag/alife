#include "Boid.h"
#include "ngl/Util.h"

Boid::Boid(const Mesh *_mesh, int _shaderId, const ngl::Transformation &_transform):
    MovingObject(_mesh, _shaderId, _transform)
{
    m_neighbourhoodDist = (m_mesh == NULL)? 0 : m_mesh->getBoundingRadius();
    m_neighbourhoodFOV = 0;
    m_panicDist = (m_mesh == NULL)? 0 : m_mesh->getBoundingRadius();
    m_obstacleLookupDist = (m_mesh == NULL)? 0 : m_mesh->getBoundingRadius();
}

Boid::~Boid() { }

void Boid::setNeighbourhoodDistance(ngl::Real _d)
{
    assert(_d >= 0);
    m_neighbourhoodDist = _d + ((m_mesh == NULL)? 0 : m_mesh->getBoundingRadius());
}

void Boid::setPanicDistance(ngl::Real _d)
{
    assert(_d >= 0);
    m_panicDist = _d + ((m_mesh == NULL)? 0 : m_mesh->getBoundingRadius());
}

void Boid::setObstacleLookupDistance(ngl::Real _d)
{
    assert(_d >= 0);
    m_obstacleLookupDist = _d + ((m_mesh == NULL)? 0 : m_mesh->getBoundingRadius());
}

bool Boid::isInNeighbourhood(const MovingObject &_so) const
{
    ngl::Vec4 dir = _so.getPosition() - getPosition();
    ngl::Real distSqr = dir.lengthSquared();
    if (distSqr < utils::C_ERR)
        return true;

    dir.normalize();
    return (distSqr <= getNeighbourhoodDistanceSqr()) && (getHeadingDir().dot(dir) > cos(m_neighbourhoodFOV));
}

RenderObject *Boid::sCreate(const Mesh *_mesh)
{
    static unsigned boidCnt = 0;

    ngl::Vec4 v = utils::genRandPointOnSphere(1.0);
    v.m_w = 0;
//    v *= utils::randf(5, 8);
//    ngl::Vec4 p = utils::genRandPointInBox(-10.0, 10.0);
    ngl::Vec4 p = ngl::Vec4(0, 0, 0);
    Boid* boid = new Boid(_mesh, -1);
    boid->m_position = p;
    boid->setMass(1.0);
    boid->setMaxSpeed(2.0);
    boid->setMaxTurningAngle(ngl::PI / 4);
    boid->setVelocity(v);

    boid->setPanicDistance(1.0);
    boid->setNeighbourhoodDistance(1.0);
    boid->setNeighbourhoodFOV(ngl::PI);
    boid->setObstacleLookupDistance(20.0);

    boidCnt++;

    return boid;
}


void Boid::update(ngl::Real _deltaT)
{
    MovingObject::update(_deltaT);
}
