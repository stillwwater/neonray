#include "camera.h"
#include "math.h"

#include <cmath>

namespace ne {

Camera::Camera(Vec3 pos,
               Vec3 lookat,
               Vec3 vup,
               float vfov,
               float aspect,
               float aperture,
               float focus_dist)
{
    float theta = radians(vfov);
    float h = tanf(theta/2.0f);
    float viewport_height = 2.0f * h;
    float viewport_width = viewport_height * aspect;

    w = (pos - lookat).normalized();
    u = Vec3::cross(vup, w).normalized();
    v = Vec3::cross(w, u);

    h_plane = u * (viewport_width * focus_dist);
    v_plane = v * (viewport_height * focus_dist);
    lower_left = pos - h_plane*0.5f - v_plane*0.5f - focus_dist*w;
    position = pos;

    lens_radius = aperture / 2.0f;
}

} // ne
