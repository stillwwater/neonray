#ifndef NE_ENTITY_H
#define NE_ENTITY_H

#include <vector>
#include <memory>
#include <cstdio>

#include "ray.h"
#include "material.h"
#include "vec.h"
#include "aabb.h"

namespace ne {

class Material;

struct Hit {
    enum Face { Front_Face, Back_Face };

    Vec3 position;
    Vec3 normal;
    Vec3 uv;
    float dist;
    Face face;
    Material *material;
};

class Entity {
public:
    virtual bool ray_intersect(
        const Ray &ray,
        Range range,
        Hit &hit) const = 0;

    virtual bool bounding_box(Aabb &box) const = 0;

    virtual ~Entity() {};
};

class Sphere : public Entity {
public:
    Vec3 position;
    float radius;
    Material *material;

    Sphere() {}
    Sphere(Vec3 position, float radius, Material *material)
        : position(position), radius(radius), material(material) {}

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~Sphere() {}
};

class World : public Entity {
public:
    std::vector<std::shared_ptr<Entity>> entities;

    World() {}

    inline void clear();
    inline void add(std::shared_ptr<Entity> entity);

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~World() {}
};

// Bounding Volume Hierarchies
class BVH_Node : public Entity {
public:
    std::shared_ptr<Entity> left;
    std::shared_ptr<Entity> right;
    Aabb aabb;

    BVH_Node();

    BVH_Node(World &world)
        : BVH_Node(world.entities, 0, world.entities.size()) {}

    BVH_Node(std::vector<std::shared_ptr<Entity>> &entities,
             size_t start, size_t end);


    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~BVH_Node() {}
};

inline void World::clear() {
    entities.clear();
}

inline void World::add(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
}

} // ne

#endif // NE_ENTITY_H
