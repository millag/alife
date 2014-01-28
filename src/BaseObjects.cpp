#include "BaseObjects.h"
#include "ngl/Util.h"
#include "Utils.h"



RenderObject::RenderObject():m_mesh(NULL),m_shaderId(-1),m_boundingRadius(0)
{ }

RenderObject::RenderObject(const Mesh* _mesh, int _shaderId, const ngl::Mat4 &_transform):
    m_mesh(_mesh),m_transform(_transform),m_shaderId(_shaderId)
{
    calcBoundingRadius();
}

RenderObject::~RenderObject() { }

unsigned RenderObject::getMeshId() const
{
    assert(m_mesh != NULL);
    return m_mesh->getId();
}

const Mesh* RenderObject::getMesh() const
{
    return m_mesh;
}

void RenderObject::setMesh(const Mesh* _mesh)
{
    m_mesh = _mesh;
    calcBoundingRadius();
}

// FIX:: following calculations are wrong when applying shear to the object
void RenderObject::calcBoundingRadius()
{
    if (!m_mesh || !m_mesh->getBoundingRadius())
    {
        m_boundingRadius = 0;
        return;
    }

    ngl::Vec4 rx =  (utils::C_EX  * m_mesh->getBoundingRadius()) * m_transform;
    ngl::Vec4 ry =  (utils::C_EY  * m_mesh->getBoundingRadius()) * m_transform;
    ngl::Vec4 rz =  (utils::C_EZ  * m_mesh->getBoundingRadius()) * m_transform;

    m_boundingRadius = std::sqrt(std::max(rx.lengthSquared(), std::max(ry.lengthSquared(), rz.lengthSquared())));
}



MovingObject::MovingObject(const Mesh *_mesh, int _shaderId, const ngl::Mat4 &_transform):
    RenderObject(_mesh, _shaderId, _transform)
{
    m_headingDir = m_transform.getBackVector();
    m_headingDir.normalize();
    assert(m_headingDir.m_w == 0);
    m_position = ngl::Vec4() * m_transform;

    m_maxSpeedSqr = m_maxSpeed = 0;
    m_maxTurningAngle = 0;
    m_mass = 1.0;
}


const ngl::Vec4 &MovingObject::getHeadingDir() const
{
    return m_headingDir;
}

const ngl::Vec4 &MovingObject::getPosition() const
{
    return m_position;
}

void MovingObject::setVelocity(const ngl::Vec4 &_v)
{
    assert(_v.m_w == 0);
    m_velocity = _v;
    utils::truncate(m_velocity, m_maxSpeed);
}

void MovingObject::update(ngl::Real _deltaT)
{
    if (m_acceleration.lengthSquared() < utils::C_ERR && m_velocity.lengthSquared() < utils::C_ERR)
    {
        return;
    }
    m_velocity += m_acceleration * _deltaT;
    utils::truncate(m_velocity, m_maxSpeed);
    m_position += m_velocity * _deltaT;
    updateHeading();
}

//FIX: this need serious rethinking to get banking right
void MovingObject::updateHeading()
{
    if ( m_velocity.lengthSquared() > utils::C_ERR)
    {
        m_headingDir = m_velocity;
        m_headingDir.normalize();
    }

    ngl::Vec4 yaxis = ngl::Vec4(m_transform.getUpVector());
    yaxis.m_w = 0;
    assert(yaxis.lengthSquared() > utils::C_ERR);
    ngl::Vec4 zaxis = m_headingDir;
    assert(zaxis.lengthSquared() > utils::C_ERR);
    ngl::Vec4 xaxis = yaxis.cross(zaxis);
    assert(xaxis.lengthSquared() > utils::C_ERR);
    xaxis.normalize();
    yaxis =  zaxis.cross(xaxis);
    assert(yaxis.lengthSquared() > utils::C_ERR);
    yaxis.normalize();


    ngl::Mat4 m(xaxis.m_x, xaxis.m_y, xaxis.m_z, 0,
                yaxis.m_x, yaxis.m_y, yaxis.m_z, 0,
                zaxis.m_x, zaxis.m_y, zaxis.m_z, 0,
                m_position.m_x, m_position.m_y, m_position.m_z, 1);
    m_transform = m;

}



//    ngl::Vec4 localHeading =  m_headingDir;
//    localHeading.normalize();
//    ngl::Vec4 projectedHeading(localHeading.m_x, 0, localHeading.m_z, 0);
//    projectedHeading.normalize();



//    ngl::Real phi = ngl::degrees(std::acos(localHeading.dot(projectedHeading))) * utils::getSign(localHeading.m_y);
//    ngl::Real theta = ngl::degrees(std::acos(projectedHeading.dot((utils::C_EZ)))) * utils::getSign(localHeading.m_x);

//    m_transform.addRotation(phi, theta, 0);
