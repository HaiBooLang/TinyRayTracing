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

class translate : public Hittable {
public:
    translate(shared_ptr<Hittable> p, const Vec3 &displacement) : ptr(p), offset(displacement) {}

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override;

public:
    shared_ptr<Hittable> ptr;
    Vec3 offset;
};

inline bool translate::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    Ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

inline bool translate::bounding_box(float time0, float time1, aabb &output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = aabb(output_box.aabb_min() + offset, output_box.aabb_max() + offset);

    return true;
}

class RotateY : public Hittable {
public:
    RotateY(shared_ptr<Hittable> p, float angle);

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override {
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<Hittable> ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    aabb bbox;
};

inline RotateY::RotateY(shared_ptr<Hittable> p, float angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    Point3 rotatey_min(infinity, infinity, infinity);
    Point3 rotatey_max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i * bbox.aabb_max().x() + (1 - i) * bbox.aabb_min().x();
                auto y = j * bbox.aabb_max().y() + (1 - j) * bbox.aabb_min().y();
                auto z = k * bbox.aabb_max().z() + (1 - k) * bbox.aabb_min().z();

                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                Vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    rotatey_min[c] = fmin(rotatey_min[c], tester[c]);
                    rotatey_max[c] = fmax(rotatey_max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(rotatey_min, rotatey_max);
}

inline bool RotateY::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    Ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.p;
    auto normal = rec.normal;

    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}

#endif