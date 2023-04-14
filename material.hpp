#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.hpp"

struct hit_record;

// an abstract material class that encapsulates behavior
// 1. Produce a scattered ray (or say it absorbed the incident ray)
// 2. If scattered, say how much the ray should be attenuated
class material {
public:
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         color &attenuation, ray &scattered) const = 0;
};

#endif