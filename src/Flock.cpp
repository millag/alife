#include "Flock.h"
#include <algorithm>

Flock::Flock():m_scene(Scene()) { }

Flock::Flock(const Scene& _scene):m_scene(_scene)
{
    initialize();
}

Flock::~Flock()
{
    delete m_integrator;
    typedef std::vector<Rule*>::const_iterator RIter;
    for (RIter it = m_rules.begin(); it != m_rules.end(); ++it)
    {
        delete (*it);
    }
}

void Flock::initialize()
{
    m_integrator = new Integrator();

//    m_rules.resize(2, NULL);
//    m_rules[0] = new ObstacleAvoidance(this, 1.0, 1.0);
//    m_rules[1] = new VolumeConstraint(m_scene.getBoundingVolume(), 0.4, 1.0);

    m_rules.resize(5, NULL);
    m_rules[0] = new ObstacleAvoidance(this, 1.0, 1.0);
    m_rules[1] = new Separation(this, 1.0, 0.9);
    m_rules[2] = new Alignment(this, 0.5, 0.5);
    m_rules[3] = new Cohesion(this, 0.5, 0.4);
    m_rules[4] = new VolumeConstraint(m_scene.getBoundingVolume(), 0.4, 1.0);
}

bool Flock::isInFlock(const Boid *_boid) const
{
    typedef std::vector<Boid*>::const_iterator BIter;
    BIter it = std::find(m_boids.begin(), m_boids.end(), _boid);
    return (it != m_boids.end());
}

// FIX: rules should be in boid and flock by inserted by builder
void Flock::joinBoid(Boid *_boid)
{
    assert(!isInFlock(_boid));

    std::vector<Rule*> rules(m_rules);
    rules.push_back(new Wander(this, 0.4, 1.0));
    _boid->setRules(rules);
    m_boids.push_back(_boid);
}

void Flock::update(ngl::Real _deltaT)
{
    typedef std::vector<Boid*>::iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        Boid* boid = (*it);

        m_neighbours.clear();
        findNeighbours(boid, m_neighbours);
        m_obstacles.clear();
        findObstacles(boid, m_obstacles);

        typedef std::vector<Rule*>::const_iterator RIter;
        int i = 0;

        const std::vector<Rule*>& rules = boid->getRules();
        std::vector<ngl::Vec4> forces(rules.size());
        for (RIter it = rules.begin(); it != rules.end(); ++it)
        {
            forces[i++] = (*it)->getForce(boid);
        }

        ngl::Vec4 acc = m_integrator->calculateAcceleration(boid, forces, _deltaT);
        boid->setAcceleration(acc);
    }

    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        (*it)->update(_deltaT);
    }
}

//FIX: use grid
void Flock::findNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours) const
{
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        if ((*it) == _boid)
        {
            continue;
        }
        if (_boid->isInNeighbourhood(*(*it)))
        {
            o_neighbours.push_back((*it));
        }
    }
}


//FIX: use grid
void Flock::findObstacles(const Boid *_boid, std::vector<Obstacle*> &o_obstacles) const
{
    const std::vector<Obstacle*>& obstacles = m_scene.getObstacles();
    typedef std::vector<Obstacle*>::const_iterator OIter;
    for (OIter it = obstacles.begin(); it != obstacles.end(); ++it)
    {
        if (_boid->isInRange((*(*it))))
        {
            o_obstacles.push_back((*it));
        }
    }
}

void Flock::getNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours)
{
    o_neighbours.insert(o_neighbours.begin(), m_neighbours.begin(), m_neighbours.end());
}

void Flock::getObstacles(const Boid *_boid, std::vector<Obstacle *> &o_obstacles)
{
    o_obstacles.insert(o_obstacles.begin(), m_obstacles.begin(), m_obstacles.end());
}
