#include "Mesh.h"

#include <limits>
#include "ngl/Util.h"

AABB::AABB():m_boundaryVerts(6),m_boundingRadius(0)
{ }

AABB::AABB(const std::vector<ngl::Vec4>& _boundaryVerts)
{
    setBoundaryVertices(_boundaryVerts);
}

void AABB::setBoundaryVertices(const std::vector<ngl::Vec4> &_boundaryVerts)
{
    assert(areBoundaryVerts(_boundaryVerts));
    m_boundaryVerts = _boundaryVerts;
    calcBoundingBox();
}


void AABB::calcBoundingBox()
{
    m_vmin.set(m_boundaryVerts[0].m_x, m_boundaryVerts[1].m_y, m_boundaryVerts[2].m_z);
    m_vmax.set(m_boundaryVerts[3].m_x, m_boundaryVerts[4].m_y, m_boundaryVerts[5].m_z);
    ngl::Real w = m_vmax.m_x - m_vmin.m_x;
    ngl::Real h = m_vmax.m_y - m_vmin.m_y;
    ngl::Real d = m_vmax.m_z - m_vmin.m_z;
    m_boundingRadius = std::max(w, std::max(h, d)) / 2;
}


bool AABB::areBoundaryVerts(const std::vector<ngl::Vec4>& _boundaryVerts)
{
    if (_boundaryVerts.size() != 6)
    {
        return false;
    }
    typedef std::vector<ngl::Vec4>::const_iterator VIter;
    ngl::Vec4 v = *(_boundaryVerts.begin());
    for ( VIter it = _boundaryVerts.begin(); it != _boundaryVerts.end(); ++it)
    {
        if (v.m_x > (*it).m_x )
            return false;
    }
    v = *(_boundaryVerts.begin() + 1);
    for ( VIter it = _boundaryVerts.begin(); it != _boundaryVerts.end(); ++it)
    {
        if (v.m_y > (*it).m_y )
            return false;
    }
    v = *(_boundaryVerts.begin() + 2);
    for ( VIter it = _boundaryVerts.begin(); it != _boundaryVerts.end(); ++it)
    {
        if (v.m_z > (*it).m_z )
            return false;
    }
    v = *(_boundaryVerts.begin() + 3);
    for ( VIter it = _boundaryVerts.begin(); it != _boundaryVerts.end(); ++it)
    {
        if (v.m_x < (*it).m_x )
            return false;
    }
    v = *(_boundaryVerts.begin() + 4);
    for ( VIter it = _boundaryVerts.begin(); it != _boundaryVerts.end(); ++it)
    {
        if (v.m_y < (*it).m_y )
            return false;
    }
    v = *(_boundaryVerts.begin() + 5);
    for ( VIter it = _boundaryVerts.begin(); it != _boundaryVerts.end(); ++it)
    {
        if (v.m_z < (*it).m_z )
            return false;
    }
    return true;
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
        m_AABB.setBoundaryVertices(boundaryVerts);
        return;
    }

    typedef std::vector<ngl::Vec4>::iterator VIter;
    for (VIter it = boundaryVerts.begin() + 1; it != boundaryVerts.end(); ++it)
    {
        (*it) = (*m_vertices.begin());
    }

    for (VIter it = m_vertices.begin() + 1; it != m_vertices.end(); ++it)
    {
        ngl::Vec4 vert = (*it);
        if (boundaryVerts[0].m_x > vert.m_x)
        {
            boundaryVerts[0] = vert;
        }
        if (boundaryVerts[1].m_y > vert.m_y)
        {
            boundaryVerts[1] = vert;
        }
        if (boundaryVerts[2].m_z > vert.m_z)
        {
            boundaryVerts[2] = vert;
        }
        if (boundaryVerts[3].m_x < vert.m_x)
        {
            boundaryVerts[3] = vert;
        }
        if (boundaryVerts[4].m_y < vert.m_y)
        {
            boundaryVerts[4] = vert;
        }
        if (boundaryVerts[5].m_z < vert.m_z)
        {
            boundaryVerts[5] = vert;
        }
    }
    m_AABB.setBoundaryVertices(boundaryVerts);
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

