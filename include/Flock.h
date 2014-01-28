#ifndef FLOCK_H
#define FLOCK_H

#include "Scene.h"
#include "Servant.h"

class Flock : public INeighboursServant, public IObstacleServant
{
public:
    Flock(const Scene& _scene);
    virtual ~Flock();

    bool isInFlock(const Boid* _boid) const;
    void joinBoid(Boid* _boid);
    void update(ngl::Real _deltaT);

    void getNeighbours(const Boid* _boid, std::vector<Boid*>& o_neighbours);
    void getObstacles(const Boid* _boid, std::vector<Obstacle*>& o_obstacles);
protected:
//    reference to scene object
//    note: this is needed to query for near by obstacles/scene bounds/ect.
    const Scene& m_scene;
//    note: boids are not owned by the flock so it won't care destroing them
    std::vector<Boid*> m_boids;
//    note: rules are created and owned by the flock itself so it must clean them up
    std::vector<Rule*> m_rules;

    std::map< const Boid*, std::vector<Boid*> > m_neighboursMap;
    std::map< const Boid*, std::vector<Obstacle*> > m_obstaclesMap;

    void initialize();
    void findNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours) const;
    void findObstacles(const Boid *_boid, std::vector<Obstacle*> &o_obstacles) const;
private:
    Flock();
};

#endif // FLOCK_H
