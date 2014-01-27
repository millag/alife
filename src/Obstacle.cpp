#include "Obstacle.h"
#include <limits>

ngl::Real raySphereIntersect(const ngl::Vec4& _rPos, const ngl::Vec4& _rDir, const ngl::Vec4& _sPos, ngl::Real _sRadius, ngl::Vec4& o_ip, ngl::Vec4& o_in);

ngl::Real SphericalObstacle::findPossibleCollisionPoint(const MovingObject* _object, ngl::Vec4& o_iPoint, ngl::Vec4& o_iNormal) const
{
    ngl::Real distSqr = std::numeric_limits<ngl::Real>::max();

    ngl::Vec4 rPos =_object->getPosition();
    ngl::Vec4 rDir = _object->getHeadingDir();
    ngl::Vec4 sPos = this->getPosition();
    ngl::Real sInflatedRadius = this->getBoundingRadius() + _object->getBoundingRadius();
    distSqr = raySphereIntersect(rPos, rDir, sPos, sInflatedRadius, o_iPoint, o_iNormal);

    return distSqr;
}

ngl::Real raySphereIntersect(const ngl::Vec4& _rPos, const ngl::Vec4& _rDir, const ngl::Vec4& _sPos, ngl::Real _sRadius, ngl::Vec4& o_ip, ngl::Vec4& o_in)
{
//    TODO : implement
    return std::numeric_limits<ngl::Real>::max();
}
