#ifndef NE_CAMERA_H
#define NE_CAMERA_H

#include "vec.h"
#include "ray.h"

namespace ne {

class Camera {
public:
    Camera(
        Vec3 pos,
        Vec3 lookat,
        Vec3 vup,
        float vfov,
        float aspect,
        float aperture,
        float focus_dist);

    inline Ray ray_from_view(float u, float v) const;

private:
    Vec3 position;
    Vec3 lower_left;
    Vec3 h_plane;
    Vec3 v_plane;
    Vec3 u, v, w;
    float lens_radius;
};

inline Ray Camera::ray_from_view(float s, float t) const {
    Vec3 r = lens_radius * Vec3::random_in_unit_circle();
    Vec3 offset = u*r.x + v*r.y;
    return Ray{
        position + offset,
        lower_left + s*h_plane + t*v_plane - position - offset,
    };
}

} // ne

#endif // NE_CAMERA_H
