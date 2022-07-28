#include "../include/Texture.h"
#include "../include/stb_image_impl.h"

// maybe we need to change linear space
Texture::Texture(const char* filename)
{
    auto components_per_pixel = bytes_per_pixel;

    data = stbi_load(
        filename, &width, &height, &components_per_pixel, components_per_pixel);

    if (!data) {
        std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
        width = height = 0;
    }

    bytes_per_scanline = bytes_per_pixel * width;
}

Texture::~Texture()
{
    if (data != nullptr) {
        stbi_image_free(data);
    }
    width = 0, height = 0, bytes_per_scanline = 0;
}

Color3 Texture::value(const vec2& uv)const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (data == nullptr)
        return Color3(0, 1, 1);
    auto u = uv[0]; auto v = uv[1];
    // Clamp input texture coordinates to [0,1] x [1,0]
    u = float_clamp(u, 0.0, 1.0);
    v = float_clamp(1.0-v, 0.0,1.0);  // Flip V to image coordinates

    auto i = static_cast<int>(u * width);
    auto j = static_cast<int>(v * height);

    // Clamp integer mapping, since actual coordinates should be less than 1.0
    if (i >= width)  i = width - 1;
    if (j >= height) j = height - 1;

    i = std::max(i, 0);
    j = std::max(j, 0);

    const auto color_scale = 1.0 / 255.0;
    auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

    return Color3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}
