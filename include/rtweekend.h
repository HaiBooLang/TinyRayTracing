#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>

// usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// constants

constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float PI = 3.1415926535897932385;

// utility functions

inline float degrees_to_radians(float degrees) {
    return degrees * PI / 180.0;
}

// clamp

inline float clamp(float x, float min, float max) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

// common headers

#include "ray.h"
#include "vec3.h"

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_float(min, max + 1));
}
#endif