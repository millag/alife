#include "Rules.h"
#include <limits>

void Rule::setPriority(ngl::Real _p)
{
    assert( _p >= 0 && _p <= 1);
    m_priority = _p;
}

void Rule::setWeight(ngl::Real _w)
{
    assert( _w >= 0 && _w <= 1);
    m_weight = _w;
}




ngl::Vec4 Seek::getForce(const Boid *_boid)
{
    ngl::Vec4 steerForce = m_target - _boid->getPosition();
    steerForce.m_w = 0;
    if (steerForce.lengthSquared() < utils::C_ERR)
    {
        return steerForce;
    }
    ngl::Real weight = calcWeight(_boid, steerForce);
    steerForce.normalize();
    return  steerForce * _boid->getMaxSpeed() * weight * m_weight;
}

ngl::Real Seek::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    if (_steerForce.lengthSquared() > _boid->getPanicDistanceSqr())
        return 1;
    return _steerForce.length() / _boid->getPanicDistance();
}

ngl::Vec4 Flee::getForce(const Boid *_boid)
{
    ngl::Vec4 steerForce = _boid->getPosition() - m_target;
    steerForce.m_w = 0;
    if (steerForce.lengthSquared() < utils::C_ERR)
    {
        steerForce = utils::genRandPointOnSphere() * _boid->getMaxSpeed() * calcWeight(_boid, steerForce) * m_weight;
        steerForce.m_w = 0;
        return steerForce;
    }

    ngl::Real weight = calcWeight(_boid, steerForce);
    steerForce.normalize();
    return  steerForce * _boid->getMaxSpeed() * weight * m_weight;
}

ngl::Real Flee::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    if (_steerForce.lengthSquared() > _boid->getPanicDistanceSqr())
        return 0;
    return 1 - _steerForce.length() / _boid->getPanicDistance();
}




INeighboursServant &Separation::getServant() const
{
    return *(dynamic_cast<INeighboursServant*>(m_servant));
}


ngl::Vec4 Separation::getForce(const Boid *_boid)
{
    std::vector<Boid*> neighbours;
    getServant().getNeighbours(_boid, neighbours);

    ngl::Vec4 steerForce(0,0,0,0);

     typedef std::vector<Boid*>::const_iterator BIter;
     for (BIter it = neighbours.begin(); it != neighbours.end(); ++it)
     {
         Boid* other = (*it);
         ngl::Vec4 v = _boid->getPosition() - other->getPosition();
         if (v.length() < other->getBoundingRadius()  + _boid->getPanicDistance())
         {
             ngl::Vec4 target = other->getPosition();
             if (v.length() > other->getBoundingRadius())
             {
                 v.normalize();
                 target += v * other->getBoundingRadius();
             }
             m_flee.setTarget(target);
             steerForce += m_flee.getForce(_boid);
         }
     }

     utils::truncate(steerForce, _boid->getMaxSpeed());
     return steerForce * calcWeight(_boid, steerForce) * m_weight;
}

ngl::Real Separation::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    return 1.0;
}



INeighboursServant& Alignment::getServant() const
{
    return *(dynamic_cast<INeighboursServant*>(m_servant));
}

ngl::Vec4 Alignment::getForce(const Boid *_boid)
{
    std::vector<Boid*> neighbours;
    getServant().getNeighbours(_boid, neighbours);

    ngl::Vec4 steerForce(0,0,0,0);

    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        steerForce += (*it)->getVelocity();
    }

    if (neighbours.size())
    {
        steerForce /= neighbours.size();
        utils::truncate(steerForce, _boid->getMaxSpeed());
    }

    return steerForce * m_weight;
}

ngl::Real Alignment::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    return m_weight;
}





INeighboursServant &Cohesion::getServant() const
{
    return *(dynamic_cast<INeighboursServant*>(m_servant));
}

