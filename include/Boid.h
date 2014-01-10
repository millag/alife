#ifndef BOID_H
#define BOID_H

#include "Utils.h"
#include "BaseObjects.h"

class Boid : public MovingObject
{
public:
    Boid(const Mesh* _mesh, int _shaderId, const ngl::Transformation& _transform = ngl::Transformation());
    virtual ~Boid();

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

    bool isInNeighbourhood(const RenderObject& _so) const;

    static RenderObject *sCreate(const Mesh* _mesh);

    virtual void update(ngl::Real _deltaT);

protected:
    ngl::Real m_neighbourhoodDist;
//    measured in radians
    ngl::Real m_neighbourhoodFOV;
    ngl::Real m_panicDist;
    ngl::Real m_obstacleLookupDist;
};

#endif // BOID_H
