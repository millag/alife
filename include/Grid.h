#ifndef GRID_H
#define GRID_H

#include <list>
#include "Boid.h"

struct Cell {
public:
    std::list<unsigned> m_objectIdxList;
};

class Grid {

public:
    Grid(ngl::Real _size, unsigned _divisions);
    ~Grid() { }

//    checks if object has already been added to grid
    bool isObjectInGrid(const Boid* _object) const;
//    adds object to grid
    void addObject(Boid* _object);
//    adds multiple objects to grid
    void addObjects(const std::vector<Boid*> _objects);
//    finds all objects within distance from a point
    void findObjectsWithinDistance(const ngl::Vec4& _pos, ngl::Real _dist, std::vector<Boid*>& o_objects) const;

//    allocates memory for grid
    void initialize();
//   updates objects' cells
    void update();

protected:
    ngl::Real m_size;
    unsigned m_divisions;
    unsigned m_divisionsSqr;
    ngl::Real m_cellSize;
    AABB m_volume;

    std::vector<Cell> m_cells;
    std::vector<Boid*> m_objects;

    unsigned findCellIdx(const ngl::Vec4& _pos, unsigned& o_i, unsigned& o_j, unsigned &o_k) const;
    unsigned calcIdx(unsigned _i, unsigned _j, unsigned _k) const;
    void findTestCells(const ngl::Vec4& _pos, ngl::Real _dist, std::vector<unsigned>& o_testCells) const;
};

#endif // GRID_H
