
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <ngl/Transformation.h>

class Mesh
{
public:
    enum SHADING_MODE { FLAT, GOURAUD };

    Mesh():m_vertices(),m_vindices(),m_normals(),m_nindices() { }
    virtual ~Mesh() { }

    unsigned getNFaces() const;
    unsigned getNVertices() const;
    void computeNormals(Mesh::SHADING_MODE _mode);

    std::vector<ngl::Vec4> m_vertices;
    std::vector<unsigned> m_vindices;
    std::vector<ngl::Vec4> m_normals;
    std::vector<unsigned> m_nindices;
};

class SceneObject
{
public:
    SceneObject(int _meshId = -1, const ngl::Transformation& _transform = ngl::Transformation());
    SceneObject(const SceneObject& _so);

    virtual ~SceneObject();

    const ngl::Transformation& getTransform() const;
    const ngl::Vec4& getPosition() const;
    void setPosition(const ngl::Vec4 &_p);
    void lookAt(const ngl::Vec4& _at, const ngl::Vec4& _up);

    int getMeshId() const;
    void setMeshId( int _meshId);

private:
    int m_meshId;
protected:
    ngl::Transformation m_transform;
};

class BoidMesh : public Mesh
{
public:
    BoidMesh();
    ~BoidMesh() { }
};

class ObstacleMesh : public Mesh
{
public:
    ObstacleMesh();
    ~ObstacleMesh() { }
};

#endif // GEOMETRY_H
