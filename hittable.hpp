#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.hpp"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    // Hittables and materials need to know each other
    shared_ptr<material> mat_ptr;
    float t;
    bool front_face;

    inline void set_face_normal(const ray &r, const vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// an “abstract class” for anything a ray might hit
class hittable {
public:
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const = 0;
};

#endif