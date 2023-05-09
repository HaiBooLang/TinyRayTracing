#ifndef TEXTURE_H
#define TEXTURE_H

#include "perlin.hpp"
#include "rtweekend.hpp"

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

#endif