#include <cmath>

#include "entity.h"
#include "vec.h"
#include "ray.h"

namespace ne {

bool Sphere::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    Vec3 oc = ray.origin - position;
    float a = ray.direction.length_sqr();
    float half_b = Vec3::dot(oc, ray.direction);
    float c = oc.length_sqr() - radius*radius;
    float discriminant = half_b*half_b - a*c;

    if (discriminant <= 0) {
        return false;
    }

    float root = sqrtf(discriminant);
    float dist = (-half_b - root) / a;

    if (dist >= range.max || dist <= range.min) {
        dist = (-half_b + root) / a;
        if (dist >= range.max || dist <= range.min) {
            // Hit is outside range
            return false;
        }
    }

    hit.dist = dist;
    hit.position = ray.at(hit.dist);
    hit.normal = ((hit.position - position) / radius).normalized();
    hit.material = material;

    if (Vec3::dot(ray.direction, hit.normal) >= 0) {
        // Invert normals if they are inside the entity
        hit.normal = -hit.normal;
        hit.face = Hit::Back_Face;
    } else {
        hit.face = Hit::Front_Face;
    }

    return true;
}

bool World::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    Hit current_hit;
    bool any_hit = false;

    for (const auto &entity : entities) {
        if (entity->ray_intersect(ray, range, current_hit)) {
            range.max = current_hit.dist;
            hit = current_hit;
            any_hit = true;
        }
    }
    return any_hit;
}

} // ne
