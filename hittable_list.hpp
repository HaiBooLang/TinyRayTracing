#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.hpp"
#include "hittable.hpp"
#include <vector>

class hittable_list : public hittable {
public:
    hittable_list() {
    }
    hittable_list(std::shared_ptr<hittable> object) {
        add(object);
    }

    void clear() {
        objects.clear();
    }
    void add(std::shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb &output_box) const override;

public:
    std::vector<std::shared_ptr<hittable>> objects;
};

inline bool hittable_list::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    hit_record temp_rec;
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

inline bool hittable_list::bounding_box(float time0, float time1, aabb &output_box) const {
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