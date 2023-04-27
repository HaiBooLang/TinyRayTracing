#ifndef COLOR_H
#define COLOR_H

#include "rtweekend.hpp"
#include <ostream>

inline void write_color(std::ostream &image_out, Color &&pixel_color, int samples_per_pixel) {
    float r = pixel_color.x();
    float g = pixel_color.y();
    float b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0
    const float scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component
    image_out << static_cast<unsigned short>(255.999 * clamp(r, 0.0, 0.999)) << ' '
              << static_cast<unsigned short>(255.999 * clamp(g, 0.0, 0.999)) << ' '
              << static_cast<unsigned short>(255.999 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif