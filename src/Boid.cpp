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

bool Boid::isInNeighbourhood(const RenderObject &_so) const
{
    ngl::Vec4 dir = _so.getPosition() - getPosition();
    ngl::Real distSqr = dir.lengthSquared();
    if (distSqr == 0)
        return true;

    dir.normalize();
    return (distSqr <= getNeighbourhoodDistanceSqr()) && (getHeadingDir().dot(dir) > cos(m_neighbourhoodFOV));
}

RenderObject *Boid::sCreate(const Mesh *_mesh)
{
        ngl::Transformation t;
        t.setPosition(utils::genRandPointInBox(-5.0, 5.0));

        Boid* boid = new Boid(_mesh, -1, t);
        ngl::Vec4 v = utils::genRandPointInSphere(1.0);
//        ngl::Vec4 v(1.0,0,0);
        boid->setMaxSpeed(1.0);
        boid->setMaxTurningAngle(ngl::PI);
        boid->setMass(1.0);
        boid->setVelocity(v);

        boid->setNeighbourhoodDistance(15.0);
        boid->setNeighbourhoodFOV(ngl::PI);
        boid->setPanicDistance(1.0);
        boid->setObstacleLookupDistance(4.0);
        return boid;

//        ngl::Transformation t;
//        t.setPosition(ngl::Vec4());

//        Boid* boid = new Boid(_meshId, t);
//        ngl::Vec4 v(utils::ex);
//        boid->setVelocity( v + v * (0.5 / v.length()));
//        return boid;
}


void Boid::update(ngl::Real _deltaT)
{
    MovingObject::update(_deltaT);
}
