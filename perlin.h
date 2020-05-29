#ifndef NE_PERLIN_H
#define NE_PERLIN_H

#include "vec.h"

#include <cmath>

namespace ne {
namespace perlin {

const int PointCount = 256;
extern Vec3 values[PointCount];
extern int perm_x[PointCount];
extern int perm_y[PointCount];
extern int perm_z[PointCount];

inline void init() {
    auto permute = [](int (&p)[PointCount]) {
        for (int i = 0; i < PointCount; ++i) {
            p[i] = i;
        }
        for (int i = PointCount-1; i > 0; --i) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    };
    permute(perm_x);
    permute(perm_y);
    permute(perm_z);

    for (int i = 0; i < PointCount; ++i) {
        Vec3 v(randomf(-1, 1), randomf(-1, 1), randomf(-1, 1));
        values[i] = v.normalized();
    }
}

inline float trilinear_interp(Vec3 c[2][2][2], float u, float v, float w) {
    // Smoothing
    float uu = u*u*(3.0f - 2.0f*u);
    float vv = v*v*(3.0f - 2.0f*v);
    float ww = w*w*(3.0f - 2.0f*w);

    float acc = 0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                Vec3 weight(u - i, v - j, w - k);
                acc += (i*uu + (1-i)*(1-uu)) *
                       (j*vv + (1-j)*(1-vv)) *
                       (k*ww + (1-k)*(1-ww)) *
                       Vec3::dot(c[i][j][k], weight);
            }
        }
    }
    return acc;
}

inline float noise(const Vec3 &p) {
    float u = p.x - floorf(p.x);
    float v = p.y - floorf(p.y);
    float w = p.z - floorf(p.z);

    int i = floorf(p.x);
    int j = floorf(p.y);
    int k = floorf(p.z);
    Vec3 c[2][2][2];

    for (int di = 0; di < 2; ++di) {
        for (int dj = 0; dj < 2; ++dj) {
            for (int dk = 0; dk < 2; ++dk) {
                int x = perm_x[(i+di) & 255];
                int y = perm_y[(j+dj) & 255];
                int z = perm_z[(k+dk) & 255];
                c[di][dj][dk] = values[x ^ y ^ z];
            }
        }
    }
    return trilinear_interp(c, u, v, w);
}

inline float turb(const Vec3 &p, int depth = 7) {
    float acc = 0;
    float weight = 1.0f;
    auto tmp = p;
    for (int i = 0; i < depth; ++i) {
        acc += weight*noise(tmp);
        weight *= 0.5f;
        tmp = tmp * 2.0f;
    }
    return fabsf(acc);
}

} // perlin
} // ne

#endif // NE_PERLIN_H
