#include "Integrator.h"

ngl::Vec4 Integrator::calculateAcceleration(const Boid* _boid, const std::vector<ngl::Vec4>& _forces, ngl::Real _deltaT) const
{
    ngl::Vec4 acc(0,0,0,0);
    typedef std::vector<ngl::Vec4>::const_iterator VIter;
    for (VIter it = _forces.begin(); it != _forces.end(); ++it)
    {
        assert((*it).m_w == 0);
        acc += (*it);
    }

    acc /= _boid->getMass();
    return acc;
}
