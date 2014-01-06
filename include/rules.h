#ifndef RULES_H
#define RULES_H

#include "Boid.h"

class Rule
{
public:
    Rule(ngl::Real _weight = 1.0);
    virtual ~Rule();

    ngl::Real getWeiht() const;
    void setWeight(const ngl::Real& _weight);

    virtual ngl::Vec4 getForce(const Boid*_boid, const std::vector<Boid*>& _neighbours)=0;

protected:
    ngl::Real m_weight;
};

class Alignment : public Rule
{
public:
    Alignment(ngl::Real _weight = 1.0);
    ngl::Vec4 getForce(const Boid*_boid, const std::vector<Boid*>& _neighbours);
};

class Separation : public Rule
{
public:
    Separation(ngl::Real _distance = 1.0, ngl::Real _weight = 1.0);

    ngl::Real getDistance() const;
    void setDistance(const ngl::Real& _distance);
    ngl::Vec4 getForce(const Boid*_boid, const std::vector<Boid*>& _neighbours);
private:
    ngl::Real m_sqrDistance;
};

class Cohesion : public Rule
{
public:
    Cohesion(ngl::Real _weight = 1.0);
    ngl::Vec4 getForce(const Boid*_boid, const std::vector<Boid*>& _neighbours);
};

#endif // RULES_H
