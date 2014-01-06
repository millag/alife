#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "rules.h"

class Integrator{
public:
    Integrator() { }
    ~Integrator() { }

    ngl::Vec4 calculateAcceleration(const std::vector<Rule*>& _rules, const Boid* _boid,const std::vector<Boid *>& _neighbours) const;
};

#endif // INTEGRATOR_H
