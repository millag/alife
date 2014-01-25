
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Mesh.h"
#include "ngl/Transformation.h"

class RenderObject
{
public:
    RenderObject(const Mesh* _mesh, int _shaderId, const ngl::Transformation& _transform = ngl::Transformation());
    virtual ~RenderObject();

    unsigned getMeshId() const;
    const Mesh* getMesh() const;
    void setMesh(const Mesh* _mesh);
    const ngl::Transformation& getTransform() const { return m_transform; }
    void setTransform(const ngl::Transformation& _transform);

    ngl::Real getBoundingRadius() const { return m_boundingRadius; }

protected:
    const Mesh* m_mesh;
    ngl::Transformation m_transform;
    int m_shaderId;
    ngl::Real m_boundingRadius;

    void calcBoundingRadius();

private:
    RenderObject();
};

class MovingObject : public RenderObject
{
public:
    MovingObject(const Mesh* _mesh, int _shaderId, const ngl::Transformation& _transform = ngl::Transformation());
    virtual ~MovingObject() { }

    const ngl::Vec4& getHeadingDir() const;
    const ngl::Vec4& getPosition() const;
    void setPosition(const ngl::Vec4& _pos)
    {
        m_position = _pos;
    }

    ngl::Vec4 getVelocity() const { return m_velocity; }
    void setVelocity(const ngl::Vec4& _v);

    ngl::Vec4 getAcceleration() const { return m_acceleration; }
    void setAcceleration(const ngl::Vec4& _a)
    {
        assert(_a.m_w == 0);
        m_acceleration = _a;
    }

    ngl::Real getMaxSpeed() const { return m_maxSpeed; }
    ngl::Real getMaxSpeedSqr() const { return m_maxSpeedSqr; }
    void  setMaxSpeed(ngl::Real _ms)
    {
        assert(_ms >= 0);
        m_maxSpeed = _ms;
        m_maxSpeedSqr = _ms * _ms;
    }

    ngl::Real getMass() const { return m_mass; }
    void  setMass(ngl::Real _m)
    {
        assert(_m > 0);
        m_mass = _m;
    }

    ngl::Real getMaxTurningAngle() const { return m_maxTurningAngle; }
    void  setMaxTurningAngle(ngl::Real _mta)
    {
        assert(_mta >= 0);
        m_maxTurningAngle = _mta;
    }

    virtual void update(ngl::Real _deltaT);

protected:
//    measured in units per sec, kept in global space coords
    ngl::Vec4 m_acceleration;
//    measured in units per sec, kept in global space coords
    ngl::Vec4 m_velocity;
//    measured in units per sec
    ngl::Real m_maxSpeed;
    ngl::Real m_maxSpeedSqr;
//    measured in radians
    ngl::Real m_maxTurningAngle;
//    measured in kg
    ngl::Real m_mass;
//    normalized vector pointing the direction the object is facing in global space coords
    ngl::Vec4 m_headingDir;
    ngl::Vec4 m_position;

    void updateHeading();
};

#endif // GEOMETRY_H
