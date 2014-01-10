#ifndef MESH_H
#define MESH_H

#include <vector>
#include "ngl/Vec4.h"

class AABB
{
public:
    AABB();
    AABB(const std::vector<ngl::Vec4>& _boundaryVerts);

    ngl::Real getBoundingRadius() const { return m_boundingRadius; }
    ngl::Vec4 getBottomLeft() const { return m_vmin; }
    ngl::Vec4 getTopRight() const { return m_vmax; }
    ngl::Vec4 getCenter() const { return (m_vmin + m_vmax) / 2; }

    const std::vector<ngl::Vec4>& getBoundaryVertices() const { return m_boundaryVerts; }
    void setBoundaryVertices(const std::vector<ngl::Vec4>& _boundaryVerts);

protected:
    std::vector<ngl::Vec4> m_boundaryVerts;
    ngl::Vec4 m_vmin;
    ngl::Vec4 m_vmax;
    ngl::Real m_boundingRadius;

    bool areBoundaryVerts(const std::vector<ngl::Vec4>& _boundaryVerts);
    void calcBoundingBox();
};

class Mesh
{
public:
    enum SHADING_MODE { FLAT, GOURAUD };

    Mesh(unsigned _id,
         const std::vector<ngl::Vec4>& _v = std::vector<ngl::Vec4>(),
         const std::vector<unsigned>& _vi = std::vector<unsigned>(),
         const std::vector<ngl::Vec4>& _vn = std::vector<ngl::Vec4>(),
         const std::vector<unsigned>& _vni = std::vector<unsigned>() );

    virtual ~Mesh() { }

    unsigned getId() const { return m_id; }
    unsigned getNFaces() const { return (m_vindices.size() / 3); }
    unsigned getNVertices() const { return (m_vertices.size()); }

    const std::vector<ngl::Vec4>& getVertices() const { return m_vertices; }
    const std::vector<unsigned>& getVIndices() const { return m_vindices; }
    const std::vector<ngl::Vec4>& getNormals() const { return m_normals; }
    const std::vector<unsigned>& getNIndices() const { return m_nindices; }
    const AABB& getAABB() const { return m_AABB; }
    ngl::Real getBoundingRadius() const { return m_AABB.getBoundingRadius(); }

    void calcNormals(Mesh::SHADING_MODE _mode);

protected:
    const unsigned m_id;
    std::vector<ngl::Vec4> m_vertices;
    std::vector<unsigned> m_vindices;
    std::vector<ngl::Vec4> m_normals;
    std::vector<unsigned> m_nindices;
    AABB m_AABB;

    void calcAABB();
private:
    Mesh();
};

class BoidMesh : public Mesh
{
public:
    BoidMesh(unsigned _id);
    ~BoidMesh() { }
};

class ObstacleMesh : public Mesh
{
public:
    ObstacleMesh(unsigned _id);
    ~ObstacleMesh() { }
};

#endif // MESH_H
