#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <memory>
#include <ostream>
#include <utility>

#include "aarectangle.hpp"
#include "box.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "constant_medium.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "moving_sphere.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"

class Material;

HittableList cornell_smoke() {
    HittableList objects;

    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

    objects.add(make_shared<YZRectangle>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<YZRectangle>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<XZRectangle>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<XZRectangle>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<XZRectangle>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<XYRectangle>(0, 555, 0, 555, 555, white));

    shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<translate>(box1, Vec3(265, 0, 295));

    shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<translate>(box2, Vec3(130, 0, 65));

    objects.add(make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
    objects.add(make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

    return objects;
}

HittableList cornell_box() {
    HittableList objects;

    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    objects.add(make_shared<YZRectangle>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<YZRectangle>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<XZRectangle>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<XZRectangle>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<XZRectangle>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<XYRectangle>(0, 555, 0, 555, 555, white));

    shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<translate>(box1, Vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<translate>(box2, Vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}

HittableList simple_light() {
    HittableList objects;

    auto pertext = make_shared<NoiseTexture>(4);
    objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    objects.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(Color(4, 4, 4));
    objects.add(make_shared<XYRectangle>(3, 5, 1, 3, -2, difflight));

    return objects;
}

inline HittableList mars() {
    auto mars_texture = std::make_shared<ImageTexture>("MarsTopoMap.jpg");
    auto mars_surface = make_shared<Lambertian>(mars_texture);
    auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, mars_surface);

    return HittableList(globe);
}

inline HittableList earth() {
    auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
    auto earth_surface = make_shared<Lambertian>(earth_texture);
    auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);

    return HittableList(globe);
}
inline HittableList two_perlin_spheres() {
    HittableList objects;

    auto perlin_texture = make_shared<NoiseTexture>(random_int(5, 10));
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

// Color(189.0 / 255.0, 195.0 / 255.0, 199.0 / 255.0)

inline Color ray_color(const Ray &r, const Color &background, const Hittable &world, int depth) {
    HitRecord record;

    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return Color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 10e-3, infinity, record))
        return background;

    Ray scattered;
    Color attenuation;
    Color emitted = record.material_pointer->emitted(record.u, record.v, record.p);

    if (!record.material_pointer->scatter(r, record, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

inline void test() {

    // Image
    std::ofstream image_out("img.ppm");

    float aspect_ratio = 1.0;
    constexpr int image_width = 400;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 200;
    int samples_max_depth = 50;

    auto image_data = new Vec3[image_height][image_width];

    // World

    HittableList world;

    Point3 look_from;
    Point3 look_at;
    float vertical_view_field = 40.0;
    float aperture = 0.0;
    Color background(0, 0, 0);

    switch (0) {
    case 1:
        world = random_scene();
        background = Color(0.70, 0.80, 1.00);
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        aperture = 0.1;
        break;

    case 2:
        world = two_spheres();
        background = Color(0.70, 0.80, 1.00);
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        break;

    case 3:
        world = two_perlin_spheres();
        background = Color(0.70, 0.80, 1.00);
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        break;

    case 4:
        world = mars();
        background = Color(0.70, 0.80, 1.00);
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        break;

    case 5:
        world = earth();
        background = Color(0.70, 0.80, 1.00);
        look_from = Point3(13, 2, 3);
        look_at = Point3(0, 0, 0);
        vertical_view_field = 20.0;
        break;

    case 6:
        world = simple_light();
        background = Color(0, 0, 0);
        look_from = Point3(26, 3, 6);
        look_at = Point3(0, 2, 0);
        vertical_view_field = 20.0;
        break;

    case 7:
        world = cornell_box();
        aspect_ratio = 1.0;
        background = Color(0, 0, 0);
        look_from = Point3(278, 278, -800);
        look_at = Point3(278, 278, 0);
        vertical_view_field = 40.0;
        break;

    default:
    case 8:
        world = cornell_smoke();
        look_from = Point3(278, 278, -800);
        look_at = Point3(278, 278, 0);
        vertical_view_field = 40.0;
        break;
    }

    // Camera

    const Vec3 vertical_up(0, 1, 0);
    constexpr float distance_to_focus = 10.0;

    Camera camera(look_from, look_at, vertical_up, vertical_view_field, aspect_ratio, aperture,
                  distance_to_focus, 0.0, 1.0);

// Render
#pragma omp parallel for schedule(auto)
    for (int y = image_height - 1; y >= 0; --y) {
        std::cerr << "\nScanlines ramaining: " << y << ' ' << std::flush;

        for (int x = 0; x < image_width; ++x) {
            Color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                float u = (x + random_float()) / (image_width - 1);
                float v = (y + random_float()) / (image_height - 1);
                Ray r = camera.get_ray(u, v);
                pixel_color += ray_color(r, background, world, samples_max_depth);
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
