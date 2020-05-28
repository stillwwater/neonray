#ifndef NE_RENDERER_H
#define NE_RENDERER_H

#include "texture.h"
#include "color.h"
#include "vec.h"
#include "ray.h"
#include "entity.h"
#include "camera.h"

namespace ne {

struct RenderChunk {
    int offset_x;
    int offset_y;
    int tex_width;
    int tex_height;
};

struct RenderJob {
    int tid;
    int aa_samples;
    int max_depth;
    int seed;
    RenderChunk chunk;
};

class Renderer {
public:
    int aa_samples;
    int max_depth;
    int threads;
    int chunk_size;

    Renderer(int aa_samples, int max_depth, int threads, int chunk_size)
        : aa_samples(aa_samples),
          max_depth(max_depth),
          threads(threads),
          chunk_size(chunk_size) {}

    Renderer(int aa_samples, int max_depth, int threads)
        : aa_samples(aa_samples),
          max_depth(max_depth),
          threads(threads) { chunk_size = 64; }

    void render(const Camera &camera,
                const Entity *entity,
                const Texture *render_tex) const;

    void render_progressive(const Camera &camera,
                            const Entity *entity,
                            const Texture *render_tex) const;

    static Color trace_ray(const Ray &ray, const Entity *entity, int depth);

private:
    void render_chunk(const Camera &camera,
                      const Entity *entity,
                      const Texture *render_tex,
                      const RenderChunk &chunk) const;
};

}

#endif // NE_RENDERER_H
