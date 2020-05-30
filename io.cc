#include "io.h"
#include "texture.h"
#include "vec.h"
#include "color.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <ios>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace ne {

void str_split(const std::string &s, std::vector<std::string> &out, char sep) {
    out.clear();
    std::istringstream in(s);
    std::string line;
    while (std::getline(in, line, sep)) {
        out.push_back(line);
    }
}

std::vector<Vec3> read_obj(const std::string &filename) {
    std::vector<Vec3> verts;
    std::vector<Vec3> vindex;
    std::ifstream in(filename);

    if (!in) {
        return verts;
    }

    std::string line;
    std::vector<std::string> tokens;
    std::vector<std::string> buf;
    tokens.reserve(4);

    while (std::getline(in, line)) {
        if (line.size() == 0 || line[0] == '#') continue;
        str_split(line, tokens, ' ');

        if (tokens[0] == "v") {
            // Read vertex lookup
            float x = std::stof(tokens[1]);
            float y = std::stof(tokens[2]);
            float z = std::stof(tokens[3]);
            vindex.push_back(Vec3(x, y, z));
            continue;
        }

        if (tokens[0] == "f") {
            // Read face
            str_split(tokens[1], buf, '/');
            verts.push_back(vindex[std::stoi(buf[0])-1]);

            str_split(tokens[2], buf, '/');
            verts.push_back(vindex[std::stoi(buf[0])-1]);

            str_split(tokens[3], buf, '/');
            verts.push_back(vindex[std::stoi(buf[0])-1]);
        }
    }
    return verts;
}

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
