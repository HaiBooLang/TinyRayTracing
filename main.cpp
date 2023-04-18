#include "rtweekend.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>


class material;

inline void write_img(std::ostream &out, double pixel_color[3],
                      int samples_per_pixel) {
    const double scale = 1.0 / samples_per_pixel;
    const double r = sqrt(scale * pixel_color[0]);
    const double g = sqrt(scale * pixel_color[1]);
    const double b = sqrt(scale * pixel_color[2]);

    out << static_cast<int>(255.999 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(255.999 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(255.999 * clamp(b, 0.0, 0.999)) << '\n';
}

inline hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2,
                          b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

inline color ray_color(const ray &r, const hittable &world, int depth) {
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

    constexpr double aspect_ratio = 3.0 / 2.0;
    constexpr int image_width = 1200;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int samples_per_pixel = 100;
    constexpr int max_depth = 50;
    auto img_tmp = new double[image_height][image_width][3]{0};

    // World
    auto world = random_scene();

    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture,
               dist_to_focus);

    // Render
    img_out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

#pragma omp parallel
    {
#pragma omp single
        {
            for (int j = image_height - 1; j >= 0; --j) {
                std::cerr << "\nScanlines ramaining: " << j << ' '
                          << std::flush;
                {
                    for (int i = 0; i < image_width; ++i) {
                        color pixel_color(0, 0, 0);
#pragma omp task
                        {

                            for (int s = 0; s < samples_per_pixel; ++s) {
                                auto u =
                                    (i + random_double()) / (image_width - 1);
                                auto v =
                                    (j + random_double()) / (image_height - 1);
                                ray r = cam.get_ray(u, v);
                                pixel_color += ray_color(r, world, max_depth);
                            }

                            img_tmp[j][i][0] = pixel_color.x();
                            img_tmp[j][i][1] = pixel_color.y();
                            img_tmp[j][i][2] = pixel_color.z();
                        }
                    }
                }
            }
        }
    }

    for (int i = image_height - 1; i >= 0; --i) {
        for (int j = 0; j < image_width; ++j) {
            write_img(img_out, img_tmp[i][j], samples_per_pixel);
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

// const auto material_lambertian_grey = std::make_shared<lambertian>(
//     color(44.0 / 255.0, 62.0 / 255.0, 80.0 / 255.0));
// const auto material_lambertian_yellow = make_shared<lambertian>(
//     color(251.0 / 255.0, 215.0 / 255.0, 134.0 / 255.0));
// const auto material_metal_green = make_shared<metal>(
//     color(198.0 / 255.0, 255.0 / 255.0, 221.0 / 255.0), 0.1);
// const auto material_matel_red = make_shared<metal>(
//     color(247.0 / 255.0, 121.0 / 255.0, 125.0 / 255.0), 0.9);
// const auto material_dielectric_greater_one =
// make_shared<dielectric>(1.1); const auto material_dielectric_less_one =
// make_shared<dielectric>(0.9);

// const auto material_ground = material_lambertian_grey;
// const auto material_left_outside = material_dielectric_greater_one;
// const auto material_left_inside = material_dielectric_less_one;
// const auto material_center = material_lambertian_yellow;
// const auto material_right = material_metal_green;

// world.add(
//     make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0,
//     material_ground));
// world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5,
//                               material_left_outside));
// world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4,
//                               material_left_inside));
// world.add(
//     make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
// world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5,
// material_right));