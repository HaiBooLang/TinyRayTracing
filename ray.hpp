#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

class ray {
public:
    ray() {
    }
    ray(const point3 &origin, const vec3 &direction, float time = 0.0)
        : orig(origin), dir(direction), tm(time) {
    }

    point3 origin() const {
        return orig;
    }
    vec3 direction() const {
        return dir;
    }
    float time() const {
        return tm;
    }

    point3 at(float t) const {
        return orig + t * dir;
    }

public:
    point3 orig;
    vec3 dir;
    float tm;
};

#endif