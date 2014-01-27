#include "Mesh.h"

#include <limits>
#include "ngl/Util.h"

AABB::AABB():m_vmin(),m_vmax(),m_boundingRadius(0)
{ }

AABB::AABB(const ngl::Vec4 &_vmin, const ngl::Vec4 &_vmax):m_vmin(_vmin),m_vmax(_vmax),m_boundingRadius(0)
{
    assert(m_vmin.m_x <= m_vmax.m_x && m_vmin.m_y <= m_vmax.m_y && m_vmin.m_z <= m_vmax.m_z);
    calcBoundingRadius();
}

void AABB::reshape(const ngl::Vec4 &_vmin, const ngl::Vec4 &_vmax)
{
    assert(_vmin.m_x <= _vmax.m_x && _vmin.m_y <= _vmax.m_y && _vmin.m_z <= _vmax.m_z);
    m_vmin = _vmin;
    m_vmax = _vmax;
    calcBoundingRadius();
}

void AABB::calcBoundingRadius()
{
    m_dimetions = m_vmax - m_vmin;
    m_boundingRadius = std::max(m_dimetions.m_x, std::max(m_dimetions.m_y, m_dimetions.m_z)) / 2;
}

Mesh::Mesh():m_id(-1),m_vertices(),m_vindices(),m_normals(),m_nindices(),m_AABB()
{ }

Mesh::Mesh(unsigned _id,
           const std::vector<ngl::Vec4>& _v,
           const std::vector<unsigned>& _vi,
           const std::vector<ngl::Vec4>& _vn,
           const std::vector<unsigned>& _vni):
    m_id(_id),m_vertices(_v),m_vindices(_vi),m_normals(_vn),m_nindices(_vni),m_AABB()
{
    calcAABB();
}

//TODO: mode is ignored for now - FLAT assumed
void Mesh::calcNormals(Mesh::SHADING_MODE _mode)
{
    m_normals.resize(m_vindices.size());

    unsigned i  = 0;
    typedef std::vector<unsigned>::const_iterator ULIter;
    for (ULIter it = m_vindices.begin(); it != m_vindices.end(); it += 3)
    {
        m_normals[3*i] = ngl::calcNormal( m_vertices[ *(it) ], m_vertices[ *(it + 2) ], m_vertices[ *(it + 1) ] );
        m_normals[3*i + 1] = m_normals[3*i];
        m_normals[3*i + 2] = m_normals[3*i];
        ++i;
    }
    m_nindices.resize(m_vindices.size());
    m_nindices.insert(m_nindices.begin(), m_vindices.begin(), m_vindices.end());
}


void Mesh::calcAABB()
{
    std::vector<ngl::Vec4> boundaryVerts = std::vector<ngl::Vec4>(6);
    if (!m_vertices.size())
    {
        m_AABB.reshape(ngl::Vec4(), ngl::Vec4());
        return;
    }

    ngl::Vec4 vmin = (*m_vertices.begin());
    ngl::Vec4 vmax = (*m_vertices.begin());
    typedef std::vector<ngl::Vec4>::iterator VIter;
    for (VIter it = m_vertices.begin() + 1; it != m_vertices.end(); ++it)
    {
        ngl::Vec4 vert = (*it);
        vmin.m_x = std::min(vert.m_x, vmin.m_x);
        vmin.m_y = std::min(vert.m_y, vmin.m_y);
        vmin.m_z = std::min(vert.m_z, vmin.m_z);

        vmax.m_x = std::max(vert.m_x, vmax.m_x);
        vmax.m_y = std::max(vert.m_y, vmax.m_y);
        vmax.m_z = std::max(vert.m_z, vmax.m_z);
    }
    m_AABB.reshape(vmin, vmax);
}

//FIX: what if bbox is not in the center of local coordinate system
ngl::Real Mesh::getBoundingRadius() const
{
    return m_AABB.getBoundingRadius();
}

BoidMesh::BoidMesh(unsigned _id):Mesh(_id)
{
    ngl::Vec4 v[] = {
                         ngl::Vec4(0,   1, 1),
                         ngl::Vec4(0,   0,-1),
                         ngl::Vec4(-0.5,0, 1),

                         ngl::Vec4(0,   1, 1),
                         ngl::Vec4(0,   0,-1),
                         ngl::Vec4(0.5, 0, 1),

                         ngl::Vec4(0,   1, 1),
                         ngl::Vec4(0,   0, 1.5),
                         ngl::Vec4(-0.5,0, 1),

                         ngl::Vec4(0,   1, 1),
                         ngl::Vec4(0,   0, 1.5),
                         ngl::Vec4(0.5, 0, 1)
                    };
    unsigned vi[] = {0,1,2,5,4,3,8,7,6,9,10,11};
    m_vertices.insert(m_vertices.begin(), v, v + 12);
    m_vindices.insert(m_vindices.begin(), vi, vi + 12);
    calcNormals( FLAT );
    calcAABB();
}

ObstacleMesh::ObstacleMesh(unsigned _id):Mesh(_id)
{
    const unsigned divu = 10;
    const unsigned divv = 5;
    const ngl::Real radius = 1.0;

    m_vertices.push_back(ngl::Vec4(0,radius,0));
    for (unsigned i = 1; i < divv; i++)
    {
        ngl::Real y = std::cos( ((float)i / divv) * ngl::PI ) * radius;
        ngl::Real r = std::sin( ((float)i / divv) * ngl::PI ) * radius;

        for (unsigned j = 0; j < divu; j++)
        {
            ngl::Real x = std::cos( ((float)j / divu) * ngl::PI * 2) * r;
            ngl::Real z = std::sin( ((float)j / divu) * ngl::PI * 2) * r;
            m_vertices.push_back(ngl::Vec4(x,y,z));
        }
    }
    m_vertices.push_back(ngl::Vec4(0,-radius,0));

    for (unsigned j = 0; j < divu; j++)
    {
        unsigned idxp1 = 0;
        unsigned idxc1 = j % divu + 1;
        unsigned idxc2 = (j + 1) % divu + 1;

        m_vindices.push_back(idxp1);
        m_vindices.push_back(idxc2);
        m_vindices.push_back(idxc1);

        idxc1 = (divv - 2) * divu + j % divu + 1;
        idxc2 = (divv - 2) * divu + (j + 1) % divu + 1;
        idxp1 = (divv - 1) * divu + 1;

        m_vindices.push_back(idxc1);
        m_vindices.push_back(idxp1);
        m_vindices.push_back(idxc2);

    }

    for (unsigned i = 1; i < divv - 1; i++)
    {
        for (unsigned j = 0; j < divu; j++)
        {
            unsigned idxp1 = (i - 1) * divu + j % divu + 1;
            unsigned idxp2 = (i - 1) * divu + (j + 1) % divu + 1;
            unsigned idxc1 = i * divu + j % divu + 1;
            unsigned idxc2 = i * divu + (j + 1) % divu + 1;

            m_vindices.push_back(idxp1);
            m_vindices.push_back(idxp2);
            m_vindices.push_back(idxc1);


            m_vindices.push_back(idxc2);
            m_vindices.push_back(idxc1);
            m_vindices.push_back(idxp2);
        }
    }

    calcNormals( FLAT );
    calcAABB();
}

