#ifndef BOID_H
#define BOID_H

#include "utils.h"
#include "geometry.h"

class Boid : public SceneObject
{
public:
    Boid(int _meshId = -1, const ngl::Transformation& _transform = ngl::Transformation());
    Boid(const Boid& _boid);

    virtual ~Boid();

    ngl::Real getNeighbourhoodDistance() const;
    ngl::Real getNeighbourhoodAngle() const;

    ngl::Real getMaxSpeed() const;
    void  setMaxSpeed(const ngl::Real& _maxSpeed);


    ngl::Vec4 getDirection() const;
    const ngl::Vec4& getVelocity() const;
    void setVelocity(const ngl::Vec4& v);

    ngl::Real getMass() const;
    void  setMass(const ngl::Real& _mass);

//    returns the distance to the object if within boid's field of view, otherwise INFINITY
    ngl::Real getSqrDistanceToObject(const SceneObject& _object) const;
    bool isInNeighbourhood(const SceneObject& _object) const;

//    update the boid's current position according to current velocity
    void update(ngl::Real _deltaT);

    static SceneObject* create(int _meshId);

protected:

//    speed measured in units per sec
    ngl::Real m_maxSpeedSqr;
//    current sped is the length of velocity vec
    ngl::Vec4 m_velocity;
    ngl::Real m_mass;
    ngl::Real m_neighbourhoodSqrDistance;
//    measured in radians
    ngl::Real m_neighbourhoodAngle;
};

#endif // BOID_H
