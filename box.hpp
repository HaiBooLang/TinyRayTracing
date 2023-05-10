#ifndef BOX_H
#define BOX_H

#include "rtweekend.hpp"

#include "aarectangle.hpp"
#include "hittable_list.hpp"

class Box : public Hittable {
public:
    Box() {}
    Box(const Point3 &p0, const Point3 &p1, shared_ptr<Material> ptr);

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override {
        output_box = aabb(box_min, box_max);
        return true;
    }

public:
    Point3 box_min;
    Point3 box_max;
    HittableList sides;
};

inline Box::Box(const Point3 &p0, const Point3 &p1, shared_ptr<Material> ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(make_shared<XYRectangle>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(make_shared<XYRectangle>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    sides.add(make_shared<XZRectangle>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(make_shared<XZRectangle>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    sides.add(make_shared<YZRectangle>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(make_shared<YZRectangle>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

inline bool Box::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    return sides.hit(r, t_min, t_max, rec);
}

#endif