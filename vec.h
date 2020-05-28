#ifndef NE_VEC_H
#define NE_VEC_H

#include "math.h"

#include <cmath>
#include <stdio.h>

namespace ne {

struct Vec3 {
    static const Vec3 zero;
    static const Vec3 one;
    static const Vec3 Up;
    static const Vec3 Left;
    static const Vec3 Right;
    static const Vec3 Down;
    static const Vec3 Forward;

    union {
        struct {
            float x, y, z;
        };
        float a[3];
    };

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3(const Vec3 &v) : x(v.x), y(v.y), z(v.z) {}

    float length() const;
    float length_sqr() const;

    // Returns the normalized vector to length 1
    Vec3 normalized() const;

    static inline float dot(const Vec3 &a, const Vec3 &b);
    static inline Vec3 cross(const Vec3 &a, const Vec3 &b);

    // Multiply each component of one vector with another
    static inline Vec3 scale(const Vec3 &a, const Vec3 &b);

    // Lineraly interpolate between two vectors
    static inline Vec3 lerp(const Vec3 &a, const Vec3 &b, float t);

    // Reflect a vector to normal
    static inline Vec3 reflect(const Vec3 &v, const Vec3 &normal);

    // Refract a vector on normal
    static inline Vec3 refract(const Vec3 &v, const Vec3 &n, float etai_etat);

    // Returns a vector of length 1 with a random direction
    static inline Vec3 random_lambertian();
    static inline Vec3 random_in_unit_sphere();
    static inline Vec3 random_in_unit_circle();
    static inline Vec3 random_in_hemisphere(const Vec3 &normal);

    Vec3 operator-() const;
    float operator[](int index) const;

    bool operator==(const Vec3 &v) const;
    bool operator!=(const Vec3 &v) const;
};


inline Vec3 operator+(const Vec3 &a, const Vec3& b) {
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vec3 operator-(const Vec3 &a, const Vec3& b) {
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vec3 operator*(const Vec3 &v, float s) {
    return Vec3(v.x * s, v.y * s, v.z * s);
}

inline Vec3 operator*(float s, const Vec3 &v) {
    return Vec3(v.x * s, v.y * s, v.z * s);
}

inline Vec3 operator/(const Vec3 &v, float s) {
    return Vec3(v.x / s, v.y / s, v.z / s);
}

inline float Vec3::length() const {
    return sqrtf(x*x + y*y + z*z);
}

inline float Vec3::length_sqr() const {
    return x*x + y*y + z*z;
}

inline float Vec3::dot(const Vec3 &a, const Vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 Vec3::cross(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

inline Vec3 Vec3::lerp(const Vec3 &a, const Vec3 &b, float t) {
    t = clamp01(t);
    return Vec3(a.x + (b.x - a.x)*t, a.y + (b.y - a.y)*t, a.z + (b.z - a.z)*t);
}

inline Vec3 Vec3::reflect(const Vec3 &v, const Vec3 &normal) {
    // R = V - (2*N*(dot(V, N)))
    float factor = 2.0f * dot(v, normal);
    return {
        v.x - normal.x*factor,
        v.y - normal.y*factor,
        v.z - normal.z*factor,
    };
}

inline Vec3 Vec3::refract(const Vec3 &v, const Vec3 &n, float etai_etat) {
    // R'|| = eta/eta'(R + (-R dot n)n)
    float cos_theta = Vec3::dot(-v, n);
    Vec3 r_parallel = etai_etat * (v + cos_theta*n);
    Vec3 r_perp = -sqrtf(1.0f - r_parallel.length_sqr()) * n;
    return r_parallel + r_perp;
}

inline Vec3 Vec3::random_in_unit_sphere() {
    for (;;) {
        Vec3 r{randomf(-1.0f, 1.0f),
               randomf(-1.0f, 1.0f),
               randomf(-1.0f, 1.0f)};
        if (r.length_sqr() < 1) return r;
    }
}

inline Vec3 Vec3::random_in_unit_circle() {
    for (;;) {
        Vec3 r{randomf(-1, 1), randomf(-1, 1), 0};
        if (r.length_sqr() < 1) return r;
    }
}

inline Vec3 Vec3::random_lambertian() {
    float a = randomf(0, 2.0f*PI);
    float z = randomf(-1.0f, 1.0f);
    float r = sqrtf(1.0f - z*z);
    return Vec3(r*cosf(a), r*sinf(a), z);
}

inline Vec3 Vec3::random_in_hemisphere(const Vec3 &normal) {
    Vec3 r = random_in_unit_sphere();
    if (Vec3::dot(r, normal) > 0) {
        return r;
    }
    return -r;
}

inline Vec3 Vec3::normalized() const {
    float len = length();
    if (len > Epsilon) {
        return Vec3(x/len, y/len, z/len);
    }
    return *this;
}

inline bool Vec3::operator==(const Vec3 &v) const {
    float dx = x - v.x;
    float dy = y - v.y;
    float dz = z - v.z;
    float len_sqr = dx*dx + dy*dy + dz*dz;
    return len_sqr < Epsilon*Epsilon;
}

inline bool Vec3::operator!=(const Vec3 &v) const {
    float dx = x - v.x;
    float dy = y - v.y;
    float dz = z - v.z;
    float len_sqr = dx*dx + dy*dy + dz*dz;
    return len_sqr >= Epsilon*Epsilon;
}

inline Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

inline float Vec3::operator[](int index) const {
    return a[index];
}

} // ne

#endif // NE_VEC_H
