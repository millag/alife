#include "flock.h"

Flock::Flock():m_scene(Scene()) { }

Flock::Flock(const Scene& _scene):m_scene(_scene),m_maxSpeed(2.0)
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

    m_rules.resize(3, NULL);
    m_rules[0] = new Alignment(0.1);
    m_rules[1] = new Separation(5.0, 1.0);
    m_rules[2] = new Cohesion(0.15);
}

bool Flock::isInFlock(const Boid *_boid) const
{
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        if ((*it) == _boid)
            return true;
    }
    return false;
}

void Flock::joinBoid(Boid *_boid)
{
    assert(!isInFlock(_boid));
    _boid->setMaxSpeed(m_maxSpeed);
    m_boids.push_back(_boid);
}

void Flock::update(ngl::Real _deltaT)
{
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        Boid* boid = (*it);
        std::vector<Boid*> neighbours;
        findNeighboursWithinDistance(boid, boid->getNeighbourhoodDistance(), neighbours);

        ngl::Vec4 acc = m_integrator->calculateAcceleration( m_rules, boid, neighbours);
        boid->setVelocity(boid->getVelocity() + _deltaT * acc);
    }

    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        (*it)->update(_deltaT);
    }
}

void Flock::findNeighboursWithinDistance(const Boid *_boid, ngl::Real _distance, std::vector<Boid *> &o_neighbours) const
{
    ngl::Real searchDistance = _distance * _distance;

    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_boids.begin(); it != m_boids.end(); ++it)
    {
        Boid* boid = (*it);
        if (boid == _boid)
            continue;

        ngl::Real sqrDist = boid->getSqrDistanceToObject(*_boid);
        if (sqrDist <= searchDistance)
            o_neighbours.push_back(boid);
    }
}
