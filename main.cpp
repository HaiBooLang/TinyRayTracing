#include "rtweekend.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include <Windows.h>
#include <fstream>
#include <memory>

color ray_color(const ray &r, const hittable &world) {
    hit_record rec;

    // world
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }

    // background
    vec3 unit_direction = unit_vector(r.direction());
    const auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.596, 0.765, 0.475);
}

void test() {

    // Image
    std::ofstream img_out("img.ppm");

    constexpr double aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int samples_per_pixel = 100;

    // World
    hittable_list world;
    for (int i = 9; i >= -9; --i) {
        for (int j = 5; j >= -5; --j) {
            world.add(make_shared<sphere>(point3(i, j, -2), 0.5));
        }
    }

    // Camera
    camera cam;

    // Render

    img_out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\nScanlines ramaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }
            write_color(img_out, pixel_color, samples_per_pixel);
        }
    }
    std::cerr << "\nDone\n";

    img_out.close();
}

int main() {

    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);

    double time = 0.0;
    constexpr double max = 1.0;

    for (int i = 0; i < max; ++i) {
        QueryPerformanceCounter(&t1);

        test();

        QueryPerformanceCounter(&t2);
        time += (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
    }

    std::cerr << "time = " << time / max << std::endl;
}
