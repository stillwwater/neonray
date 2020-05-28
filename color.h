#ifndef NE_COLOR_H
#define NE_COLOR_H

#include <stdio.h>

#include "math.h"
#include "vec.h"

namespace ne {

struct Color24;

struct Color {
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Black;
    static const Color White;

    float r, g, b;

    Color() : r(0), g(0), b(0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
    Color(Vec3 v) : r(v.x), g(v.y), b(v.z) {}

    static inline Color lerp(const Color &a, const Color &b, float t);
    static inline Color random();
    static inline Color random(float min, float max);

    // Convert color to 24 bit color (rgb255)
    inline Color24 to_color24() const;

    // Clamp color values from 0 to 1.0 with gamma correction;
    static inline Color gamma2(const Color &c, float scale);

    bool operator==(const Color &c) const;
    bool operator!=(const Color &c) const;

    // Colors can be implicitly converted to Vec3
    explicit operator Vec3() const;
};

struct Color24 {
    unsigned char r, g, b;

    Color24() : r(0), g(0), b(0) {}

    Color24(unsigned char r, unsigned char g, unsigned char b)
        : r(r), g(g), b(b) {}

    // Convert 32 bit color to floating point color
    inline Color to_colorf() const;
};

inline Color Color::lerp(const Color &a, const Color &b, float t) {
    t = clamp01(t);
    return Color(a.r+(b.r - a.r)*t, a.g+(b.g - a.g)*t, a.b+(b.b - a.b)*t);
}

inline Color operator+(const Color &a, const Color &b) {
    return Color(a.r + b.r, a.g + b.g, a.b + b.b);
}

inline Color operator-(const Color &a, const Color &b) {
    return Color(a.r - b.r, a.g - b.g, a.b - b.b);
}

inline Color operator*(const Color &a, const Color &b) {
    return Color(a.r * b.r, a.g * b.g, a.b * b.b);
}

inline Color operator*(float s, const Color &c) {
    return Color(c.r * s, c.g * s, c.b * s);
}

inline Color operator*(const Color &c, float s) {
    return Color(c.r * s, c.g * s, c.b * s);
}

inline Color operator/(const Color &a, const Color &b) {
    return Color(a.r / b.r, a.g / b.g, a.b / b.b);
}

inline bool Color::operator==(const Color &c) const {
    return approx_eq(r, c.r) && approx_eq(g, c.g) && approx_eq(b, c.b);
}

inline bool Color::operator!=(const Color &c) const {
    return !approx_eq(r, c.r) || !approx_eq(g, c.g) || !approx_eq(b, c.b);
}

inline Color::operator Vec3() const {
    return Vec3(r, g, b);
}

inline Color Color::random() {
    return Color(randomf(), randomf(), randomf());
}

inline Color Color::random(float min, float max) {
    return Color(randomf(min, max), randomf(min, max), randomf(min, max));
}

inline Color Color::gamma2(const Color &c, float scale) {
    float r = clamp01(sqrtf(scale * c.r));
    float g = clamp01(sqrtf(scale * c.g));
    float b = clamp01(sqrtf(scale * c.b));
    return Color(r, g, b);
}

inline Color24 Color::to_color24() const {
    return Color24{
        static_cast<unsigned char>(r * 255.0f),
        static_cast<unsigned char>(g * 255.0f),
        static_cast<unsigned char>(b * 255.0f),
    };
}

inline Color Color24::to_colorf() const {
    return Color(r/255.0f, g/255.0f, b/255.0f);
}

} // ne

#endif // NE_COLOR_H
