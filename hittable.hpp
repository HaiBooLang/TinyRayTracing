#ifndef Hittable_H
#define Hittable_H

#include "aabb.hpp"
#include "rtweekend.hpp"

class Material;

struct HitRecord {
    Point3 p;
    Vec3 normal;
    // Hittables and materials need to know each other
    shared_ptr<Material> material_pointer;
    float t;
    float u;
    float v;
    bool front_face;

    inline void set_face_normal(const Ray &r, const Vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// an “abstract class” for anything a ray might hit
class Hittable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
    virtual bool bounding_box(float time0, float time1, aabb &output_box) const = 0;
};

#endif