#include "Boid.h"

#include <algorithm>
#include "ngl/Util.h"
#include "Rules.h"

bool compareRules(Rule *_lhs, Rule *_rhs)
{
    assert(_lhs != NULL && _rhs != NULL);
    return _lhs->getPriority() < _rhs->getPriority();
}


Boid::Boid(const Mesh *_mesh, int _shaderId, const ngl::Mat4 &_transform):
    MovingObject(_mesh, _shaderId, _transform)
{
    m_neighbourhoodDist = (m_mesh == NULL)? 0 : m_mesh->getBoundingRadius();
    m_neighbourhoodFOV = 0;
    m_panicDist = (m_mesh == NULL)? 0 : m_mesh->getBoundingRadius();
    m_obstacleLookupDist = (m_mesh == NULL)? 0 : m_mesh->getBoundingRadius();
}

Boid::~Boid() { }


void Boid::setRules(const std::vector<Rule*>& _rules)
{
    m_rules.clear();
    m_rules.insert(m_rules.begin(), _rules.begin(), _rules.end());
    std::stable_sort(m_rules.begin(), m_rules.end(), compareRules);
}

void Boid::setNeighbourhoodDistance(ngl::Real _d)
{
    assert(_d >= 0);
    m_neighbourhoodDist = _d + ((m_mesh == NULL)? 0 : m_mesh->getBoundingRadius());
}

ngl::Real Boid::getPanicDistance() const
{
    return m_panicDist;
}

void Boid::setPanicDistance(ngl::Real _d)
{
    assert(_d >= 0);
    m_panicDist = _d + ((m_mesh == NULL)? 0 : m_mesh->getBoundingRadius());
}

ngl::Real Boid::getObstacleLookupDistance() const
{
     return m_obstacleLookupDist * (1 + getCurrentSpeed() / getMaxSpeed() * 0.5);
}

void Boid::setObstacleLookupDistance(ngl::Real _d)
{
    assert(_d >= 0);
    m_obstacleLookupDist = _d + ((m_mesh == NULL)? 0 : m_mesh->getBoundingRadius());
}

bool Boid::isInNeighbourhood(const Boid &_boid) const
{
    ngl::Vec4 dir = _boid.getPosition() - getPosition();
    ngl::Real distSqr = dir.lengthSquared();
    if (distSqr < utils::C_ERR)
        return true;

    dir.normalize();
    ngl::Real neighbourDistSqr = std::pow(getNeighbourhoodDistance() , 2);
    return (distSqr < neighbourDistSqr) && (getHeadingDir().dot(dir) > cos(m_neighbourhoodFOV));
}

bool Boid::isInRange(const Obstacle &_obstacle) const
{
    ngl::Vec4 dir = _obstacle.getPosition() - getPosition();
    ngl::Real distSqr = dir.lengthSquared();
    if (distSqr < utils::C_ERR)
        return true;

    dir.normalize();
    ngl::Real lookUpDistSqr = std::pow(getObstacleLookupDistance() , 2);
    return (distSqr <= lookUpDistSqr) && (getHeadingDir().dot(dir) > cos(m_neighbourhoodFOV));
}
