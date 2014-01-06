#include "utils.h"
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
    if (v.lengthSquared() == 0)
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
