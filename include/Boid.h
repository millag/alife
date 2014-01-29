#ifndef BOID_H
#define BOID_H

#include "Utils.h"
#include "Obstacle.h"

class Rule;

class Boid : public MovingObject
{
public:
    Boid(const Mesh* _mesh, int _shaderId, const ngl::Mat4 &_transform = ngl::Mat4());
    virtual ~Boid();

    const std::vector<Rule*>& getRules() const { return m_rules; }
    void setRules(const std::vector<Rule*>& _rules);

    ngl::Real getNeighbourhoodDistance() const { return m_neighbourhoodDist; }
    void setNeighbourhoodDistance(ngl::Real _d);

    ngl::Real getNeighbourhoodFOV() const { return m_neighbourhoodFOV; }
    void setNeighbourhoodFOV(ngl::Real _fov) { m_neighbourhoodFOV = _fov; }

    ngl::Real getPanicDistance() const;
    void setPanicDistance(ngl::Real _d);

    ngl::Real getObstacleLookupDistance() const;
    void setObstacleLookupDistance(ngl::Real _d);

    bool isInNeighbourhood(const Boid &_boid) const;
    bool isInRange(const Obstacle &_obstacle) const;

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
