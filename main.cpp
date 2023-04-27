#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <utility>

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "moving_sphere.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"

class material;

inline hittable_list random_scene() {
    hittable_list world;

    const auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_material = random_float();
            point3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_material < 0.6) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_float(0, 0.5), 0);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                         sphere_material));
                } else if (choose_material < 0.85) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    const auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    const auto material2 =
        make_shared<lambertian>(color(247.0 / 255.0, 121.0 / 255.0, 125.0 / 255.0));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    const auto material3 =
        make_shared<metal>(color(198.0 / 255.0, 255.0 / 255.0, 221.0 / 255.0), 0.1);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

inline color ray_color(const ray &r, const hittable &world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return color(0, 0, 0);

    // world
    if (world.hit(r, 10e-3, infinity, rec)) {
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

inline void test() {

    // Image
    std::ofstream image_out("img.ppm");

    constexpr float aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 100;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int samples_per_pixel = 66;
    constexpr int samples_max_depth = 30;

    auto image_data = new vec3[image_height][image_width];

    // World
    auto world = random_scene();

    // Camera
    const point3 lookfrom(13, 2, 3);
    const point3 lookat(0, 0, 0);
    const vec3 vup(0, 1, 0);
    const float dist_to_focus = 10.0;
    const float aperture = 0.05;

    Camera camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // Render
#pragma omp parallel for schedule(dynamic)
    for (int y = image_height - 1; y >= 0; --y) {
        std::cerr << "\nScanlines ramaining: " << y << ' ' << std::flush;

        for (int x = 0; x < image_width; ++x) {
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                float u = (x + random_float()) / (image_width - 1);
                float v = (y + random_float()) / (image_height - 1);
                ray r = camera.get_ray(u, v);
                pixel_color += ray_color(r, world, samples_max_depth);
            }

            image_data[y][x] = std::move(pixel_color);
        }
    }

    image_out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int y = image_height - 1; y >= 0; --y) {
        for (int x = 0; x < image_width; ++x) {
            write_color(image_out, std::move(image_data[y][x]), samples_per_pixel);
        }
    }

    std::cerr << "\nDone\n";

    delete[] image_data;

    image_out.close();
}

int main() {

    LARGE_INTEGER time_start, time_end, tc;
    QueryPerformanceFrequency(&tc);

    float time = 0.0;
    constexpr int max = 1;

    for (int i = 0; i < max; ++i) {
        QueryPerformanceCounter(&time_start);

        test();

        QueryPerformanceCounter(&time_end);
        time += static_cast<float>(time_end.QuadPart - time_start.QuadPart) /
                static_cast<float>(tc.QuadPart);
    }

    std::cerr << "time = " << time / static_cast<float>(max) << std::endl;
}
