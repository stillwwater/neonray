#include "material.h"
#include "ray.h"
#include "vec.h"
#include "entity.h"

#include <cmath>

namespace ne {

float schlick(float cos_theta, float ri) {
    float r0 = (1.0f - ri) / (1.0f + ri);
    r0 = r0*r0;
    float m = (1.0f - cos_theta);
    float m2 = m * m;
    return r0 + (1.0f - r0) * m2*m2*m; // pow(m, 5)
}

bool Diffuse::scatter(
    const Ray &r_in, const Hit &hit, Color &attenuation, Ray &r_out
) const {
    Vec3 direction = hit.normal + Vec3::random_lambertian();
    r_out = Ray(hit.position, direction);
    attenuation = shader(v2f{hit.uv, hit.position, albedo});
    return true;
}

bool Metal::scatter(
    const Ray &r_in, const Hit &hit, Color &attenuation, Ray &r_out
) const {
    Vec3 reflected = Vec3::reflect(r_in.direction.normalized(), hit.normal);
    Vec3 r = roughness * Vec3::random_in_hemisphere(hit.normal);
    r_out = Ray(hit.position, reflected + r);
    attenuation = albedo;

    // Absorb ray if it points towards the surface
    return Vec3::dot(r_out.direction, hit.normal) > 0;
}

bool Dielectric::scatter(
    const Ray &r_in, const Hit &hit, Color &attenuation, Ray &r_out
) const {
    attenuation = albedo;
    float etai_etat = hit.face == Hit::Front_Face ? 1.0f / ri : ri;

    Vec3 direction = r_in.direction.normalized();
    float cos_theta = fminf(Vec3::dot(-direction, hit.normal), 1.0f);
    float sin_theta = sqrtf(1.0f - cos_theta*cos_theta);

    if (etai_etat * sin_theta > 1.0f || randomf() < schlick(cos_theta, ri)) {
        // Cannot refract ray
        Vec3 reflected = Vec3::reflect(direction, hit.normal);
        r_out = Ray(hit.position, reflected);
        return true;
    }

    Vec3 refracted = Vec3::refract(direction, hit.normal, etai_etat);
    r_out = Ray(hit.position, refracted);
    return true;
}

} // ne
