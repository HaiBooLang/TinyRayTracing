#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.hpp"

// implements a simple camera using the axis-aligned camera
class Camera {
public:
    Camera(point3 lookfrom, point3 lookat, vec3 vup,
           float vfov, // vertical field-of-view in degrees
           float aspect_ratio, float aperture, float focus_dist) {
            
        const float theta = degrees_to_radians(vfov);
        const float h = tan(theta / 2);
        const float viewport_height = 2.0 * h;
        const float viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        m_origin = lookfrom;
        m_horizontal = focus_dist * viewport_width * u;
        m_vertical = focus_dist * viewport_height * v;
        m_lower_left_corner =
            m_origin - m_horizontal / 2 - m_vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
    }
    ray get_ray(float s, float t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();

        return ray(m_origin + offset, m_lower_left_corner + s * m_horizontal +
                                          t * m_vertical - m_origin - offset);
    }

private:
    point3 m_origin;
    point3 m_lower_left_corner;
    vec3 m_horizontal;
    vec3 m_vertical;
    vec3 u, v, w;
    float lens_radius;
};
#endif