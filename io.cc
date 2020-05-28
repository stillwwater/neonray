#include "io.h"
#include "texture.h"

#include <cstdio>
#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <tuple>

namespace ne {

Texture *read_bmp(const std::string &filename) {
    std::ifstream in(filename, std::ios_base::binary);
    if (!in) {
        return nullptr;
    }

    BitmapFileHeader file_header;
    in.read((char *)&file_header, sizeof(file_header));

    BitmapDIBHeader dib_header;
    in.read((char *)&dib_header, sizeof(dib_header));

    auto tex = new Texture(dib_header.image_width, dib_header.image_height);

    for (int y = 0; y < dib_header.image_height; y++) {
        for (int x = 0; x < dib_header.image_width; x++) {
            Color24 bgr;
            in.read((char *)&bgr, sizeof(bgr));

            // Alpha unused
            unsigned char a;
            in >> a;

            Color24 rgb(bgr.b, bgr.g, bgr.r);
            tex->write_pixel(x, y, rgb.to_colorf());
        }
    }
    return tex;
}

bool write_bmp(const std::string &filename, const Texture *tex) {
    std::ofstream out(filename, std::ios_base::binary);
    if (!out) return false;

    int width = tex->width();
    int height = tex->height();

    int im_size = (width * 4) * height;
    int header_size = sizeof(BitmapFileHeader) + sizeof(BitmapDIBHeader);

    BitmapFileHeader file_header;
    file_header.file_size = header_size + im_size;
    file_header.pixel_array_offset = header_size;
    out.write((const char *)&file_header, sizeof(file_header));

    BitmapDIBHeader dib_header;
    dib_header.dib_header_size = sizeof(BitmapDIBHeader);
    dib_header.image_width = width;
    dib_header.image_height = height;
    dib_header.bits_per_pixel = 32; // RGBA
    out.write((const char *)&dib_header, sizeof(dib_header));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto rgb = tex->read_pixel(x, y).to_color24();
            Color24 bgr(rgb.b, rgb.g, rgb.r);

            out.write((const char *)&bgr, sizeof(bgr));

            // Alpha
            out << char(255);
        }
    }
    return true;
}

} // ne
