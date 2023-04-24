#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "material.hpp"
#include "vec3.hpp"

class material;

class sphere : public hittable {
public:
    sphere() {}
    sphere(point3 cen, float r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m){};

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const override;

public:
    point3 center;
    float radius;
    shared_ptr<material> mat_ptr;
};

inline bool sphere::hit(const ray &r, float t_min, float t_max,
                        hit_record &rec) const {
    // t^2 b⋅b + 2tb⋅(A - C) + (A - C)⋅(A - C) - r^2 = 0
    vec3 oc = r.origin() - center;
    const auto a = r.direction().length_squared();
    const auto half_b = dot(oc, r.direction());
    const auto c = oc.length_squared() - radius * radius;

    const auto discriminant = half_b * half_b - a * c;
    if (discriminant < 10e-3)
        return false;
    const auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    // record
    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif