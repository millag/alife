#ifndef UTILS_H
#define UTILS_H

#include "ngl/Vec4.h"
#include "Mesh.h"

namespace utils {

const static unsigned short c_sec = 1000;
const static unsigned short c_fps = 30;
const static unsigned short c_updateRate = c_sec / 30;

const static ngl::Real c_err = 0.000001;
const static ngl::Vec4 c_ex(1,0,0,0);
const static ngl::Vec4 c_ey(0,1,0,0);
const static ngl::Vec4 c_ez(0,0,1,0);
const static ngl::Vec4 c_ew(0,0,0,1);

template<class T>
T clamp(T v, T a, T b);

ngl::Real getSign(ngl::Real _value);
ngl::Real randf(ngl::Real _min = 0.0, ngl::Real _max = 1.0);
ngl::Vec4 genRandPointInBox(ngl::Real _bBoxMin = -1.0, ngl::Real _bBoxMax = 1.0);
ngl::Vec4 genRandPointInSphere(ngl::Real _radius = 1.0, const ngl::Vec4& _center = ngl::Vec4());
void truncate(ngl::Vec4& io_v, ngl::Real _maxLength);

bool isInsideVolume(const ngl::Vec4& _p, const AABB& _volume);
}

#endif // UTILS_H
