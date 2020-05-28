#ifndef NE_RAY_H
#define NE_RAY_H

#include "vec.h"

namespace ne
{

struct Ray
{
    Vec3 origin;
    Vec3 direction;

    Ray() {}
    Ray(Vec3 origin, Vec3 direction) : origin(origin), direction(direction) {}

    inline Vec3 at(float dist) const;
};

inline Vec3 Ray::at(float dist) const {
    return origin + direction*dist;
}

} // ne

#endif // NE_RAY_H
