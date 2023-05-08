#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <memory>
#include <ostream>
#include <utility>

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "moving_sphere.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"

class Material;

inline HittableList two_perlin_spheres() {
    HittableList objects;

    auto perlin_texture = make_shared<NoiseTexture>();
    objects.add(
        make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(perlin_texture)));
    objects.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(perlin_texture)));

    return objects;
}

inline HittableList random_scene() {
    HittableList world;

    const auto checker = make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_material = random_float();
            Point3 sphere_center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if ((sphere_center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_material < 0.6) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = make_shared<Lambertian>(albedo);
                    auto center2 = sphere_center + Vec3(0, random_float(0, 0.5), 0);
                    world.add(make_shared<moving_sphere>(sphere_center, center2, 0.0, 1.0, 0.2,
                                                         sphere_material));
                } else if (choose_material < 0.85) {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(sphere_center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(sphere_center, 0.2, sphere_material));
                }
            }
        }
    }

    const auto material_front =
        make_shared<Metal>(Color(198.0 / 255.0, 255.0 / 255.0, 221.0 / 255.0), 0.1);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material_front));

    const auto material_middle = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material_middle));

    const auto material_behind =
        make_shared<Lambertian>(Color(247.0 / 255.0, 121.0 / 255.0, 125.0 / 255.0));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material_behind));

    return world;
}

inline HittableList two_spheres() {
    HittableList objects;

    auto checker = make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    objects.add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
    objects.add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    return objects;
}

inline Color ray_color(const Ray &r, const Hittable &world, int depth) {
    HitRecord record;

    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return Color(0, 0, 0);

    // world
    if (world.hit(r, 10e-3, infinity, record)) {
        Ray scattered;
        Color attenuation;
        if (record.material_pointer->scatter(r, record, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return Color(0, 0, 0);
    }

    // background
    Vec3 unit_direction = unit_vector(r.direction());
    const auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) +
           t * Color(189.0 / 255.0, 195.0 / 255.0, 199.0 / 255.0);
}

inline void test() {

    // Image
    std::ofstream image_out("img.ppm");

    constexpr float aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 1200;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    constexpr int samples_per_pixel = 100;
    constexpr int samples_max_depth = 50;

    auto image_data = new Vec3[image_height][image_width];

    // World

    HittableList world;

    Point3 look_from;
    Point3 look_at;
    auto vertical_view_field = 40.0;
    auto aperture = 0.0;

    switch (0) {
    case 1:
        world = random_scene();
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        aperture = 0.1;
        break;

    case 2:
        world = two_spheres();
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        break;
    default:
    case 3:
        world = two_perlin_spheres();
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        break;
    }

    // Camera

    const Vec3 vertical_up(0, 1, 0);
    constexpr float distance_to_focus = 10.0;

    Camera camera(look_from, look_at, vertical_up, vertical_view_field, aspect_ratio, aperture,
                  distance_to_focus, 0.0, 1.0);

    // Render
    // #pragma omp parallel for schedule(dynamic)
    for (int y = image_height - 1; y >= 0; --y) {
        std::cerr << "\nScanlines ramaining: " << y << ' ' << std::flush;

        for (int x = 0; x < image_width; ++x) {
            Color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                float u = (x + random_float()) / (image_width - 1);
                float v = (y + random_float()) / (image_height - 1);
                Ray r = camera.get_ray(u, v);
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
