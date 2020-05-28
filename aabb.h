#ifndef NE_AABB_H
#define NE_AABB_H

#include "vec.h"
#include "ray.h"

#include <cmath>
#include <algorithm>

namespace ne {

struct Aabb {
    Vec3 min_bounds;
    Vec3 max_bounds;

    Aabb() {}

    Aabb(const Vec3 &min, const Vec3 &max)
        : min_bounds(min), max_bounds(max) {}

    static inline Aabb enclose(const Aabb &box0, const Aabb &box1);

    inline bool intersect(const Ray &ray, float tmin, float tmax) const;
};

inline Aabb Aabb::enclose(const Aabb &box0, const Aabb &box1) {
    Vec3 min_bounds(fminf(box0.min_bounds.x, box1.min_bounds.x),
                    fminf(box0.min_bounds.y, box1.min_bounds.y),
                    fminf(box0.min_bounds.z, box1.min_bounds.z));

    Vec3 max_bounds(fmaxf(box0.max_bounds.x, box1.max_bounds.x),
                    fmaxf(box0.max_bounds.y, box1.max_bounds.y),
                    fmaxf(box0.max_bounds.z, box1.max_bounds.z));

    return Aabb(min_bounds, max_bounds);
}

inline bool Aabb::intersect(const Ray &ray, float tmin, float tmax) const {
    for (int a = 0; a < 3; ++a) {
        float inv_d = 1.0f / ray.direction[a];
        float t0 = (min_bounds[a] - ray.origin[a]) * inv_d;
        float t1 = (max_bounds[a] - ray.origin[a]) * inv_d;
        if (inv_d < 0) {
            std::swap(t0, t1);
        }
        tmin = fmaxf(t0, tmin);
        tmax = fminf(t1, tmax);
        if (tmax <= tmin) {
            return false;
        }
    }
    return true;
}

} // ne

#endif // NE_AABB_H
