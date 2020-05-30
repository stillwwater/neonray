#include "color.h"
#include "camera.h"
#include "texture.h"
#include "material.h"
#include "vec.h"
#include "entity.h"
#include "ray.h"
#include "io.h"
#include "renderer.h"
#include "shader.h"
#include "perlin.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
#include <cmath>

using namespace ne;

std::unique_ptr<World> cornell_box() {
    auto world = std::make_unique<World>();
    World boxes;

    auto red = new Diffuse(surf_solid_color(), Color(0.65f, 0.05f, 0.05f));
    auto green = new Diffuse(surf_solid_color(), Color(0.12f, 0.45f, 0.15f));
    auto white = new Diffuse(surf_solid_color(), Color(0.73f, 0.73f, 0.73f));
    auto light = new Light(Color(1, 0.878, 0.768) * 38.0f);

    world->add(std::make_shared<Flip>(
                std::make_shared<PlaneYZ>(0, 555, 0, 555, 555, red)));

    world->add(std::make_shared<PlaneYZ>(0, 555, 0, 555, 0, green));

    world->add(std::make_shared<PlaneXZ>(213, 343, 227, 332, 554, light));

    world->add(std::make_shared<Flip>(
                std::make_shared<PlaneXZ>(0, 555, 0, 555, 555, white)));

    world->add(std::make_shared<PlaneXZ>(0, 555, 0, 555, 0, white));
    world->add(std::make_shared<Flip>(
                std::make_shared<PlaneXY>(0, 555, 0, 555, 555, white)));

    std::shared_ptr<Entity> box1 =
        std::make_shared<Box>(Vec3::zero, Vec3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Move>(box1, Vec3(265, 0, 295));
    boxes.add(box1);

    std::shared_ptr<Entity> box2 =
        std::make_shared<Box>(Vec3::zero, Vec3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Move>(box2, Vec3(130, 0, 65));
    boxes.add(box2);
    world->add(std::make_shared<BVH_Node>(boxes));
    return world;
}

std::unique_ptr<World> basic_scene() {
    auto world = std::make_unique<World>();

    auto m = new Diffuse(surf_marble(), Color::Black);
    auto c = new Diffuse(surf_checker(), Color::Black);
    world->add(std::make_shared<Sphere>(Vec3(0, -1000 ,0), 1000, c));
    world->add(std::make_shared<Sphere>(Vec3(0, 2, 0), 2, m));

    auto l = new Light(Color::White * 4);
    world->add(std::make_shared<Sphere>(Vec3(0, 7, 0), 2, l));
    world->add(std::make_shared<PlaneYZ>(3, 5, 1, 3, -2, l));

    return world;
}

std::unique_ptr<World> random_scene() {
    auto world = std::make_unique<World>();
    World small_spheres;

    auto ground = new Diffuse(surf_checker(), Color(0.03f, 0.01f, 0.05f));
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
                mat = new Diffuse(surf_solid_color(), albedo);
            } else if (rmat < 0.95f) {
                auto albedo = Color::random(0.5f, 1);
                float rough = randomf(0, 0.5f);
                mat = new Metal(surf_solid_color(), albedo, rough);
            } else {
                mat = new Dielectric(1.5f);
            }
            small_spheres.add(std::make_unique<Sphere>(center, 0.2f, mat));
        }
    }

    world->add(std::make_shared<BVH_Node>(small_spheres));

    auto mat1 = new Dielectric(1.5f);
    world->add(std::make_unique<Sphere>(Vec3(0, 1, 0), 1.0f, mat1));

    auto mat2 = new Diffuse(surf_solid_color(), Color::Red);
    world->add(std::make_unique<Sphere>(Vec3(-4, 1, 0), 1.0f, mat2));

    auto mat3 = new Metal(surf_solid_color(), Color(0.7f, 0.6f, 0.5f), 0.0f);
    world->add(std::make_unique<Sphere>(Vec3(4, 1, 0), 1.0f, mat3));

    return world;
}

std::unique_ptr<World> scene_cube() {
    auto world = std::make_unique<World>();
    auto white = new Diffuse(surf_marble(), Color(0.73f, 0.73f, 0.73f));

    Vec3 v[] = {
        {0, 0, 0},
        {0.891838 , 1.199458 , -0.954319},
        {0.891838 , -0.800542, -0.954319},
        {0.891838 , 1.199458 , 1.045681},
        {0.891838 , -0.800542, 1.045681},
        {-1.108162, 1.199458 , -0.954319},
        {-1.108162, -0.800542, -0.954319},
        {-1.108162, 1.199458 , 1.045681},
        {-1.108162, -0.800542, 1.045681},
    };

    Vec3 tris[] = {
        v[5], v[3], v[1],
        v[3], v[8], v[4],
        v[7], v[6], v[8],
        v[2], v[8], v[6],
        v[1], v[4], v[2],
        v[5], v[2], v[6],
        v[5], v[7], v[3],
        v[3], v[7], v[8],
        v[7], v[5], v[6],
        v[2], v[4], v[8],
        v[1], v[3], v[4],
        v[5], v[1], v[2],
    };
    std::vector<Vec3> verts;
    for (int i = 0; i < 36; ++i) {
        verts.push_back(tris[i]);
    }
    world->add(std::make_shared<Mesh>(verts, white));

    return world;
}

std::unique_ptr<World> scene_mesh(const std::string &filename) {
    auto verts = read_obj(filename);
    auto world = std::make_unique<World>();
    auto white = new Diffuse(surf_solid_color(), Color::White);
    auto ground = new Metal(surf_checker(), Color(0, 0, 0), 0);

    world->add(std::make_shared<Mesh>(verts, white));
    world->add(std::make_shared<PlaneXZ>(-555, 555, -555, 555, -1, ground));
    return world;
}

int main(void) {
    srand(1018);
    perlin::init();
    auto tex = new Texture(720, 720);
    write_bmp("tex.bmp", tex);

    int width = tex->width();
    int height = tex->height();
    float aspect = float(width) / float(height);

    Vec3 cam_pos(278, 278, -800);
    Vec3 cam_lookat(278, 278, 0);
    float focus = 10.0f;
    float aperture = 0;
    Camera camera(cam_pos, cam_lookat, Vec3::Up, 40, aspect, aperture, focus);

    auto world = cornell_box();

    Renderer renderer(2000, 20, 4);
    renderer.render_progressive(camera, world.get(), tex);

    return 0;
}
