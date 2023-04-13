#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "vec3.hpp"
#include <cmath>

class sphere : public hittable {
public:
    sphere() {}
    sphere(point3 cen, double r) : center(cen), radius(r) {}

    virtual bool hit(const ray &r, double t_min, double t_max,
                     hit_record &rec) const override;

public:
    point3 center;
    double radius;
};

bool sphere::hit(const ray &r, double t_min, double t_max,
                 hit_record &rec) const {
    vec3 oc = r.origin() - center;
    const auto a = r.direction().length_squared();
    const auto half_b = dot(oc, r.direction());
    const auto c = oc.length_squared() - radius * radius;

    const auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
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
    rec.normal = (rec.p - center)/radius;

    return true;
}

#endif