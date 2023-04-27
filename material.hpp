#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.hpp"
#include "rtweekend.hpp"
#include "texture.hpp"

struct HitRecord;

// an abstract material class that encapsulates behavior
// 1. Produce a scattered ray (or say it absorbed the incident ray)
// 2. If scattered, say how much the ray should be attenuated
class Material {
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
                         Ray &scattered) const = 0;
};

class Lambertian : public Material {
public:
    explicit Lambertian(const Color &albedo) : albedo_(make_shared<SolidColor>(albedo)) {}
    explicit Lambertian(shared_ptr<Texture> a) : albedo_(a) {}
    
    virtual bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
                         Ray &scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo_->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<Texture> albedo_;
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

#endif