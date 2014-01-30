#include "Flock.h"
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/thread/detail/thread_group.hpp>

#include "Grid.h"
#include "Integrator.h"

class Callable : public INeighboursServant, public IObstacleServant
{

public:
    typedef std::vector<Boid*>::const_iterator BIter;

    Callable(Flock* _flock, BIter _begin, BIter _end, ngl::Real _deltaT):
        m_flock(_flock), m_begin(_begin), m_end(_end),m_deltaT(_deltaT)
    { }

    void operator()()
    {
        assert(m_flock != NULL);

        for (BIter it = m_begin; it != m_end; ++it)
        {
            Boid* boid = (*it);

            m_neighbours.clear();
            m_flock->findNeighbours(boid, m_neighbours);
            m_obstacles.clear();
            m_flock->findObstacles(boid, m_obstacles);

            typedef std::vector<Rule*>::const_iterator RIter;
            const std::vector<Rule*>& rules = boid->getRules();
            std::vector<ngl::Vec4> forces;
            forces.reserve(rules.size());
            for (RIter it = rules.begin(); it != rules.end(); ++it)
            {
                forces.push_back( (*it)->getForce(this, boid) );
            }

            ngl::Vec4 acc = m_integrator.calculateAcceleration(boid, forces, m_deltaT);
            boid->setAcceleration(acc);
        }
    }

    void getNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours)
    {
        assert(_boid != NULL);
        o_neighbours.insert(o_neighbours.begin(), m_neighbours.begin(), m_neighbours.end());
    }

    void getObstacles(const Boid *_boid, std::vector<Obstacle *> &o_obstacles)
    {
        assert(_boid != NULL);
        o_obstacles.insert(o_obstacles.begin(), m_obstacles.begin(), m_obstacles.end());
    }

private:
    Flock* m_flock;
    BIter m_begin;
    BIter m_end;
    ngl::Real m_deltaT;

    Integrator m_integrator;
    std::vector<Boid*> m_neighbours;
    std::vector<Obstacle*> m_obstacles;
};


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

// FIX: rules should be in boid and flock by inserted by builder
void Flock::initialize()
{
    m_rules.reserve(20);
    m_rules.push_back( new ObstacleAvoidance(.0, 1.0) );
    m_rules.push_back( new Separation(1.0, 0.7) );
    m_rules.push_back( new Alignment(0.5, 0.8) );
    m_rules.push_back( new Cohesion(0.5, 0.7) );
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
    rules.push_back(new Wander(0.4, 1.0));
    _boid->setRules(rules);
    m_boids.push_back(_boid);
}

void Flock::update(ngl::Real _deltaT)
{

    size_t nMaxThreads = boost::thread::hardware_concurrency();
    nMaxThreads = std::min(m_boids.size(), nMaxThreads);
//    std::cout << "Max threads: " << nMaxThreads << std::endl;

    unsigned nCnt = m_boids.size() / nMaxThreads;
    int remainder = m_boids.size() % nMaxThreads;
//    std::cout << "boids per thread: " << nCnt << " , Reminder: " << reminder << std::endl;

    typedef std::vector<Boid*>::const_iterator BIter;
    unsigned idx = 0;
//    boost::thread_group group;
    for (unsigned i = 0; i < nMaxThreads; ++i)
    {
        BIter beg = m_boids.begin() + idx;
        idx += nCnt + (((remainder) > 0)? 1 : 0);
        BIter end = m_boids.begin() + idx;

        Callable func(this, beg, end, _deltaT);
//        group.create_thread(func);

        func();
        remainder--;
    }
//    group.join_all();

    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        (*it)->update(_deltaT);
    }
}

// with grid
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
