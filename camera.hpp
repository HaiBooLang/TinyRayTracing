#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.hpp"

// implements a simple camera using the axis-aligned camera
class Camera {
public:
    Camera(Point3 look_from, Point3 look_at, Vec3 vertical_up,
           float vertical_view_field , // vertical field-of-view in degrees
           float aspect_ratio, float aperture, float focus_dist, float time0 = 0.0,
           float time1 = 0.0) {

        const float theta = degrees_to_radians(vertical_view_field );
        const float h = tan(theta / 2);
        const float viewport_height = 2.0 * h;
        const float viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vertical_up, w));
        v = cross(w, u);

        origin_ = look_from;
        horizontal_ = focus_dist * viewport_width * u;
        vertical_ = focus_dist * viewport_height * v;
        lower_left_corner_ = origin_ - horizontal_ / 2 - vertical_ / 2 - focus_dist * w;

        lens_radius_ = aperture / 2;
        time0_ = time0;
        time1_ = time1;
    }
    Ray get_ray(float s, float t) const {
        Vec3 rd = lens_radius_ * random_in_unit_disk();
        Vec3 offset = u * rd.x() + v * rd.y();

        return Ray(origin_ + offset,
                   lower_left_corner_ + s * horizontal_ + t * vertical_ - origin_ - offset,
                   random_float(time0_, time1_));
    }

private:
    Point3 origin_;
    Point3 lower_left_corner_;
    Vec3 horizontal_;
    Vec3 vertical_;
    Vec3 u, v, w;
    float lens_radius_;
    float time0_, time1_; // shutter open/close times
};
#endif