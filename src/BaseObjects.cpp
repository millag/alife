#include "BaseObjects.h"
#include "ngl/Util.h"
#include "Utils.h"



RenderObject::RenderObject():m_mesh(NULL),m_shaderId(-1),m_boundingRadius(0)
{ }

RenderObject::RenderObject(const Mesh* _mesh, int _shaderId, const ngl::Transformation& _transform):
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

void RenderObject::setTransform(const ngl::Transformation& _transform)
{
    m_transform = _transform;
    calcBoundingRadius();
}

//void RenderObject::lookAt(const ngl::Vec4 &_at, const ngl::Vec4 &_up)
//{
//    const ngl::Vec4& t = m_transform.getPosition();
//    ngl::Vec4 zaxis = _at - t;
//    zaxis.normalize();
//    ngl::Vec4 xaxis = _up.cross(zaxis);
//    xaxis.normalize();
//    ngl::Vec4 yaxis = zaxis.cross(xaxis);
//    yaxis.normalize();
//    zaxis *= -1.0;
//    ngl::Mat4 m(xaxis.m_x, xaxis.m_y, xaxis.m_z, 0,
//                yaxis.m_x, yaxis.m_y, yaxis.m_z, 0,
//                zaxis.m_x, zaxis.m_y, zaxis.m_z, 0,
//                    t.m_x,     t.m_y,     t.m_z, 1);
//    m_transform.setMatrix(m);
//}


// FIX:: following calculations are wrong when applying shear to the object
void RenderObject::calcBoundingRadius()
{
    if (!m_mesh || !m_mesh->getBoundingRadius())
    {
        m_boundingRadius = 0;
        return;
    }

    const ngl::Mat4& transform = m_transform.getMatrix();
    ngl::Vec4 rx =  transform * (utils::c_ex  * m_mesh->getBoundingRadius());
    ngl::Vec4 ry =  transform * (utils::c_ey  * m_mesh->getBoundingRadius());
    ngl::Vec4 rz =  transform * (utils::c_ez  * m_mesh->getBoundingRadius());

    m_boundingRadius = std::sqrt(std::max(rx.lengthSquared(), std::max(ry.lengthSquared(), rz.lengthSquared())));
}



MovingObject::MovingObject(const Mesh *_mesh, int _shaderId, const ngl::Transformation &_transform):
    RenderObject(_mesh, _shaderId, _transform)
{
    m_headingDir = m_transform.getMatrix().getForwardVector();
    m_headingDir.normalize();

    m_maxSpeedSqr = m_maxSpeed = 0;
    m_maxTurningAngle = 0;
    m_mass = 1.0;
}


ngl::Vec4 MovingObject::getHeadingDir() const
{
    return m_headingDir;
}

void MovingObject::setVelocity(const ngl::Vec4 &_v)
{
    m_velocity = _v;
    utils::truncate(m_velocity, m_maxSpeed);
}

void MovingObject::update(ngl::Real _deltaT)
{
    if (m_acceleration.lengthSquared() < utils::c_err && m_velocity.lengthSquared() < utils::c_err)
    {
        return;
    }
    m_velocity += m_acceleration * _deltaT;
    utils::truncate(m_velocity, m_maxSpeed);
//    m_transform.setPosition( m_transform.getPosition() + m_velocity * _deltaT);
    m_transform.addPosition( m_velocity * _deltaT);
    updateHeading();
}

void MovingObject::updateHeading()
{
    if ( m_velocity.lengthSquared() < utils::c_err)
    {
        return;
    }

    m_headingDir = m_velocity;
    m_headingDir.normalize();

//    TODO set new orientation
//    TODO: update parent cell in acc grid
}
