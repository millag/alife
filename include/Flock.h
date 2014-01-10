#ifndef FLOCK_H
#define FLOCK_H

#include "Scene.h"
#include "integrator.h"

class Flock
{
public:
    Flock(const Scene& _scene);
    virtual ~Flock();

    bool isInFlock(const Boid* _boid) const;
    void findNeighboursWithinDistance(const Boid* _boid, ngl::Real _distance, std::vector<Boid *> &o_neighbours) const;
    void joinBoid(Boid* _boid);
    void update(ngl::Real _deltaT);

protected:
//    reference to scene object
//    note: this is needed to query for near by obstacles/scene bounds/ect.
    const Scene& m_scene;
//    object responsible for combining/integration over all the rules
    Integrator* m_integrator;
//    note: boids are not owned by the flock so it won't care destroing them
    std::vector<Boid*> m_boids;
//    note: rules are created and owned by the flock itself so it must clean them up
    std::vector<Rule*> m_rules;
    ngl::Real m_maxSpeed;

    void initialize();
private:
    Flock();
};

#endif // FLOCK_H
