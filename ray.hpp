#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

class Ray {
public:
    Ray() {
    }
    Ray(const Point3 &origin, const Vec3 &direction, float time = 0.0)
        : origin_(origin), direction_(direction), time_(time) {
    }

    Point3 origin() const {
        return origin_;
    }
    Vec3 direction() const {
        return direction_;
    }
    float time() const {
        return time_;
    }

    Point3 at(const float t) const {
        return origin_ + t * direction_;
    }

public:
    Point3 origin_;
    Vec3 direction_;
    float time_;
};

#endif