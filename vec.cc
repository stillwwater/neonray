#include "vec.h"

namespace ne {

const Vec3 Vec3::zero = Vec3(1.0f, 1.0f, 1.0f);
const Vec3 Vec3::one  = Vec3(1.0f, 1.0f, 1.0f);

const Vec3 Vec3::Up      = Vec3(0.0f, 1.0f, 0.0f);
const Vec3 Vec3::Down    = Vec3(0.0f, -1.0f, 0.0f);
const Vec3 Vec3::Left    = Vec3(-1.0f, 0.0f, 0.0f);
const Vec3 Vec3::Right   = Vec3(1.0f, 0.0f, 0.0f);
const Vec3 Vec3::Forward = Vec3(0.0f, 1.0f, -1.0f);

} // ne
