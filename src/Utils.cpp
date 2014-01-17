#include "Utils.h"
#include <cstdlib>

template<class T>
T utils::clamp(T v, T a, T b)
{
    return std::max(a, std::min(v, b));
}

ngl::Real utils::randf(ngl::Real _min, ngl::Real _max) {
    return _min + (_max - _min) * ((ngl::Real)std::rand() / RAND_MAX);
}

ngl::Vec4 utils::genRandPointInBox( ngl::Real _bBoxMin, ngl::Real _bBoxMax )
{
    return ngl::Vec4(randf(_bBoxMin, _bBoxMax), randf(_bBoxMin, _bBoxMax), randf(_bBoxMin, _bBoxMax));
}

ngl::Vec4 utils::genRandPointInSphere(ngl::Real _radius, const ngl::Vec4& _center)
{
//    TODO generate with uniform distribution
    ngl::Vec4 v = genRandPointInBox();
    if (v.lengthSquared() < C_ERR)
        return _center;

    v.normalize();
    v *= _radius;
    v += _center;
    return v;
}

ngl::Real utils::getSign(ngl::Real _value)
{
    return (_value < 0)? -1 : 1;
}

void utils::truncate(ngl::Vec4& io_v, ngl::Real _maxLength)
{
    if (io_v.lengthSquared() > _maxLength * _maxLength)
    {
        io_v.normalize();
        io_v *= _maxLength;
    }
}

bool utils::isInsideVolume(const ngl::Vec4& _p, const AABB& _volume)
{
    ngl::Vec4 v = _p - _volume.getBottomLeft();
    if ( v.m_x < 0 || v.m_y < 0 || v.m_z < 0)
    {
        return false;
    }
    v = _volume.getTopRight() - _p;
    if ( v.m_x < 0 || v.m_y < 0 || v.m_z < 0)
    {
        return false;
    }
    return true;
}
