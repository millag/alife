#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "BaseObjects.h"
#include "Utils.h"

class Obstacle : public MovingObject
{
public:
    Obstacle(const Mesh* _mesh, int _shaderId, const ngl::Mat4& _transform = ngl::Mat4()):
        MovingObject(_mesh, _shaderId, _transform)
    { }
    virtual ~Obstacle() { }

    virtual ngl::Real findPossibleCollisionPoint(const MovingObject* _object, ngl::Vec4& o_iPoint, ngl::Vec4& o_iNormal) const = 0;
};

class SphericalObstacle : public Obstacle
{
public:
    SphericalObstacle(const Mesh* _mesh, int _shaderId, const ngl::Mat4& _transform = ngl::Mat4()):
        Obstacle(_mesh, _shaderId, _transform)
    { }
    ~SphericalObstacle() { }

    ngl::Real findPossibleCollisionPoint(const MovingObject* _object, ngl::Vec4& o_iPoint, ngl::Vec4& o_iNormal) const;
};

#endif // OBSTACLE_H
