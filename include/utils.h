#ifndef UTILS_H
#define UTILS_H

#include <ngl/Vec4.h>

namespace utils {

const static unsigned short SEC = 1000;
const static unsigned short FPS = 30;
const static unsigned short UPDATE_RATE = SEC / 30;
const static ngl::Vec4 ex(1,0,0,0);
const static ngl::Vec4 ey(0,1,0,0);
const static ngl::Vec4 ez(0,0,1,0);
const static ngl::Vec4 ew(0,0,0,1);

template<class T>
T clamp(T v, T a, T b);

ngl::Real getSign(ngl::Real _value);
ngl::Real randf(ngl::Real _min = 0.0, ngl::Real _max = 1.0);
ngl::Vec4 genRandPointInBox(ngl::Real _bBoxMin = -1.0, ngl::Real _bBoxMax = 1.0);
ngl::Vec4 genRandPointInSphere(ngl::Real _radius = 1.0, const ngl::Vec4& _center = ngl::Vec4());

}

#endif // UTILS_H
