#ifndef AABB_H
#define AABB_H

#include "rtweekend.hpp"

class aabb {
public:
    aabb() {
    }
    aabb(const point3 &a, const point3 &b) {
        aabb_minimum = a;
        aabb_maximum = b;
    }

    point3 aabb_min() const {
        return aabb_minimum;
    }
    point3 aabb_max() const {
        return aabb_maximum;
    }

    bool hit(const ray &r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            auto t0 = fmin((aabb_minimum[a] - r.origin()[a]) / r.direction()[a],
                           (aabb_maximum[a] - r.origin()[a]) / r.direction()[a]);
            auto t1 = fmax((aabb_minimum[a] - r.origin()[a]) / r.direction()[a],
                           (aabb_maximum[a] - r.origin()[a]) / r.direction()[a]);
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

public:
    point3 aabb_minimum;
    point3 aabb_maximum;
};

inline aabb surrounding_box(aabb box0, aabb box1) {
    point3 small_box(fmin(box0.aabb_min().x(), box1.aabb_min().x()),
                     fmin(box0.aabb_min().y(), box1.aabb_min().y()),
                     fmin(box0.aabb_min().z(), box1.aabb_min().z()));

    point3 big_box(fmax(box0.aabb_max().x(), box1.aabb_max().x()),
                   fmax(box0.aabb_max().y(), box1.aabb_max().y()),
                   fmax(box0.aabb_max().z(), box1.aabb_max().z()));

    return aabb(small_box, big_box);
}
#endif