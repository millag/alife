#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Rules.h"

class Integrator{
public:
    static const Integrator& sGetInstance();
    ngl::Vec4 calculateAcceleration(const Boid* _boid, const std::vector<ngl::Vec4>& _forces, ngl::Real _deltaT) const;

private:
    Integrator() { }
    static Integrator* sIntegrator;
};

#endif // INTEGRATOR_H