ngl::Vec4 Cohesion::getForce(const Boid *_boid)
{
    std::vector<Boid*> neighbours;
    getServant().getNeighbours(_boid, neighbours);

    ngl::Vec4 steerForce(0,0,0,0);
    ngl::Vec4 massCenter(0,0,0,1);
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        massCenter += (*it)->getPosition();
    }

    if (neighbours.size())
    {
        massCenter /= neighbours.size();
        m_seek.setTarget(massCenter);
        steerForce += m_seek.getForce(_boid);
    }

    return  steerForce * calcWeight(_boid, steerForce)* m_weight;
}

ngl::Real Cohesion::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    return 1.0;
}




ngl::Vec4 VolumeConstraint::getForce(const Boid *_boid)
{
    ngl::Vec4 steerForce(0,0,0,0);
    if (!utils::isInsideVolume(_boid->getPosition(), m_volume))
    {
        m_seek.setTarget(m_volume.getCenter());
        steerForce += m_seek.getForce(_boid);
        utils::truncate(steerForce, _boid->getMaxSpeed());
        return  steerForce * calcWeight(_boid, steerForce) * m_weight;
    }

    m_flee.setTarget(ngl::Vec4(m_volume.getBottomLeft().m_x, _boid->getPosition().m_y, _boid->getPosition().m_z));
    steerForce += m_flee.getForce(_boid);

    m_flee.setTarget(ngl::Vec4(_boid->getPosition().m_x, m_volume.getBottomLeft().m_y, _boid->getPosition().m_z));
    steerForce += m_flee.getForce(_boid);

    m_flee.setTarget(ngl::Vec4(_boid->getPosition().m_x, _boid->getPosition().m_y, m_volume.getBottomLeft().m_z));
    steerForce += m_flee.getForce(_boid);

    m_flee.setTarget(ngl::Vec4(m_volume.getTopRight().m_x, _boid->getPosition().m_y, _boid->getPosition().m_z));
    steerForce += m_flee.getForce(_boid);

    m_flee.setTarget(ngl::Vec4(_boid->getPosition().m_x, m_volume.getTopRight().m_y, _boid->getPosition().m_z));
    steerForce += m_flee.getForce(_boid);

    m_flee.setTarget(ngl::Vec4(_boid->getPosition().m_x, _boid->getPosition().m_y, m_volume.getTopRight().m_z));
    steerForce += m_flee.getForce(_boid);

    utils::truncate(steerForce, _boid->getMaxSpeed());
    return  steerForce * calcWeight(_boid, steerForce) * m_weight;
}

ngl::Real VolumeConstraint::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    return 1.0;
}


Wander::Wander(INeighboursServant *_servant, ngl::Real _priority, ngl::Real _weight, ngl::Real _wanderDist, ngl::Real _wanderRadius, ngl::Real _jitterAngle):
                Rule(_servant, _priority, _weight), m_wanderDist(_wanderDist), m_wanderRadius(_wanderRadius),m_jitterAngle(_jitterAngle)
{
    m_target = utils::genRandPointOnSphere();
    m_target.m_w = 0;
}

INeighboursServant &Wander::getServant() const
{
    return *(dynamic_cast<INeighboursServant*>(m_servant));
}

ngl::Vec4 Wander::getForce(const Boid *_boid)
{
    std::vector<Boid*> neighbours;
    getServant().getNeighbours(_boid, neighbours);

    ngl::Vec4 steerForce(0,0,0,0);

    if (neighbours.size() != 0)
    {
        return steerForce;
    }

    steerForce += _boid->getHeadingDir() * m_wanderDist + m_target * _boid->getTransform() * m_wanderRadius;
    utils::truncate(steerForce, _boid->getMaxSpeed());

//    calculate next target vector
    ngl::Real theta = utils::randf() * m_jitterAngle;
    ngl::Real phi = utils::randf() * m_jitterAngle;
    ngl::Real x = std::cos(phi)*std::cos(theta);
    ngl::Real y = std::cos(phi)*std::sin(theta);
    ngl::Real z = std::sin(phi);

    m_target = m_target + (ngl::Vec4(x, y, z, 0) - utils::C_EX);
    m_target.normalize();

    return  steerForce * calcWeight(_boid, steerForce)* m_weight;
}

ngl::Real Wander::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    return 1.0;
}


IObstacleServant &ObstacleAvoidance::getServant() const
{
    return *(dynamic_cast<IObstacleServant*>(m_servant));
}

