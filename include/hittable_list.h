#ifndef HittableList_H
#define HittableList_H

#include "aabb.h"
#include "hittable.h"
#include <vector>

class HittableList : public Hittable {
public:
    HittableList() {
    }
    HittableList(std::shared_ptr<Hittable> object) {
        add(object);
    }

    void clear() {
        objects.clear();
    }
    void add(std::shared_ptr<Hittable> object) {
        objects.push_back(object);
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override;

public:
    std::vector<std::shared_ptr<Hittable>> objects;
};

inline bool HittableList::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    // z-buffer
    auto closest_so_far = t_max;

    for (const auto &object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

inline bool HittableList::bounding_box(float time0, float time1, aabb &output_box) const {
    if (objects.empty())
        return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto &object : objects) {
        if (!object->bounding_box(time0, time1, temp_box))
            return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif