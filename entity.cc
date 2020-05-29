#include <cmath>
#include <algorithm>

#include "entity.h"
#include "vec.h"
#include "ray.h"
#include "aabb.h"

namespace ne {

inline void face_normal(const Ray &ray, Hit &hit) {
    if (Vec3::dot(ray.direction, hit.normal) >= 0) {
        // Invert normals if they are inside the entity
        hit.normal = -hit.normal;
        hit.face = Hit::Back_Face;
    } else {
        hit.face = Hit::Front_Face;
    }
}

Vec3 sphere_uv(const Vec3 &p) {
    // u = phi/2pi, v = theta/pi
    float phi = atan2f(p.z, p.x);
    float theta = asinf(p.y);
    float u = 1 - (phi + PI) / (2.0f*PI);
    float v = (theta + PI/2.0f) / PI;
    return Vec3(u, v, 0);
}

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
    hit.uv = sphere_uv((hit.position - position)/radius);
    face_normal(ray, hit);
    return true;
}

bool Sphere::bounding_box(Aabb &box) const {
    box = Aabb(position - radius*Vec3::one, position + radius*Vec3::one);
    return true;
}

bool PlaneXY::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    float dist = (z-ray.origin.z) / ray.direction.z;
    if (dist < range.min || dist > range.max) {
        return false;
    }
    float x = ray.origin.x + dist*ray.direction.x;
    float y = ray.origin.y + dist*ray.direction.y;
    if (x < x0 || x > x1 || y < y0 || y > y1) {
        return false;
    }
    hit.uv.x = (x - x0)/(x1 - x0);
    hit.uv.y = (y - y0)/(y1 - y0);
    hit.dist = dist;
    hit.normal = Vec3(0, 0, 1);
    hit.material = material;
    hit.position = ray.at(dist);
    face_normal(ray, hit);

    return true;
}

bool PlaneXY::bounding_box(Aabb &box) const {
    // Bounding box must have non-zero width, and planes are infinitely
    // thin on one axis so the box is padded.
    box = Aabb(Vec3(x0, y0, z-0.0001f), Vec3(x1, y1, z+0.0001f));
    return true;
}

bool PlaneXZ::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    float dist = (y-ray.origin.y) / ray.direction.y;
    if (dist < range.min || dist > range.max) {
        return false;
    }
    float x = ray.origin.x + dist*ray.direction.x;
    float z = ray.origin.z + dist*ray.direction.z;
    if (x < x0 || x > x1 || z < z0 || z > z1) {
        return false;
    }
    hit.uv.x = (x - x0)/(x1 - x0);
    hit.uv.y = (z - z0)/(z1 - z0);
    hit.dist = dist;
    hit.normal = Vec3(0, 1, 0);
    hit.material = material;
    hit.position = ray.at(dist);
    face_normal(ray, hit);

    return true;
}

bool PlaneXZ::bounding_box(Aabb &box) const {
    box = Aabb(Vec3(x0, y-0.0001f, z0), Vec3(x1, y+0.0001f, z1));
    return true;
}

bool PlaneYZ::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    float dist = (x-ray.origin.x) / ray.direction.x;
    if (dist < range.min || dist > range.max) {
        return false;
    }
    float y = ray.origin.y + dist*ray.direction.y;
    float z = ray.origin.z + dist*ray.direction.z;
    if (y < y0 || y > y1 || z < z0 || z > z1) {
        return false;
    }
    hit.uv.x = (y - y0)/(y1 - y0);
    hit.uv.y = (z - z0)/(z1 - z0);
    hit.dist = dist;
    hit.normal = Vec3(1, 0, 0);
    hit.material = material;
    hit.position = ray.at(dist);
    face_normal(ray, hit);
    return true;
}

bool PlaneYZ::bounding_box(Aabb &box) const {
    box = Aabb(Vec3(x-0.0001f, y0, z0), Vec3(x+0.0001f, y1, z1));
    return true;
}

bool Flip::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    if (!e->ray_intersect(ray, range, hit)) {
        return false;
    }
    if (hit.face == Hit::Front_Face)
        hit.face = Hit::Back_Face;
    else
        hit.face = Hit::Front_Face;
    return true;
}

