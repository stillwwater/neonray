#include "texture.h"
#include "color.h"

#include <stdio.h>
#include <assert.h>

namespace ne {

Texture::Texture(int w, int h) {
    this->w = w;
    this->h = h;
    buffer = new Color[w * h];
}

Texture::~Texture() {
    delete[] buffer;
}

Texture *Texture::gradient(int width, int height) {
    auto tex = new Texture(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color col(float(x)/(width-1), float(y)/(height-1), 0.25f);
            tex->write_pixel(x, y, col);
        }
    }
    return tex;
}

Texture *Texture::solid_color(int width, int height, const Color &color) {
    auto tex = new Texture(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tex->write_pixel(x, y, color);
        }
    }
    return tex;
}

void Texture::paste(const Texture *dst, const Texture *src, int x, int y) {
    assert(src->width() + x <= dst->width());
    assert(src->height() + y <= dst->width());

    for (int v = 0; v < src->height(); ++v) {
        for (int u = 0; u < src->width(); ++u) {
            auto color = src->read_pixel(u, v);
            dst->write_pixel(u + x, v + y, color);
        }
    }
}

} // ne
