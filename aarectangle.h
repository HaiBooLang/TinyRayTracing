#ifndef AARECTANGLE_H
#define AARECTANGLE_H

#include "rtweekend.h"

#include "hittable.h"
#include <memory>

class XYRectangle : public Hittable {
public:
    XYRectangle() {}

    XYRectangle(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Z dimension
        // a small amount.
        output_box = aabb(Point3(x0, y0, k - (10e-3)), Point3(x1, y1, k + (10e-3)));
        return true;
    }

public:
    shared_ptr<Material> mp;
    float x0, x1, y0, y1, k;
};

inline bool XYRectangle::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    float t = (k - r.origin().z()) / r.direction().z();
    if (t < t_min || t > t_max)
        return false;
    auto x = r.origin().x() + t * r.direction().x();
    auto y = r.origin().y() + t * r.direction().y();
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    auto outward_normal = Vec3(0, 0, 1);
    rec.set_face_normal(r, outward_normal);
    rec.material_pointer = mp;
    rec.p = r.at(t);
    return true;
}

class XZRectangle : public Hittable {
public:
    XZRectangle() {}

    XZRectangle(float _x0, float _x1, float _z0, float _z1, float _k, shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat){};

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Y
        // dimension a small amount.
        output_box = aabb(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
        return true;
    }

public:
    shared_ptr<Material> mp;
    float x0, x1, z0, z1, k;
};

inline bool XZRectangle::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    auto t = (k - r.origin().y()) / r.direction().y();
    if (t < t_min || t > t_max)
        return false;
    auto x = r.origin().x() + t * r.direction().x();
    auto z = r.origin().z() + t * r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outward_normal = Vec3(0, 1, 0);
    rec.set_face_normal(r, outward_normal);
    rec.material_pointer = mp;
    rec.p = r.at(t);
    return true;
}

class YZRectangle : public Hittable {
public:
    YZRectangle() {}

    YZRectangle(float _y0, float _y1, float _z0, float _z1, float _k, shared_ptr<Material> mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat){};

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override {
        // The bounding box must have non-zero width in each dimension, so pad the X
        // dimension a small amount.
        output_box = aabb(Point3(k - (10e-3), y0, z0), Point3(k + (10e-3), y1, z1));
        return true;
    }

public:
    shared_ptr<Material> mp;
    float y0, y1, z0, z1, k;
};

inline bool YZRectangle::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    auto t = (k - r.origin().x()) / r.direction().x();
    if (t < t_min || t > t_max)
        return false;
    auto y = r.origin().y() + t * r.direction().y();
    auto z = r.origin().z() + t * r.direction().z();
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outward_normal = Vec3(1, 0, 0);
    rec.set_face_normal(r, outward_normal);
    rec.material_pointer = mp;
    rec.p = r.at(t);
    return true;
}

#endif