bool Flip::bounding_box(Aabb &box) const {
    return e->bounding_box(box);
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

bool World::bounding_box(Aabb &box) const {
    if (entities.empty()) {
        return false;
    }
    Aabb temp_box;
    bool first_box;

    for (const auto &entity : entities) {
        if (!entity->bounding_box(temp_box)) {
            return false;
        }
        box = first_box ? temp_box : Aabb::enclose(box, temp_box);
        first_box = false;
    }
    return true;
}

BVH_Node::BVH_Node(std::vector<std::shared_ptr<Entity>> &entities,
                   size_t start, size_t end)
{
    int axis = random_int(0, 2);
    auto box_compare = [=](const std::shared_ptr<Entity> &a,
                           const std::shared_ptr<Entity> &b)
    {
        Aabb box_a;
        Aabb box_b;

        if (!a->bounding_box(box_a) || !b->bounding_box(box_b)) {
            printf("[error] BVH_Node without bounding box\n");
        }
        return box_a.min_bounds[axis] < box_b.min_bounds[axis];
    };

    size_t entity_span = end - start;
    switch (entity_span) {
    case 1:
        left = right = entities[start];
        break;
    case 2:
        if (box_compare(entities[start], entities[start+1])) {
            left = entities[start];
            right = entities[start+1];
        } else {
            left = entities[start+1];
            right = entities[start];
        }
        break;
    default:
        std::sort(entities.begin() + start,
                  entities.begin() + end,
                  box_compare);
        size_t mid = start + entity_span / 2;
        left = std::make_shared<BVH_Node>(entities, start, mid);
        right = std::make_shared<BVH_Node>(entities, mid, end);
        break;
    }

    Aabb box_left;
    Aabb box_right;
    if (!left->bounding_box(box_left)
        || !right->bounding_box(box_right))
    {
        printf("[error] BVH_Node without bounding box\n");
    }
    // Bounding box of this node encloses the boxes of its children
    aabb = Aabb::enclose(box_left, box_right);
}

bool BVH_Node::bounding_box(Aabb &box) const {
    box = aabb;
    return true;
}

bool BVH_Node::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    if (!aabb.intersect(ray, range.min, range.max)) {
        return false;
    }
    bool hit_left = left->ray_intersect(ray, range, hit);
    range.max = hit_left ? hit.dist : range.max;
    bool hit_right = right->ray_intersect(ray, range, hit);

    return hit_left || hit_right;
}

Box::Box(const Vec3 &p0, const Vec3 &p1, Material *m) {
    box_min = p0;
    box_max = p1;

    // Front and back
    sides.add(std::make_shared<PlaneXY>(p0.x, p1.x, p0.y, p1.y, p1.z, m));
    sides.add(std::make_shared<Flip>(
        std::make_shared<PlaneXY>(p0.x, p1.x, p0.y, p1.y, p0.z, m)));

    // Top and bottom
    sides.add(std::make_shared<PlaneXZ>(p0.x, p1.x, p0.z, p1.z, p1.y, m));
    sides.add(std::make_shared<Flip>(
        std::make_shared<PlaneXZ>(p0.x, p1.x, p0.z, p1.z, p0.y, m)));

    // Left and right
    sides.add(std::make_shared<PlaneYZ>(p0.y, p1.y, p0.z, p1.z, p1.x, m));
    sides.add(std::make_shared<Flip>(
        std::make_shared<PlaneYZ>(p0.y, p1.y, p0.z, p1.z, p0.x, m)));
}

bool Box::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    return sides.ray_intersect(ray, range, hit);
}

bool Box::bounding_box(Aabb &box) const {
    box = Aabb(box_min, box_max);
    return true;
}

bool Move::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    Ray moved(ray.origin - offset, ray.direction);
    if (!entity->ray_intersect(moved, range, hit)) {
        return false;
    }
    hit.position = hit.position + offset;
    face_normal(ray, hit);
    return true;
}

bool Move::bounding_box(Aabb &box) const {
    if (!entity->bounding_box(box)) {
        return false;
    }
    box = Aabb(box.min_bounds + offset, box.max_bounds + offset);
    return true;
}

RotateY::RotateY(std::shared_ptr<Entity> e, float angle) : entity(e) {
    float rad = radians(angle);
    sin_theta = sinf(rad);
    cos_theta = cosf(rad);
    has_box = entity->bounding_box(aabb);

    Vec3 min = Vec3::one * Infinity;
    Vec3 max = Vec3::one * -Infinity;

    // Rotated bounding box
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                float x = i*aabb.max_bounds.x + (1-i)*aabb.min_bounds.x;
                float y = j*aabb.max_bounds.y + (1-j)*aabb.min_bounds.y;
                float z = k*aabb.max_bounds.z + (1-k)*aabb.min_bounds.z;

                float rot_x = cos_theta*x + sin_theta*z;
                float rot_z = -sin_theta*x + cos_theta*z;
                Vec3 t(rot_x, y, rot_z);
                for (int c = 0; c < 3; ++c) {
                    min.a[c] = fminf(min[c], t[c]);
                    max.a[c] = fmaxf(max[c], t[c]);
                }
            }
        }
    }
    aabb = Aabb(min, max);
}

bool RotateY::ray_intersect(const Ray &ray, Range range, Hit &hit) const {
    Vec3 origin = ray.origin;
    Vec3 direction = ray.direction;

    origin.x = cos_theta*ray.origin.x - sin_theta*ray.origin.z;
    origin.z = sin_theta*ray.origin.x + cos_theta*ray.origin.z;
    direction.x = cos_theta*ray.direction.x - sin_theta*ray.direction.z;
    direction.z = sin_theta*ray.direction.x + cos_theta*ray.direction.z;

    Ray rot(origin, direction);
    if (!entity->ray_intersect(rot, range, hit)) {
        return false;
    }

    Vec3 position = hit.position;
    Vec3 normal = hit.normal;

    position.x = cos_theta*hit.position.x + sin_theta*hit.position.z;
    position.z = -sin_theta*hit.position.x + cos_theta*hit.position.z;
    normal.x = cos_theta*hit.normal.x + sin_theta*hit.normal.z;
    normal.z = -cos_theta*hit.normal.x + cos_theta*hit.normal.z;

    hit.position = position;
    hit.normal = normal;
    face_normal(rot, hit);
    return true;
}

bool RotateY::bounding_box(Aabb &box) const {
    box = aabb;
    return has_box;
}

} // ne
