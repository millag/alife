#ifndef RULES_H
#define RULES_H

#include "ngl/Util.h"
#include "Boid.h"
#include "Servant.h"


class Rule
{
public:
    Rule(ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):m_priority(_priority),m_weight(_weight) { }
    virtual ~Rule() { }

    ngl::Real getPriority() const { return m_priority; }
    void setPriority(ngl::Real _p);

    ngl::Real getWeiht() const { return m_weight; }
    void setWeight(ngl::Real _w);

    virtual ngl::Vec4 getForce(IServant* _servant,const Boid* _boid) = 0;

protected:
//    IServant* m_servant;
//  affects the order in which the rules are beeing evaluated; is in range [0, 1]
    ngl::Real m_priority;
//  affects MovingObject's affinity to steer in the calculated direction; is in range [0, 1]
    ngl::Real m_weight;

    virtual ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce) = 0;

private:
    Rule():m_priority(1.0),m_weight(1.0) { }
};

class Seek : public Rule
{
public:
    Seek(const ngl::Vec4& _target, ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight),m_target(_target)
    { }

    const ngl::Vec4& getTarget() const { return m_target; }
    void setTarget(const ngl::Vec4& _target) { m_target = _target; }
    ngl::Vec4 getForce(IServant* _servant,const Boid* _boid);

protected:
    ngl::Vec4 m_target;

    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};

class Flee : public Rule
{
public:
    Flee(const ngl::Vec4& _target, ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight),m_target(_target)
    { }

    const ngl::Vec4& getTarget() const { return m_target; }
    void setTarget(const ngl::Vec4& _target) { m_target = _target; }
    ngl::Vec4 getForce(IServant *_servant, const Boid* _boid);

protected:
    ngl::Vec4 m_target;

    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};

class Separation : public Rule
{
public:
    Separation(ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight)
    { }
    INeighboursServant& getServant() const;
    ngl::Vec4 getForce(IServant* _servant, const Boid* _boid);

protected:
    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};

class Alignment : public Rule
{
public:
    Alignment(ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight)
    { }
    INeighboursServant &getServant() const;
    ngl::Vec4 getForce(IServant *_servant, const Boid* _boid);

protected:
    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};

// Cohesion
// calculate average location (i.e. center) of all nearby boids, calculate steering vector towards that location
class Cohesion : public Rule
{
public:
    Cohesion(ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight)
    { }
    INeighboursServant& getServant() const;
    ngl::Vec4 getForce(IServant *_servant, const Boid* _boid);

protected:
    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};

class VolumeConstraint : public Rule
{
public:
    VolumeConstraint(const AABB& _volume, ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight),m_volume(_volume)
    { }
    const AABB& getVolume() const { return m_volume; }
    void setVolume(const AABB& _volume) { m_volume = _volume; }
    ngl::Vec4 getForce(IServant *_servant, const Boid* _boid);

protected:
    AABB m_volume;

    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};


class Wander : public Rule
{
public:
    Wander(ngl::Real _priority = 1.0, ngl::Real _weight = 1.0,
           ngl::Real _wanderDist = 3.0, ngl::Real _wanderRadius = 1.0, ngl::Real _jitterAngle = ngl::PI2 / 10);

    ngl::Vec4 getForce(IServant* _servant, const Boid* _boid);

protected:
    ngl::Real m_wanderDist;
    ngl::Real m_wanderRadius;
    ngl::Real m_jitterAngle;
    ngl::Vec4 m_target;

    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};


class ObstacleAvoidance : public Rule
{
public:
    ObstacleAvoidance(ngl::Real _priority = 1.0, ngl::Real _weight = 1.0):
        Rule(_priority, _weight)
    { }

    ngl::Vec4 getForce(IServant *_servant, const Boid* _boid);

protected:
//    Flee m_flee;

    ngl::Real calcWeight(const Boid* _boid, const ngl::Vec4& _steerForce);
};


#endif // RULES_H
