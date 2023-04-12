#include "color.hpp"
#include "vec3.hpp"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <ostream>


void test() {

  // Image

  std::ofstream img_out("img.ppm");

  constexpr int image_width = 256;
  constexpr int image_height = 256;

  // Render

  img_out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\nScanlines ramaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      auto r = double(i) / (image_width - 1);
      auto g = double(j) / (image_height - 1);
      auto b = 0.5;

      color pixel_color(r, g, b);
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