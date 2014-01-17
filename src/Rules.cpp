#include "Rules.h"

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
    const std::vector<Boid *> &neighbours = getServant().getNeighbours(_boid);
    ngl::Vec4 steerForce(0,0,0,0);

     typedef std::vector<Boid*>::const_iterator BIter;
     for (BIter it = neighbours.begin(); it != neighbours.end(); ++it)
     {
         m_flee.setTarget((*it)->getPosition());
         steerForce += m_flee.getForce(_boid);
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
    const std::vector<Boid *> &neighbours = getServant().getNeighbours(_boid);
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
    const std::vector<Boid *> &neighbours = getServant().getNeighbours(_boid);
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
