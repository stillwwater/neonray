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

class PlaneXY : public Entity {
public:
    float x0, x1, y0, y1, z;
    Material *material;

    PlaneXY(float x0, float x1, float y0, float y1, float z, Material *m)
        : x0(x0), x1(x1), y0(y0), y1(y1), z(z), material(m) {}

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~PlaneXY() {}
};

class PlaneXZ : public Entity {
public:
    float x0, x1, z0, z1, y;
    Material *material;

    PlaneXZ(float x0, float x1, float z0, float z1, float y, Material *m)
        : x0(x0), x1(x1), z0(z0), z1(z1), y(y), material(m) {}

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~PlaneXZ() {}
};

class PlaneYZ : public Entity {
public:
    float y0, y1, z0, z1, x;
    Material *material;

    PlaneYZ(float y0, float y1, float z0, float z1, float x, Material *m)
        : y0(y0), y1(y1), z0(z0), z1(z1), x(x), material(m) {}

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~PlaneYZ() {}
};

class Flip : public Entity {
public:
    std::shared_ptr<Entity> e;

    Flip(std::shared_ptr<Entity> e) : e(e) {}

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~Flip() {}
};

class Move : public Entity {
public:
    std::shared_ptr<Entity> entity;
    Vec3 offset;

    Move(std::shared_ptr<Entity> e, const Vec3 &offset)
        : entity(e), offset(offset) {}

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~Move() {}
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

class Box : public Entity {
public:
    World sides;
    Vec3 box_min;
    Vec3 box_max;

    Box() {}
    Box(const Vec3 &p0, const Vec3 &p1, Material *m);

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~Box() {}
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

class RotateY : public Entity {
public:
    std::shared_ptr<Entity> entity;
    float sin_theta;
    float cos_theta;
    bool has_box;
    Aabb aabb;

    RotateY(std::shared_ptr<Entity> entity, float angle);

    virtual bool ray_intersect(const Ray &ray, Range range, Hit &hit) const;
    virtual bool bounding_box(Aabb &box) const;

    ~RotateY() {}
};

} // ne

#endif // NE_ENTITY_H
