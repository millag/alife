#include "Flock.h"
#include <algorithm>

#include "Grid.h"
#include "Integrator.h"



Flock::Flock():m_scene(Scene()) { }

Flock::Flock(const Scene& _scene):m_scene(_scene)
{
    initialize();
}

Flock::~Flock()
{
    typedef std::vector<Rule*>::const_iterator RIter;
    for (RIter it = m_rules.begin(); it != m_rules.end(); ++it)
    {
        delete (*it);
    }
}

void Flock::initialize()
{
    Integrator::sGetInstance();

    m_rules.reserve(20);
    m_rules.push_back( new ObstacleAvoidance(this, 1.0, 1.0) );
    m_rules.push_back( new Separation(this, 1.0, 1.0) );
    m_rules.push_back( new Alignment(this, 0.5, 0.5) );
    m_rules.push_back( new Cohesion(this, 0.5, 0.4) );
    m_rules.push_back( new VolumeConstraint(m_scene.getBoundingVolume(), 0.4, 1.0) );
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

        typedef std::vector<Rule*>::const_iterator RIter;

        const std::vector<Rule*>& rules = boid->getRules();
        std::vector<ngl::Vec4> forces;
        forces.reserve(rules.size());
        for (RIter it = rules.begin(); it != rules.end(); ++it)
        {
            forces.push_back( (*it)->getForce(boid) );
        }

        ngl::Vec4 acc = Integrator::sGetInstance().calculateAcceleration(boid, forces, _deltaT);
        boid->setAcceleration(acc);
    }

    m_neighboursMap.clear();
    m_obstaclesMap.clear();

    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        (*it)->update(_deltaT);
    }
}

void Flock::getNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours)
{
    assert(_boid != NULL);
    if (m_neighboursMap.count(_boid) == 0)
    {
        findNeighbours(_boid, m_neighboursMap[_boid]);
    }
    o_neighbours.insert(o_neighbours.begin(), m_neighboursMap[_boid].begin(), m_neighboursMap[_boid].end());
}

void Flock::getObstacles(const Boid *_boid, std::vector<Obstacle *> &o_obstacles)
{
    assert(_boid != NULL);
    if (m_obstaclesMap.count(_boid) == 0)
    {
        findObstacles(_boid, m_obstaclesMap[_boid]);
    }
    o_obstacles.insert(o_obstacles.begin(), m_obstaclesMap[_boid].begin(), m_obstaclesMap[_boid].end());
}

// grid used
void Flock::findNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours) const
{
    assert(_boid != NULL);
    std::vector<Boid *> neighbours;
    m_scene.getGrid().findObjectsWithinDistance(_boid->getPosition(), _boid->getNeighbourhoodDistance(), neighbours);
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = neighbours.begin(); it != neighbours.end(); ++it)
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
