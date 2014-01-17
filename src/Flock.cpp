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

bool compareRules(Rule *_lhs, Rule *_rhs)
{
    assert(_lhs != NULL && _rhs != NULL);
    return _lhs->getPriority() < _rhs->getPriority();
}

void Flock::initialize()
{
    m_integrator = new Integrator();

//    m_rules.resize(1, NULL);
//    m_rules[0] = new Alignment(this, 0.5, 1.0);
    m_rules.resize(4, NULL);
    m_rules[0] = new Separation(this, 1.0, 1.0);
    m_rules[1] = new Alignment(this, 0.5, 0.5);
    m_rules[2] = new Cohesion(this, 0.5, 0.4);
    m_rules[3] = new VolumeConstraint(m_scene.getBoundingVolume(), 0.4, 1.0);

    std::stable_sort(m_rules.begin(), m_rules.end(), compareRules);
}

bool Flock::isInFlock(const Boid *_boid) const
{
    typedef std::vector<Boid*>::const_iterator BIter;
    BIter it = std::find(m_boids.begin(), m_boids.end(), _boid);
    return (it != m_boids.end());
}

void Flock::joinBoid(Boid *_boid)
{
    assert(!isInFlock(_boid));
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

        typedef std::vector<Rule*>::const_iterator RIter;
        int i = 0;
        std::vector<ngl::Vec4> forces(m_rules.size());
        for (RIter it = m_rules.begin(); it != m_rules.end(); ++it)
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

void Flock::findNeighbours(const Boid *_boid, std::vector<Boid *> &o_neighbours) const
{
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        if ((*it) == _boid)
            continue;

        if (_boid->isInNeighbourhood(*(*it)))
            o_neighbours.push_back((*it));
    }
}

