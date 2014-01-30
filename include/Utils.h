#ifndef UTILS_H
#define UTILS_H

#include "ngl/Vec4.h"
#include "Mesh.h"

namespace utils
{

    template<class T>
    T clamp(T v, T a, T b)
    {
        assert(a < b);
        return std::max(a, std::min(v, b));
    }


    const static unsigned short C_SEC = 1000;
    const static unsigned short C_UPS = 30;
    const static unsigned short C_FPS = 30;
    const static unsigned short C_UPDATERATE = C_SEC / C_UPS;
    const static unsigned short C_REFRESHRATE = C_SEC / C_FPS;

    const static ngl::Real C_ERR = 0.000001;
    const static ngl::Vec4 C_EX(1,0,0,0);
    const static ngl::Vec4 C_EY(0,1,0,0);
    const static ngl::Vec4 C_EZ(0,0,1,0);
    const static ngl::Vec4 C_EW(0,0,0,1);


    ngl::Real getSign(ngl::Real _value);
    ngl::Real randf(ngl::Real _min = 0.0, ngl::Real _max = 1.0);
    ngl::Vec4 genRandPointInBox(ngl::Real _bBoxMin = -1.0, ngl::Real _bBoxMax = 1.0);
    ngl::Vec4 genRandPointOnSphere(ngl::Real _radius = 1.0, const ngl::Vec4& _center = ngl::Vec4());
    ngl::Vec4 genRandPointOnDisk(ngl::Real _radius = 1.0, const ngl::Vec4& _center = ngl::Vec4());

    void truncate(ngl::Vec4& io_v, ngl::Real _maxLength);
    ngl::Vec4 faceforward(const ngl::Vec4& _n, const ngl::Vec4& _v);
    ngl::Vec4 reflect(const ngl::Vec4& _v, const ngl::Vec4& _n);

    bool isInsideVolume(const ngl::Vec4& _p, const AABB& _volume);
}

#endif // UTILS_H
