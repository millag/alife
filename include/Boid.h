#ifndef BOID_H
#define BOID_H

#include "Utils.h"
#include "BaseObjects.h"

class Rule;

class Boid : public MovingObject
{
public:
    Boid(const Mesh* _mesh, int _shaderId, const ngl::Transformation& _transform = ngl::Transformation());
    virtual ~Boid();

    const std::vector<Rule*>& getRules() const { return m_rules; }
    void setRules(const std::vector<Rule*>& _rules);

    ngl::Real getNeighbourhoodDistance() const { return m_neighbourhoodDist; }
    ngl::Real getNeighbourhoodDistanceSqr() const { return m_neighbourhoodDist * m_neighbourhoodDist; }
    void setNeighbourhoodDistance(ngl::Real _d);

    ngl::Real getNeighbourhoodFOV() const { return m_neighbourhoodFOV; }
    void setNeighbourhoodFOV(ngl::Real _fov) { m_neighbourhoodFOV = _fov; }

    ngl::Real getPanicDistance() const { return m_panicDist; }
    ngl::Real getPanicDistanceSqr() const { return m_panicDist * m_panicDist; }
    void setPanicDistance(ngl::Real _d);

    ngl::Real getObstacleLookupDistance() const { return m_obstacleLookupDist; }
    ngl::Real getObstacleLookupDistanceSqr() const { return m_obstacleLookupDist * m_obstacleLookupDist; }
    void setObstacleLookupDistance(ngl::Real _d);

    bool isInNeighbourhood(const MovingObject &_so) const;


//    virtual void update(ngl::Real _deltaT);

protected:
    ngl::Real m_neighbourhoodDist;
//    measured in radians
    ngl::Real m_neighbourhoodFOV;
    ngl::Real m_panicDist;
    ngl::Real m_obstacleLookupDist;
    std::vector<Rule*> m_rules;
};

#endif // BOID_H
