#include "color.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <ostream>

bool hit_sphere(const point3 &center, const double radius, const ray &r) {
  vec3 oc = r.origin() - center;
  const auto a = dot(r.direction(), r.direction());
  const auto b = 2.0 * dot(oc, r.direction());
  const auto c = dot(oc, oc) - radius * radius;
  const auto discriminant = b * b - 4 * a * c;
  return (discriminant > 0);
}

color ray_color(const ray &r) {
  if (hit_sphere(point3(0, 0, -1), 0.5, r))
    return color(0.5, 0.7, 1.0);
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.596, 0.765, 0.475);
}

void test() {

  // Image

  std::ofstream img_out("img.ppm");

  constexpr double aspect_ratio = 16.0 / 9.0;
  constexpr int image_width = 400;
  constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

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
      const ray r{origin,
                  lower_left_corner + u * horizontal + v * vertical - origin};

      color pixel_color = ray_color(r);
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