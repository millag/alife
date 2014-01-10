#ifndef SERVANT_H
#define SERVANT_H

#include "Boid.h"

class IServant
{
public:
    virtual ~IServant() { }
};

class INeighboursServant : public IServant
{
public:
    virtual ~INeighboursServant() { }
    virtual const std::vector<Boid*>& getNeighbours(const Boid* _boid) = 0;
};

class IObstacleServant : public IServant
{
public:
    virtual ~IObstacleServant() { }
    virtual void getObstacles() = 0;
};

#endif // SERVANT_H
