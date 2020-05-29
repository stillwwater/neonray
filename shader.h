#ifndef NE_SHADER_H
#define NE_SHADER_H

#include "vec.h"
#include "color.h"

namespace ne {

struct v2f {
    Vec3 uv;
    Vec3 p;
    Color albedo;
};

using Shader = Color (*)(const v2f &in);

auto surf_solid_color() -> Shader;
auto surf_checker() -> Shader;
auto surf_xor() -> Shader;
auto surf_noise() -> Shader;
auto surf_marble() -> Shader;

} // ne

#endif // NE_SHADER_H
