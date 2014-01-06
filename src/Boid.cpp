#include "Boid.h"
#include <ngl/Util.h>

Boid::Boid(int _meshId, const ngl::Transformation &_transform):
    SceneObject(_meshId, _transform),
    m_maxSpeedSqr(1.0),m_mass(1.0),
    m_neighbourhoodSqrDistance(4.0),m_neighbourhoodAngle(ngl::PI)
{ }

Boid::Boid(const Boid &_boid):SceneObject(_boid)
{
    m_maxSpeedSqr = _boid.m_maxSpeedSqr;
    m_velocity = _boid.m_velocity;
    m_neighbourhoodSqrDistance = _boid.m_neighbourhoodSqrDistance;
    m_neighbourhoodAngle = _boid.m_neighbourhoodAngle;
}

Boid::~Boid() { }


ngl::Real Boid::getMaxSpeed() const
{
    return sqrt(m_maxSpeedSqr);
}

void Boid::setMaxSpeed(const ngl::Real& _maxSpeed)
{
    m_maxSpeedSqr = _maxSpeed * _maxSpeed;
}

ngl::Vec4 Boid::getDirection() const
{
    return (m_velocity / m_velocity.length());
}

const ngl::Vec4 &Boid::getVelocity() const
{
    return m_velocity;
}

void Boid::setVelocity(const ngl::Vec4 &v)
{
    m_velocity = v;
    if (m_velocity.lengthSquared() == 0)
        return;

    if (m_velocity.lengthSquared() > m_maxSpeedSqr)
    {
        m_velocity.normalize();
        m_velocity *= sqrt(m_maxSpeedSqr);
    }

    lookAt(m_velocity, utils::ey);
}

ngl::Real Boid::getMass() const
{
    return m_mass;
}

void  Boid::setMass(const ngl::Real& _mass)
{
    m_mass = _mass;
}

ngl::Real Boid::getNeighbourhoodDistance() const
{
    return sqrt(m_neighbourhoodSqrDistance);
}

ngl::Real Boid::getNeighbourhoodAngle() const
{
    return m_neighbourhoodAngle;
}

bool Boid::isInNeighbourhood(const SceneObject& _object) const
{
    ngl::Real dist = getSqrDistanceToObject(_object);

    return (dist < m_neighbourhoodSqrDistance);
}

ngl::Real Boid::getSqrDistanceToObject(const SceneObject& _object) const
{
    ngl::Vec4 dir = _object.getPosition() - getPosition();
    ngl::Real sqrDist = dir.lengthSquared();
    if (sqrDist == 0)
        return sqrDist;

    dir.normalize();

    return ((getDirection().dot(dir) > cos(m_neighbourhoodAngle))? sqrDist : INFINITY );
}

void Boid::update(ngl::Real _deltaT)
{
    m_transform.addPosition(_deltaT * m_velocity );
}


SceneObject *Boid::create(int _meshId)
{
        ngl::Transformation t;
        t.setPosition(utils::genRandPointInBox(-5.0, 5.0));

        Boid* boid = new Boid(_meshId, t);
        ngl::Vec4 v = utils::genRandPointInSphere(0.5);
        boid->setVelocity( v + v * (0.5 / v.length()));
        return boid;
//        ngl::Transformation t;
//        t.setPosition(ngl::Vec4());

//        Boid* boid = new Boid(_meshId, t);
//        ngl::Vec4 v(utils::ex);
//        boid->setVelocity( v + v * (0.5 / v.length()));
//        return boid;
}
