#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.hpp"
#include "rtweekend.hpp"
#include "texture.hpp"
#include <memory>

struct HitRecord;

// an abstract material class that encapsulates behavior
// 1. Produce a scattered ray (or say it absorbed the incident ray)
// 2. If scattered, say how much the ray should be attenuated
class Material {
public:
    virtual Color emitted(float u, float v, const Point3 &p) const { return Color(0, 0, 0); }

    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
                         Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
    explicit Lambertian(const Color &_albedo) : albedo(make_shared<SolidColor>(_albedo)) {}
    explicit Lambertian(shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(const Ray &_r_in, const HitRecord &_rec, Color &_attenuation,
                         Ray &_scattered) const override {
        auto scatter_direction = _rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = _rec.normal;

        _scattered = Ray(_rec.p, scatter_direction, _r_in.time());
        _attenuation = albedo->value(_rec.u, _rec.v, _rec.p);
        return true;
    }

public:
    shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
    Metal(const Color &albedo, const float fuzz) : albedo_(albedo), fuzz_(fuzz < 1 ? fuzz : 1) {}

    virtual bool scatter(const Ray &ray_in, const HitRecord &record, Color &attenuation,
                         Ray &scattered) const override {
        const Vec3 reflected = reflect(unit_vector(ray_in.direction()), record.normal);
        scattered = Ray(record.p, reflected + fuzz_ * random_in_unit_shpere(), ray_in.time());
        attenuation = albedo_;
        return (dot(scattered.direction(), record.normal) > 0);
    }

public:
    Color albedo_;
    float fuzz_;
};

class Dielectric : public Material {
public:
    Dielectric(float refraction_index) : refraction_index_(refraction_index) {}

    virtual bool scatter(const Ray &ray_in, const HitRecord &record, Color &attenuation,
                         Ray &scattered) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        const float refraction_ratio =
            record.front_face ? (1.0 / refraction_index_) : refraction_index_;

        const Vec3 unit_direction = unit_vector(ray_in.direction());
        const float cos_theta = fmin(dot(-unit_direction, record.normal), 1.0);
        const float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
            direction = reflect(unit_direction, record.normal);
        else
            direction = refract(unit_direction, record.normal, refraction_ratio);

        scattered = Ray(record.p, direction, ray_in.time());

        return true;
    }

public:
    float refraction_index_; // Index of Refraction

private:
    static float reflectance(float cosine, float ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture> _emit) : emit(_emit) {}
    DiffuseLight(Color _color) : emit(make_shared<SolidColor>(_color)) {}

    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
                         Ray &scattered) const override {
        return false;
    }

    virtual Color emitted(float u, float v, const Point3 &p) const override {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<Texture> emit;
};

class Isotropic : public Material {
    public:
        Isotropic(Color c) : albedo(make_shared<SolidColor>(c)) {}
        Isotropic(shared_ptr<Texture> a) : albedo(a) {}

        virtual bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override {
            scattered = Ray(rec.p, random_in_unit_shpere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<Texture> albedo;
};




#endif