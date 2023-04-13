#include "rtweekend.hpp"

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

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    constexpr double viewport_height = 2.0;
    constexpr double viewport_width = aspect_ratio * viewport_height;
    constexpr double focal_length = 1.0;

    const point3 origin{0, 0, 0};
    const vec3 horizontal{viewport_width, 0, 0};
    const vec3 vertical{0, viewport_height, 0};
    const vec3 lower_left_corner =
        origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    // Render

    img_out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\nScanlines ramaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            const double u = double(i) / (image_width - 1);
            const double v = double(j) / (image_height - 1);
            const ray r{origin, lower_left_corner + u * horizontal +
                                    v * vertical - origin};

            color pixel_color = ray_color(r, world);
            write_color(img_out, pixel_color);
        }
    }
    std::cerr << "\nDone\n";

    img_out.close();
}

int main() {

    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);

    double time = 0.0;
    constexpr double max = 5.0;

    for (int i = 0; i < max; ++i) {
        QueryPerformanceCounter(&t1);

        test();

        QueryPerformanceCounter(&t2);
        time += (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
    }

    std::cerr << "time = " << time / max << std::endl;
}
