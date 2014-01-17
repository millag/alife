#include "Utils.h"
#include <cstdlib>
#include "ngl/Util.h"

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

// generate with uniform distribution - thank you http://mathworld.wolfram.com
ngl::Vec4 utils::genRandPointOnSphere(ngl::Real _radius, const ngl::Vec4& _center)
{
    ngl::Real u = randf(-1, 1);
    ngl::Real theta = randf(0, ngl::PI * 2);

    ngl::Real x = std::sqrt(1 - u * u) * std::cos(theta);
    ngl::Real y = std::sqrt(1 - u * u) * std::sin(theta);
    ngl::Real z = u * _radius;
    return ngl::Vec4(x, y, z) + _center;
}

ngl::Real utils::getSign(ngl::Real _value)
{
    return (_value < 0)? -1 : 1;
}

void utils::truncate(ngl::Vec4& io_v, ngl::Real _maxLength)
{
    ngl::Real maxLengthSqr = _maxLength * _maxLength;
    if (maxLengthSqr > C_ERR  && io_v.lengthSquared() > maxLengthSqr)
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
