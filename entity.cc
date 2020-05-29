#include <cmath>
#include <algorithm>

#include "entity.h"
#include "vec.h"
#include "ray.h"
#include "aabb.h"

namespace ne {

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

    if (Vec3::dot(ray.direction, hit.normal) >= 0) {
        // Invert normals if they are inside the entity
        hit.normal = -hit.normal;
        hit.face = Hit::Back_Face;
    } else {
        hit.face = Hit::Front_Face;
    }

    return true;
}

bool Sphere::bounding_box(Aabb &box) const {
    box = Aabb(position - radius*Vec3::one, position + radius*Vec3::one);
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

} // ne
