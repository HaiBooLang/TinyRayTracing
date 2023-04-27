#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.hpp"
#include "material.hpp"
#include "vec3.hpp"

class Material;

class Sphere : public Hittable {
public:
    Sphere() {}
    Sphere(Point3 center, float radius, shared_ptr<Material> material_pointer)
        : center_(center), radius_(radius), material_pointer_(material_pointer){};

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override;

private:
    static void get_sphere_uv(const Point3 &point, float &u, float &v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
        const float theta = acos(-point.y());
        const float phi = std::atan2(-point.z(), point.x()) + PI;

        u = phi / (2 * PI);
        v = theta / PI;
    }

public:
    Point3 center_;
    float radius_;
    shared_ptr<Material> material_pointer_;
};

inline bool Sphere::hit(const Ray &ray, float t_min, float t_max, HitRecord &record) const {
    // t^2 b⋅b + 2tb⋅(A - C) + (A - C)⋅(A - C) - r^2 = 0
    Vec3 oc = ray.origin() - center_;
    const float a = ray.direction().length_squared();
    const float half_b = dot(oc, ray.direction());
    const float c = oc.length_squared() - radius_ * radius_;

    const float discriminant = half_b * half_b - a * c;
    if (discriminant < 10e-3)
        return false;
    const float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    // record
    record.t = root;
    record.p = ray.at(record.t);
    Vec3 outward_normal = (record.p - center_) / radius_;
    record.set_face_normal(ray, outward_normal);
    get_sphere_uv(outward_normal, record.u, record.v);
    record.material_pointer = material_pointer_;

    return true;
}

inline bool Sphere::bounding_box(float time0, float time1, aabb &output_box) const {
    output_box =
        aabb(center_ - Vec3(radius_, radius_, radius_), center_ + Vec3(radius_, radius_, radius_));
    return true;
}

#endif