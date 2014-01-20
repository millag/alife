#ifndef GRID_H
#define GRID_H

#include <list>
#include "Boid.h"
#include "Obstacle.h"

struct Cell {
public:
    std::list<const Boid*> m_boidList;
};

class Grid {

public:
    Grid(ngl::Real _size, ngl::Real _divisions);
    ~Grid() { }

    void addObject(const Boid* _boid);
    void addObjects(const std::vector<Boid*> _boids);
    void getObjectsWithinDistance(const Boid* _boid, ngl::Real _dist);
    void update();

protected:
    ngl::Real m_size;
    ngl::Real m_divisions;
    std::vector<Cell> m_cells;

    unsigned getObjectCellIdx(const Boid* _boid) const;
};

#endif // GRID_H
