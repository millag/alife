#ifndef SERVANT_H
#define SERVANT_H

#include "Boid.h"
#include "Obstacle.h"

class IServant
{
public:
    virtual ~IServant() { }
};

class INeighboursServant : virtual public IServant
{
public:
    virtual ~INeighboursServant() { }
    virtual void getNeighbours(const Boid* _boid, std::vector<Boid*>& o_neighbours) = 0;
};

class IObstacleServant : virtual public IServant
{
public:
    virtual ~IObstacleServant() { }
    virtual void getObstacles(const Boid* _boid, std::vector<Obstacle*>& o_obstacles) = 0;
};

#endif // SERVANT_H
