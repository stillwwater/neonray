#include "shader.h"
#include "perlin.h"

#include <cmath>

namespace ne {

auto surf_solid_color() -> Shader {
    return [](const v2f &in) {
        return in.albedo;
    };
}

auto surf_checker() -> Shader {
    return [](const v2f &in) {
        Vec3 p = 6*in.p;
        float s = sinf(p.x)*sinf(p.y)*sinf(p.z);
        if (s < 0) {
            return in.albedo;
        }
        return Color::White;
    };
}

auto surf_xor() -> Shader {
    return [](const v2f &in) {
        float v = (int(in.uv.x*255) ^ int(in.uv.y*255)) / 255;
        return v * in.albedo;
    };
}

auto surf_noise() -> Shader {
    return [](const v2f &in) {
        return Color::White * 0.5f * (1.0f + perlin::noise(in.p * 4));
    };
}

auto surf_marble() -> Shader {
    return [](const v2f &in) {
        float n = sinf(4.0f*in.p.z + 10.0f*perlin::turb(in.p));
        auto marble = Color::White * 0.5f * (1.0f + n);
        return marble;
    };
}

} // ne