ngl::Vec4 ObstacleAvoidance::getForce(const Boid *_boid)
{
    assert(_boid != NULL);
    ngl::Vec4 steerForce(0,0,0,0);

    std::vector<Obstacle *> obstacles;
    getServant().getObstacles(_boid, obstacles);
    if (!obstacles.size())
    {
        return steerForce;
    }

     typedef std::vector<Obstacle*>::const_iterator OIter;
     for (OIter it = obstacles.begin(); it != obstacles.end(); ++it)
     {
         Obstacle* obstacle = (*it);
         ngl::Vec4 v = _boid->getPosition() - obstacle->getPosition();
         if (v.length() < obstacle->getBoundingRadius()  + _boid->getObstacleLookupDistance())
         {
             ngl::Vec4 target = obstacle->getPosition();
             if (v.length() > obstacle->getBoundingRadius())
             {
                 v.normalize();
                 target += v * obstacle->getBoundingRadius();
             }
             m_flee.setTarget(target);
             steerForce += m_flee.getForce(_boid);
         }
     }

     utils::truncate(steerForce, _boid->getMaxSpeed());
     return steerForce * calcWeight(_boid, steerForce) * m_weight;
}

//ngl::Vec4 ObstacleAvoidance::getForce(const Boid *_boid)
//{
//    assert(_boid != NULL);
//    ngl::Vec4 steerForce(0,0,0,0);

//    std::vector<Obstacle *> obstacles;
//    getServant().getObstacles(_boid, obstacles);
//    if (!obstacles.size())
//    {
//        return steerForce;
//    }

//    ngl::Real nearestDistSqr = std::numeric_limits<ngl::Real>::max();
//    ngl::Vec4 nearestIPos(0,0,0);
//    ngl::Vec4 nearestINormal(0,0,0,0);
//    Obstacle* nearest = NULL;

//    ngl::Mat4 m =  _boid->getCurrentMatrix();
//    ngl::Mat4 toLocalTransform = m.inverse();

//    typedef std::vector<Obstacle*>::const_iterator OIter;
//    for (OIter it = obstacles.begin(); it != obstacles.end(); ++it)
//    {
//        Obstacle* obstacle = (*it);
//        assert(obstacle != NULL);

//        ngl::Vec4 localPos = obstacle->getPosition() * toLocalTransform;
////        test if obstacle is behind boid
//        if (localPos.m_z < 0)
//        {
//            continue;
//        }

////        test if obstacle is overlapping with boid's bounding cylinder
//        ngl::Real inflatedRadius = obstacle->getBoundingRadius() + _boid->getBoundingRadius();
//        if (ngl::Vec4(localPos.m_x, localPos.m_y, 0, 0).length() > inflatedRadius)
//        {
//            continue;
//        }

//        ngl::Vec4 iPos;
//        ngl::Vec4 iNormal;
//        ngl::Real distSqr = obstacle->findPossibleCollisionPoint(_boid, iPos, iNormal);
//        if (distSqr < nearestDistSqr)
//        {
//            nearestDistSqr = distSqr;
//            nearestIPos = iPos;
//            nearestINormal = iNormal;
//            nearest = obstacle;
//        }
//    }

//    if (nearest == NULL)
//    {
//        return steerForce;
//    }

//    ngl::Vec4 v = nearestIPos - _boid->getPosition();
//    steerForce += utils::reflect(v, nearestINormal);
////    apply breaking force if too close to obstacle
//    if (v.lengthSquared() < _boid->getPanicDistanceSqr())
//    {
//        ngl::Real breakingWeight = (1 - v.length() / _boid->getPanicDistance());
//        steerForce -= _boid->getHeadingDir() * _boid->getCurrentSpeed() * breakingWeight;
//    }

//    ngl::Real distWeight = ( 1  - v.length() / _boid->getObstacleLookupDistance());
//    return  steerForce * distWeight * m_weight;
//}

ngl::Real ObstacleAvoidance::calcWeight(const Boid *_boid, const ngl::Vec4 &_steerForce)
{
    return 1.0;
}
