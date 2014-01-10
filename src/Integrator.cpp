#include "integrator.h"

ngl::Vec4 Integrator::calculateAcceleration(const std::vector<Rule*>& _rules, const Boid* _boid,const std::vector<Boid *>& _neighbours) const
{
    ngl::Vec4 acceleration;
    typedef std::vector<Rule*>::const_iterator RIter;
    for (RIter rit = _rules.begin(); rit != _rules.end(); ++rit)
    {
        Rule* rule = (*rit);
        acceleration += rule->getForce(_boid, _neighbours) / _boid->getMass();
    }

    return acceleration;
}
