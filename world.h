#pragma once

#include "aarectangle.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "rtweekend.h"
#include "sphere.h"

inline HittableList final_scene() {
    HittableList boxes1;
    auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_float(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<Box>(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    HittableList objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
    objects.add(make_shared<XZRectangle>(123, 423, 147, 412, 554, light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
    objects.add(make_shared<Sphere>(Point3(0, 150, 145), 50,
                                    make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
    objects.add(make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

    auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
    objects.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<NoiseTexture>(0.1);
    objects.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<Sphere>(Point3::random(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<RotateY>(make_shared<bvh_node>(boxes2, 0.0, 1.0), 15), Vec3(-100, 270, 395)));

    return objects;
}

inline HittableList cornell_smoke() {
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

inline HittableList cornell_box() {
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

inline HittableList simple_light() {
    HittableList world;

    auto pertext = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    world.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(Color(4, 4, 4));
    world.add(make_shared<XYRectangle>(3, 5, 1, 3, -2, difflight));

    return world;
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
    HittableList world;

    auto perlin_texture = make_shared<NoiseTexture>(random_int(5, 10));
    world.add(
        make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(perlin_texture)));
    world.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(perlin_texture)));

    return world;
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
    HittableList world;

    auto checker = make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    world.add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    return world;
}