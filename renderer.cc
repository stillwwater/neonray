#include "renderer.h"
#include "vec.h"
#include "ray.h"
#include "texture.h"
#include "math.h"
#include "camera.h"
#include "io.h"

#include <cstdio>
#include <vector>
#include <thread>
#include <cmath>

namespace ne {

void render_job(const Camera &camera,
                const Entity *entity,
                const Texture *render_tex,
                const RenderJob &job)
{
    srand(job.seed);
    int width = render_tex->width();
    int height = render_tex->height();
    auto bg = Color::Black;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto color = Color::Black;
            for (int n = 0; n < job.aa_samples; ++n) {
                float h = float(job.chunk.tex_height) - 1;
                float u = (float(x) + randomf()) / float(width - 1);
                float v = (float(y + job.chunk.offset_y) + randomf()) / h;
                auto ray = camera.ray_from_view(u, v);
                color = color + Renderer::trace_ray(ray, bg, entity, job.max_depth);
            }
            color = Color::gamma2(color, 1.0f / job.aa_samples);
            render_tex->write_pixel(x, y, color);
        }
        if (job.tid == 0 && y % 10 == 0) {
            float progress = float(y + job.chunk.offset_y)
                           / job.chunk.tex_height * 100.0f;
            printf("\rrender: %d%%", int(progress));
            fflush(stdout);
        }
    }
}

void tex_blend(const Texture *dst, const std::vector<Texture *> &src) {
    float n = 1 + src.size();
    for (int y = 0; y < dst->height(); ++y) {
        for (int x = 0; x < dst->width(); ++x) {
            auto pixel = dst->read_pixel(x, y);
            for (auto &tex : src) {
                pixel = pixel + tex->read_pixel(x, y);
            }
            pixel = pixel * (1.0f / n);
            dst->write_pixel(x, y, pixel);
        }
    }
}

void Renderer::render_chunk(const Camera &camera,
                            const Entity *entity,
                            const Texture *render_tex,
                            const RenderChunk &chunk) const
{
    int s = random_int(0, 0xffff);
    int n = std::min(threads, aa_samples);
    if (n <= 1) {
        RenderJob job{0, aa_samples, max_depth, s, chunk};
        render_job(camera, entity, render_tex, job);
        return;
    }

    int chunk_samples = aa_samples / n;
    int rem = aa_samples % n;

    std::vector<std::thread> workers;
    std::vector<Texture *> results;

    for (int i = 1; i < n; ++i) {
        auto tex = new Texture(render_tex->width(), render_tex->height());
        int t_s = random_int(0, 0xffff);

        RenderJob job{i, chunk_samples, max_depth, t_s, chunk};
        workers.push_back(std::thread(render_job, camera, entity, tex, job));
        results.push_back(tex);
    }
    // Render on main thread
    RenderJob job{0, chunk_samples + rem, max_depth, s, chunk};
    render_job(camera, entity, render_tex, job);

    // Wait for other threads to complete
    for (auto &w : workers) {
        w.join();
    }

    tex_blend(render_tex, results);

    for (auto tex : results) {
        delete tex;
    }
}

void Renderer::render(const Camera &camera,
                      const Entity *entity,
                      const Texture *render_tex) const
{
    RenderChunk chunk{0, 0, render_tex->width(), render_tex->height()};
    render_chunk(camera, entity, render_tex, chunk);
}

void Renderer::render_progressive(const Camera &camera,
                                  const Entity *entity,
                                  const Texture *render_tex) const
{
    int chunks = render_tex->height() / chunk_size;
    int rem = render_tex->height() % chunk_size;
    RenderChunk chunk{0, 0, render_tex->width(), render_tex->height()};

    auto buffer = new Texture(render_tex->width(), chunk_size + rem);
    render_chunk(camera, entity, buffer, chunk);
    Texture::paste(render_tex, buffer, 0, 0);
    write_bmp("tex.bmp", render_tex);
    delete buffer;

    buffer = new Texture(render_tex->width(), chunk_size);
    for (int i = 1; i < chunks; ++i) {
        chunk.offset_y = i * chunk_size + rem;
        render_chunk(camera, entity, buffer, chunk);

        Texture::paste(render_tex, buffer, 0, i * chunk_size + rem);
        write_bmp("tex.bmp", render_tex);
    }
    printf("\rrender: 100%%\n");
    delete buffer;
}

Color Renderer::trace_ray(const Ray &r_in,
                          const Color &bg,
                          const Entity *entity,
                          int depth)
{
    if (depth <= 0) {
        return Color::Black;
    }
    Hit hit;

    if (!entity->ray_intersect(r_in, Range{MinDist, Infinity}, hit)) {
        return bg;
        //auto direction = r_in.direction.normalized();
        //float t = 0.5f*(direction.y + 1.0f);
        //return Color::lerp(Color::White, Color(0.5f, 0.7f, 1.0f), t);
    }

    Ray r_out;
    Color attenuation;
    Color emitted = hit.material->emitted(hit.uv.x, hit.uv.y, hit.position);

    if (!hit.material->scatter(r_in, hit, attenuation, r_out)) {
        return emitted;
    }
    return emitted + attenuation * trace_ray(r_out, bg, entity, depth - 1);

}

} // ne
