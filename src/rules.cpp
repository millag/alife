#include "rules.h"

Rule::Rule(ngl::Real _weight):m_weight(_weight)
{ }

Rule::~Rule()
{ }

ngl::Real Rule::getWeiht() const
{
    return m_weight;
}

void Rule::setWeight(const ngl::Real &_weight)
{
    m_weight = _weight;
}

Cohesion::Cohesion(ngl::Real _weight):Rule(_weight)
{ }

ngl::Vec4 Cohesion::getForce(const Boid* _boid, const std::vector<Boid *> &_neighbours)
{
    ngl::Vec4 force;

//    calculate "percieved" center of mass of nearby flockmates
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = _neighbours.begin(); it != _neighbours.end(); ++it)
    {
        force += (*it)->getPosition();
    }

    if (_neighbours.size())
    {
        force /= _neighbours.size();
    }

    return (force - _boid->getPosition()) * m_weight;
}

Separation::Separation(ngl::Real _distance, ngl::Real _weight):
    Rule(_weight), m_sqrDistance(_distance * _distance)
{ }

ngl::Real Separation::getDistance() const
{
    return sqrt(m_sqrDistance);
}

void Separation::setDistance(const ngl::Real& _distance)
{
    m_sqrDistance = _distance * _distance;
}

ngl::Vec4 Separation::getForce(const Boid *_boid, const std::vector<Boid *> &_neighbours)
{
    ngl::Vec4 force;

     typedef std::vector<Boid*>::const_iterator BIter;
     for (BIter it = _neighbours.begin(); it != _neighbours.end(); ++it)
     {
         ngl::Vec4 v = (*it)->getPosition() - _boid->getPosition();
         if (v.lengthSquared() > m_sqrDistance)
             continue;

         force -= v * (1 - v.lengthSquared() / m_sqrDistance);
     }

     return force * m_weight;
}

Alignment::Alignment(ngl::Real _weight):Rule(_weight)
{ }

ngl::Vec4 Alignment::getForce(const Boid *_boid, const std::vector<Boid *> &_neighbours)
{
    ngl::Vec4 force;

    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = _neighbours.begin(); it != _neighbours.end(); ++it)
    {
        force += (*it)->getVelocity();
    }

    if (_neighbours.size())
    {
        force /= _neighbours.size();
    }

    return (force - _boid->getVelocity()) * m_weight;
}
