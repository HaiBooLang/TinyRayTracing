#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "rtweekend.hpp"

#include "hittable.hpp"

class moving_sphere : public hittable {
public:
    moving_sphere() {}
    moving_sphere(point3 cen0, point3 cen1, float _time0, float _time1, float r,
                  shared_ptr<material> m)
        : center0(cen0), center1(cen1), time0(_time0), time1(_time1), radius(r),
          mat_ptr(m){};

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const;

    point3 center(float time) const;

public:
    point3 center0, center1;
    float time0, time1;
    float radius;
    shared_ptr<material> mat_ptr;
};

inline point3 moving_sphere::center(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}
inline bool moving_sphere::hit(const ray &r, float t_min, float t_max,
                               hit_record &rec) const {
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    auto outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif