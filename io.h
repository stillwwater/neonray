#ifndef NE_IO_H
#define NE_IO_H

#include "texture.h"

#include <cstddef>
#include <string>
#include <tuple>
#include <memory>
#include <vector>

namespace ne {

#pragma pack(push, 1)
struct BitmapFileHeader {
    uint16_t signature = 0x4d42;
    uint32_t file_size = 0;
    uint16_t reserved1, reserved2;
    uint32_t pixel_array_offset = 0;
};

struct BitmapDIBHeader {
    uint32_t dib_header_size = 40;

    // If height is negative (0,0) is in top-left corner
    int32_t image_width = 0;
    int32_t image_height = 0;

    // Number of device planes is always 1
    uint16_t planes = 1;
    uint16_t bits_per_pixel = 0;

    uint32_t compression = 0;
    uint32_t image_size = 0;
    uint32_t x_ppm = 0;
    uint32_t y_ppm = 0;

    // Maximum number of color indexes
    // 0 means use all colors allowed by bits_per_pixel
    uint32_t color_count = 0;
    uint32_t important_color_count = 0;
};
#pragma pack(pop)

Texture *read_bmp(const std::string &filename);
bool write_bmp(const std::string &filename, const Texture *tex);

std::vector<Vec3> read_obj(const std::string &filename);

} // ne

#endif // NE_IO_H
