#ifndef TEXTURE_H
#define TEXTURE_H

#include "perlin.hpp"
#include "rtw_stb_image.hpp"
#include "rtweekend.hpp"
#include <iostream>

class Texture {
public:
    virtual Color value(float u, float v, const Point3 &p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor() {}
    explicit SolidColor(Color c) : color_value_(c) {}

    SolidColor(float red, float green, float blue) : SolidColor(Color(red, green, blue)) {}

    virtual Color value(float n, float v, const Vec3 &p) const override { return color_value_; }

private:
    Color color_value_;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture() {}

    CheckerTexture(shared_ptr<Texture> even, shared_ptr<Texture> odd) : even_(even), odd_(odd) {}

    CheckerTexture(Color c1, Color c2)
        : even_(make_shared<SolidColor>(c1)), odd_(make_shared<SolidColor>(c2)) {}

    virtual Color value(const float u, const float v, const Point3 &p) const {
        const float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd_->value(u, v, p);
        else
            return even_->value(u, v, p);
    }

public:
    shared_ptr<Texture> odd_;
    shared_ptr<Texture> even_;
};

class NoiseTexture : public Texture {
public:
    NoiseTexture() {}
    NoiseTexture(float _scale) : scale(_scale) {}

    virtual Color value(float u, float v, const Vec3 &p) const override {
        return Color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }

public:
    Perlin noise;
    float scale;
};

class ImageTexture : public Texture {
public:
    const static int bytes_per_pixel = 3;
    ImageTexture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    ImageTexture(const char *file_name) {
        int components_per_pixel = bytes_per_pixel;

        data = stbi_load(file_name, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Couldnot load texture image file '" << file_name << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~ImageTexture() { delete data; }

    virtual Color value(float u, float v, const Vec3 &p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr) {
            return Color(0, 1, 1);
        }

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0); // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)
            i = width - 1;
        if (j >= height)
            j = height - 1;

        const float color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;
};

#endif