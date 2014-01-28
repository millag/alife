#include "Grid.h"
#include "Utils.h"

Grid::Grid(ngl::Real _size, unsigned _divisions):
    m_size(_size), m_divisions(_divisions), m_divisionsSqr(_divisions * _divisions)
{
    assert(m_size > 0 && m_divisions > 0);
    m_cellSize = m_size / m_divisions;
    m_volume.reshape(ngl::Vec4(-m_size / 2, -m_size / 2, -m_size / 2), ngl::Vec4(m_size / 2, m_size / 2, m_size / 2));
}

void Grid::initialize()
{
    m_cells.resize(std::pow(m_divisions, 3));
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

    unsigned i,j,k;
    unsigned cellIdx = findCellIdx(_object->getPosition(), i, j, k);
    assert(cellIdx < m_cells.size());
    Cell& cell = m_cells[cellIdx];

    unsigned objectIdx = m_objects.size();
    m_objects.push_back(_object);
    assert(objectIdx < m_objects.size());
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
    assert(o_objects.size() == 0);
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
            assert( (*it2) < m_objects.size() );
            Boid* object = m_objects[ (*it2) ];
            if ((object->getPosition() - _pos).lengthSquared() < distSqr)
            {
                o_objects.push_back(object);
            }
        }
    }

//    quick check for correctness
//    assert(isSearchCorrect(o_objects, _pos, distSqr));
}

unsigned Grid::findCellIdx(const ngl::Vec4 &_pos, unsigned& o_i, unsigned& o_j, unsigned &o_k) const
{
    ngl::Vec4 v = _pos - m_volume.getBottomLeft();
    o_i = utils::clamp(std::floor(v.m_x / m_cellSize), (ngl::Real)0.0, (ngl::Real)(m_divisions - 1));
    o_j = utils::clamp(std::floor(v.m_y / m_cellSize), (ngl::Real)0.0, (ngl::Real)(m_divisions - 1));
    o_k = utils::clamp(std::floor(v.m_z / m_cellSize), (ngl::Real)0.0, (ngl::Real)(m_divisions - 1));
    return calcIdx(o_i, o_j, o_k);
}

unsigned Grid::calcIdx(unsigned _i, unsigned _j, unsigned _k) const
{
    return _k * m_divisionsSqr + _i * m_divisions + _j;
}

void Grid::findTestCells(const ngl::Vec4 &_pos, ngl::Real _dist, std::vector<unsigned>& o_testCells) const
{
    ngl::Vec4 v(_dist, _dist, _dist);
    ngl::Vec4 vmin = _pos - v;
    ngl::Vec4 vmax = _pos + v;

    unsigned iMin,jMin,kMin;
    findCellIdx(vmin, iMin, jMin, kMin);

    unsigned iMax,jMax,kMax;
    findCellIdx(vmax, iMax, jMax, kMax);

    for (unsigned k = kMin; k <= kMax; k++)
    {
        for (unsigned i = iMin; i <= iMax; i++)
        {
            for (unsigned j = jMin; j <= jMax; j++)
            {
                o_testCells.push_back( calcIdx(i, j, k) );
            }
        }
    }
//    cell containing _pos should be included at least
    assert(o_testCells.size() > 0);
}

// FIX: needs refactoring
// there is bug in here - if a boid is moved to a cell that is not checked yet, it will be checked twice
void Grid::update()
{
    unsigned i,j,k;
    unsigned idx = 0;
    typedef std::vector<Cell>::iterator CIter;
    for(CIter it  = m_cells.begin(); it != m_cells.end(); it++)
    {
        Cell& cell = (*it);

        typedef std::list<unsigned>::iterator IIter;
        IIter it2 = cell.m_objectIdxList.begin();
        while (it2 != cell.m_objectIdxList.end())
        {
            unsigned objectIdx = (*it2);
            assert(objectIdx < m_objects.size());
            assert(m_objects[objectIdx] != NULL);

            unsigned cellIdx = findCellIdx( m_objects[objectIdx]->getPosition(), i, j, k);
            assert(cellIdx < m_cells.size());
            if ( cellIdx == idx)
            {
                ++it2;
                continue;
            }

            it2 = cell.m_objectIdxList.erase(it2);
            m_cells[cellIdx].m_objectIdxList.push_back(objectIdx);
        }
        ++idx;
    }
}


bool Grid::isSearchCorrect(const std::vector<Boid*>& _objects, const ngl::Vec4& _pos,  ngl::Real _distSqr) const
{
    std::vector<Boid *> neighbours;
    typedef std::vector<Boid*>::const_iterator BIter;
    for (BIter it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        if (((*it)->getPosition() - _pos).lengthSquared() < _distSqr)
        {
            neighbours.push_back((*it));
        }
    }

    return (neighbours.size() == _objects.size());
}
