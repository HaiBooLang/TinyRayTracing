#include <Windows.h>
#include <fstream>
#include <iomanip>
#include <memory>
#include <ostream>
#include <thread>


#include "aarectangle.h"
#include "box.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "rtweekend.h"
#include "sphere.h"
#include "world.h"


class Material;


// Color(189.0 / 255.0, 195.0 / 255.0, 199.0 / 255.0)

Color ray_color(const Ray& r, const Color& background, const Hittable& world, int depth) {
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
	int samples_max_depth = 64;

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

	case 8:
		world = cornell_smoke();
		look_from = Point3(278, 278, -800);
		look_at = Point3(278, 278, 0);
		vertical_view_field = 40.0;
		break;

	default:
	case 9:
		world = final_scene();
		background = Color(0, 0, 0);
		look_from = Point3(478, 278, -600);
		look_at = Point3(278, 278, 0);
		vertical_view_field = 40.0;
		break;
	}

	// Camera

	const Vec3 vertical_up(0, 1, 0);
	constexpr float distance_to_focus = 10.0;

	Camera camera(look_from, look_at, vertical_up, vertical_view_field, aspect_ratio, aperture,
		distance_to_focus, 0.0, 1.0);

	int n = image_height;

	const int num_threads = std::thread::hardware_concurrency() - 1;
	std::vector<std::thread> threads(num_threads);

	for (int i = 0; i < num_threads; ++i) {
		int start_y = i * image_height / num_threads;
		int end_y = (i + 1) * image_height / num_threads;

		threads[i] = std::thread([start_y, end_y, image_width, image_height, samples_per_pixel, &camera, &background, &world, samples_max_depth, &image_data, &n]() {
			for (int y = end_y - 1; y >= start_y; --y) {
				std::cerr << '\n' << n-- << std::flush;

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
			});
	}

	for (auto& thread : threads) {
		thread.join();
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
