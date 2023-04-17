#include "rtweekend.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include <Windows.h>
#include <fstream>
#include <memory>

class material;

color ray_color(const ray &r, const hittable &world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return color(0, 0, 0);

    // world
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    // background
    vec3 unit_direction = unit_vector(r.direction());
    const auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) +
           t * color(189.0 / 255.0, 195.0 / 255.0, 199.0 / 255.0);
}

void test() {

    // Image
    std::ofstream img_out("img.ppm");

    constexpr double aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 800;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int samples_per_pixel = 100;
    constexpr int max_depth = 50;

    // World
    hittable_list world;

    const auto material_lambertian_grey = std::make_shared<lambertian>(
        color(44.0 / 255.0, 62.0 / 255.0, 80.0 / 255.0));
    const auto material_lambertian_yellow = make_shared<lambertian>(
        color(251.0 / 255.0, 215.0 / 255.0, 134.0 / 255.0));
    const auto material_metal_green = make_shared<metal>(
        color(198.0 / 255.0, 255.0 / 255.0, 221.0 / 255.0), 0.1);
    const auto material_matel_red = make_shared<metal>(
        color(247.0 / 255.0, 121.0 / 255.0, 125.0 / 255.0), 0.9);
    const auto material_dielectric_greater_one = make_shared<dielectric>(1.1);
    const auto material_dielectric_less_one = make_shared<dielectric>(0.9);

    const auto material_ground = material_lambertian_grey;
    const auto material_left_outside = material_dielectric_greater_one;
    const auto material_left_inside = material_dielectric_less_one;
    const auto material_center = material_lambertian_yellow;
    const auto material_right = material_metal_green;

    world.add(
        make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5,
                                  material_left_outside));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4,
                                  material_left_inside));
    world.add(
        make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // Camera
    camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 90,
               aspect_ratio);

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
                pixel_color += ray_color(r, world, max_depth);
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
