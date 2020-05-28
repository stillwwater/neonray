#ifndef NE_TEXTURE_H
#define NE_TEXTURE_H

#include "color.h"

#include <string>

namespace ne {

class Texture {
public:
    const static int Channels = 3;

    Texture() : w(0), h(0) {}
    Texture(int w, int h);

    static Texture *gradient(int width, int height);
    static Texture *solid_color(int width, int height, const Color &color);
    static void paste(const Texture *dst, const Texture *src, int x, int y);

    inline int width() const;
    inline int height() const;

    inline Color sample(float u, float v) const;
    inline Color sample(Vec3 uv) const;
    inline Color read_pixel(int x, int y) const;
    inline void write_pixel(int x, int y, Color c) const;

    ~Texture();

private:
    Color *buffer;
    int w, h;
};

inline Color Texture::sample(float u, float v) const {
    if (buffer == nullptr) {
        // Null texture
        return Color(1, 0, 1);
    }

    u = clamp01(u);
    v = clamp01(v);

    int x = static_cast<int>(u * w);
    int y = static_cast<int>(v * h);

    // u, v can be 1.0f, clamp x and y to not read outside of the image
    if (x >= w) x = w-1;
    if (y >= h) y = h-1;

    return read_pixel(x, y);
}

inline Color Texture::sample(Vec3 uv) const {
    // Z component unused
    return sample(uv.x, uv.y);
}

inline Color Texture::read_pixel(int x, int y) const {
    return buffer[x + w * y];
}

inline void Texture::write_pixel(int x, int y, Color c) const {
    buffer[x + w * y] = c;
}

inline int Texture::height() const {
    return h;
}

inline int Texture::width() const {
    return w;
}

} // ne

#endif // NE_TEXTURE_H
