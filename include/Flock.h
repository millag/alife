#ifndef FLOCK_H
#define FLOCK_H

#include "Scene.h"
#include "Integrator.h"
#include "Servant.h"

class Flock : public INeighboursServant
{
public:
    Flock(const Scene& _scene);
    virtual ~Flock();

    bool isInFlock(const Boid* _boid) const;
    void joinBoid(Boid* _boid);
    void update(ngl::Real _deltaT);

    const std::vector<Boid*>& getNeighbours(const Boid* _boid) { return m_neighbours; }

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

    std::vector<Boid*> m_neighbours;

    void initialize();
    void findNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours) const;
private:
    Flock();
};

#endif // FLOCK_H
