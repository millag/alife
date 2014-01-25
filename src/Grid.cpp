#include "Grid.h"

Grid::Grid(ngl::Real _size, unsigned _divisions):
    m_size(_size), m_divisions(_divisions), m_divisionsSqr(_divisions * _divisions)
{
    assert(m_size > 0);
    m_volume.reshape(ngl::Vec4(-m_size / 2, -m_size / 2, -m_size / 2), ngl::Vec4(m_size / 2, m_size / 2, m_size / 2));
}

void Grid::initialize()
{
    m_cells.resize(1 + std::pow(m_divisions, 3));
    m_objects.reserve(1000);
}

bool Grid::isObjectInGrid(const Boid *_object) const
{
    assert(_object != NULL);
    typedef std::vector<Boid*>::const_iterator BIter;
    for( BIter it  = m_objects.begin(); it != m_objects.end(); it++)
    {
        if ((*it) == _object)
        {
            return true;
        }
    }
    return false;
}

void Grid::addObject(Boid *_object)
{
    assert(_object != NULL);
    if (isObjectInGrid(_object))
    {
        return;
    }

    int i,j,k;
    unsigned cellIdx = findCellIdx(_object->getPosition(), i, j, k);
    Cell& cell = m_cells[cellIdx];

    unsigned objectIdx = m_objects.size();
    m_objects.push_back(_object);
    cell.m_objectIdxList.push_back(objectIdx);
}

void Grid::addObjects(const std::vector<Boid *> _objects)
{
    typedef std::vector<Boid*>::const_iterator BIter;
    for( BIter it  = _objects.begin(); it != _objects.end(); ++it)
    {
        addObject((*it));
    }
}


void Grid::findObjectsWithinDistance(const ngl::Vec4& _pos, ngl::Real _dist, std::vector<Boid*>& o_objects) const
{
    ngl::Real distSqr = _dist * _dist;

    std::vector<unsigned> testCells;
    testCells.reserve(m_cells.size());
    findTestCells(_pos, _dist, testCells);
    assert( testCells.size() <= m_cells.size() );

    typedef std::vector<unsigned>::iterator CIter;
    for (CIter it = testCells.begin(); it != testCells.end(); ++it)
    {
        assert( (*it) < m_cells.size() );
        const Cell& cell = m_cells[ (*it) ];
        typedef std::list<unsigned>::const_iterator IIter;
        for (IIter it2 = cell.m_objectIdxList.begin(); it2 != cell.m_objectIdxList.end(); ++it2)
        {
            Boid* object = m_objects[ (*it2) ];
            if ((object->getPosition() - _pos).lengthSquared() < distSqr)
            {
                o_objects.push_back(object);
            }
        }
    }
}

unsigned Grid::findCellIdx(const ngl::Vec4 &_pos, int& o_i, int& o_j, int &o_k) const
{
    if (!utils::isInsideVolume(_pos, m_volume))
    {
//      TODO: handle objects outside grid effectively - currently all stored in first cell
        o_i = o_j = o_k = -1;
        return 0;
    }

    ngl::Vec4 v = _pos - m_volume.getBottomLeft();
    o_i = std::floor( v.m_x / m_divisions );
    o_j = std::floor( v.m_y / m_divisions );
    o_k = std::floor( v.m_z / m_divisions );
    return calcIdx(o_i, o_j, o_k);
}

unsigned Grid::calcIdx(int _i, int _j, int _k) const
{
    if (_i < 0 || _j < 0 || _k < 0)
    {
//      TODO: handle objects outside grid effectively - currently all stored in first cell
        return 0;
    }
    return _k * m_divisionsSqr + _i * m_divisions + _j + 1;
}

void Grid::findTestCells(const ngl::Vec4 &_pos, ngl::Real _dist, std::vector<unsigned>& o_testCells) const
{
    ngl::Vec4 v(_dist, _dist, _dist);
    AABB searchVolume(_pos - v, _pos + v);

    int iMin,jMin,kMin;
    unsigned cellMinIdx = findCellIdx(searchVolume.getBottomLeft(), iMin, jMin, kMin);
    if (cellMinIdx == 0)
    {
        iMin = jMin = kMin = 0;
        o_testCells.push_back(0);
    }

    int iMax,jMax,kMax;
    unsigned cellMaxIdx = findCellIdx(searchVolume.getTopRight(), iMax, jMax, kMax);
    if (cellMaxIdx == 0)
    {
        iMax = jMax = kMax = m_divisions - 1;
        if (cellMinIdx != 0)
        {
            o_testCells.push_back(0);
        }
    }

    for (int k = kMin; k <= kMax; k++)
    {
        for (int i = iMin; i <= iMax; i++)
        {
            for (int j = jMin; j <= jMax; j++)
            {
                o_testCells.push_back( calcIdx(i, j, k) );
            }
        }
    }
}

void Grid::update()
{
    int i,j,k;
    unsigned idx = 0;
    typedef std::vector<Cell>::iterator CIter;
    for(CIter it  = m_cells.begin(); it != m_cells.end(); it++)
    {
        Cell& cell = (*it);
        typedef std::list<unsigned>::iterator IIter;
        for (IIter it2 = cell.m_objectIdxList.begin(); it2 != cell.m_objectIdxList.end(); it2++)
        {
            unsigned objectIdx = (*it2);
            assert(objectIdx < m_objects.size());
            assert(m_objects[objectIdx] != NULL);

            unsigned cellIdx = findCellIdx( m_objects[objectIdx]->getPosition(), i, j, k);
            assert(cellIdx < m_cells.size());
            if ( cellIdx == idx)
            {
                continue;
            }

            cell.m_objectIdxList.erase(it2);
            m_cells[cellIdx].m_objectIdxList.push_back(objectIdx);
        }

        ++idx;
    }
}
