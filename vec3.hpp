#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <random>

using std::sqrt;

inline float random_float() {
    // Returns a random real in [0,1)
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    static std::mt19937 generator(std::random_device{}());
    return distribution(generator);
}

inline float random_float(float min, float max) {
    // Returns a random real in [min,max)
    return min + (max - min) * random_float();
}

class Vec3 {
public:
    Vec3() : e{0, 0, 0} {
    }
    Vec3(float e0, float e1, float e2) : e{e0, e1, e2} {
    }

    float x() const {
        return e[0];
    }
    float y() const {
        return e[1];
    }
    float z() const {
        return e[2];
    }

    Vec3 operator-() const {
        return Vec3(-e[0], -e[1], -e[2]);
    }
    float operator[](int i) const {
        return e[i];
    }
    float &operator[](int i) {
        return e[i];
    }

    Vec3 &operator+=(const Vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3 &operator*=(const float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3 &operator/=(const float t) {
        return *this *= 1 / t;
    }

    float length() const {
        return sqrt(length_squared());
    }

    float length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline static Vec3 random() {
        return Vec3(random_float(), random_float(), random_float());
    }

    inline static Vec3 random(float min, float max) {
        return Vec3(random_float(min, max), random_float(min, max), random_float(min, max));
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions
        const float s = 1e-3;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

public:
    float e[3];
};

using Color = Vec3;
using Point3 = Vec3;

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

inline Vec3 operator*(float t, const Vec3 &v) {
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, float t) {
    return t * v;
}

inline Vec3 operator/(Vec3 v, float t) {
    return (1 / t) * v;
}

inline float dot(const Vec3 &u, const Vec3 &v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1], u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(Vec3 v) {
    return v / v.length();
}

inline Vec3 random_in_unit_shpere() {
    while (true) {
        auto p = Vec3::random(-1, 1);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

inline Vec3 random_unit_vector() {
    return unit_vector(random_in_unit_shpere());
}

inline Vec3 random_in_hemisphere(const Vec3 &normal) {
    Vec3 in_unit_sphere = random_in_unit_shpere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
    ;
}

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) {
    return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3 &uv, const Vec3 &n, const float etai_over_etat) {
    const auto cos_theta = fmin(dot(-uv, n), 1.0);
    const Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    const Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

inline Vec3 random_in_unit_disk() {
    while (true) {
        auto p = Vec3(random_float(-1, 1), random_float(-1, 1), 0);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

#endif