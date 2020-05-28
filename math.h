#ifndef NE_MATH_H
#define NE_MATH_H

#include <cmath>
#include <cstdlib>
#include <limits>

namespace ne {

const float Epsilon = 0.00001f;
const float MinDist = 0.001f;
const float Infinity = std::numeric_limits<float>::max();
const float PI = 3.1415927f;

struct Range {
    float min, max;
    Range(float min, float max) : min(min), max(max) {}
};

inline float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline float clamp01(float value) {
    return clamp(value, 0.0f, 1.0f);
}

inline float radians(float degrees) {
    return degrees * PI / 180.0f;
}

// Random number in range [0,1)
inline float randomf() {
    return rand() / (static_cast<float>(RAND_MAX) + 1.0f);
}

inline float randomf(float min, float max) {
    return min + (max - min)*randomf();
}

inline int random_int(int min, int max) {
    return static_cast<int>(randomf(min, max + 1));
}

inline bool approx_eq(float a, float b) {
    return std::abs(a - b) < Epsilon;
}

} // ne

#endif // NE_MATH_H
