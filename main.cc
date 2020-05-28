#include "color.h"
#include "camera.h"
#include "texture.h"
#include "material.h"
#include "vec.h"
#include "entity.h"
#include "ray.h"
#include "io.h"
#include "renderer.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
#include <cmath>

using namespace ne;

std::unique_ptr<World> random_scene() {
    auto world = std::make_unique<World>();
    auto ground = new Diffuse(Color(0.5f, 0.5f, 0.5f));
    world->add(std::make_unique<Sphere>(Vec3(0,-1000,0), 1000, ground));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float rmat = randomf();
            Vec3 center(a + 0.9f*randomf(), 0.2f, b + 0.9f*randomf());

            if ((center - Vec3(4, 0.2f, 0)).length() <= 0.9f) {
                continue;
            }

            Material *mat;
            if (rmat < 0.8f) {
                // diffuse
                auto albedo = Color::random() * Color::random();
                mat = new Diffuse(albedo);
            } else if (rmat < 0.95f) {
                auto albedo = Color::random(0.5f, 1);
                float rough = randomf(0, 0.5f);
                mat = new Metal(albedo, rough);
            } else {
                mat = new Dielectric(1.5f);
            }
            world->add(std::make_unique<Sphere>(center, 0.2f, mat));
        }
    }

    auto mat1 = new Dielectric(1.5f);
    world->add(std::make_unique<Sphere>(Vec3(0, 1, 0), 1.0f, mat1));

    auto mat2 = new Diffuse(Color::Red);
    world->add(std::make_unique<Sphere>(Vec3(-4, 1, 0), 1.0f, mat2));

    auto mat3 = new Metal(Color(0.7f, 0.6f, 0.5f), 0.0f);
    world->add(std::make_unique<Sphere>(Vec3(4, 1, 0), 1.0f, mat3));

    return world;
}

int main(void) {
    auto tex = new Texture(800, 600);
    write_bmp("tex.bmp", tex);

    int width = tex->width();
    int height = tex->height();
    float aspect = float(width) / float(height);

    Vec3 cam_pos(13, 2, 3);
    Vec3 cam_lookat(0, 0, 0);
    float focus = 10.0f;
    float aperture = 0.1f;
    Camera camera(cam_pos, cam_lookat, Vec3::Up, 20, aspect, aperture, focus);

    auto world = random_scene();

    Renderer renderer(20, 10, 4);
    renderer.render_progressive(camera, world.get(), tex);

    write_bmp("tex.bmp", tex);
    return 0;
}
