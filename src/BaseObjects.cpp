#include "geometry.h"
#include <ngl/Util.h>

unsigned Mesh::getNFaces() const
{
    return (m_vindices.size() / 3);
}

unsigned Mesh::getNVertices() const
{
    return (m_vertices.size());
}

//TODO: mode is ignored for now - FLAT assumed
//QUESTION: check if normal index array should be the same as the vertex index array, probably this is NGL specific WTF??
void Mesh::computeNormals(Mesh::SHADING_MODE _mode)
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

SceneObject::SceneObject(int _meshId, const ngl::Transformation& _transform):
    m_meshId(_meshId),m_transform(_transform)
{ }

SceneObject::SceneObject(const SceneObject& _so):
    m_meshId(_so.m_meshId),m_transform(_so.m_transform)
{ }

SceneObject::~SceneObject() { }

int SceneObject::getMeshId() const
{
    return m_meshId;
}

void SceneObject::setMeshId(int _meshId)
{
    m_meshId = _meshId;
}

const ngl::Vec4& SceneObject::getPosition() const
{
    return m_transform.getPosition();
}

void SceneObject::setPosition(const ngl::Vec4 &_p)
{
    m_transform.setPosition(_p);
}

const ngl::Transformation& SceneObject::getTransform() const
{
    return m_transform;
}

void SceneObject::lookAt(const ngl::Vec4 &_at, const ngl::Vec4 &_up)
{
    const ngl::Vec4& t = m_transform.getPosition();
    ngl::Vec4 zaxis = _at - t;
    zaxis.normalize();
    ngl::Vec4 xaxis = _up.cross(zaxis);
    xaxis.normalize();
    ngl::Vec4 yaxis = zaxis.cross(xaxis);
    yaxis.normalize();
    zaxis *= -1.0;
    ngl::Mat4 m(xaxis.m_x, xaxis.m_y, xaxis.m_z, 0,
                yaxis.m_x, yaxis.m_y, yaxis.m_z, 0,
                zaxis.m_x, zaxis.m_y, zaxis.m_z, 0,
                    t.m_x,     t.m_y,     t.m_z, 1);
    m_transform.setMatrix(m);
}

BoidMesh::BoidMesh():Mesh()
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
    computeNormals( FLAT );
}

ObstacleMesh::ObstacleMesh():Mesh()
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
    computeNormals( FLAT );
}
