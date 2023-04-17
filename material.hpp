#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.hpp"
#include "rtweekend.hpp"
#include <cmath>

struct hit_record;

// an abstract material class that encapsulates behavior
// 1. Produce a scattered ray (or say it absorbed the incident ray)
// 2. If scattered, say how much the ray should be attenuated
class material {
public:
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material {
public:
    lambertian(const color &a) : albedo(a) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         color &attenuation, ray &scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    color albedo;
};

class metal : public material {
public:
    metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         color &attenuation, ray &scattered) const override {
        const vec3 reflected =
            reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_shpere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    color albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         color &attenuation, ray &scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        const double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        const vec3 unit_direction = unit_vector(r_in.direction());
        const double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        const double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract)
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);

        return true;
    }

public:
    double ir;
};

#endif