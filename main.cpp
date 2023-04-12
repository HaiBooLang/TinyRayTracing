#include <Windows.h>
#include <fstream>
#include <iostream>
#include <ostream>

void test() {
  std::ofstream img("img.ppm");

  constexpr int image_width = 256;
  constexpr int image_height = 256;

  img << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\nScanlines ramaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      auto r = double(i) / (image_width - 1);
      auto g = double(j) / (image_height - 1);
      auto b = 0.5;

      int ir = static_cast<int>(255.999 * r);
      int ig = static_cast<int>(255.999 * g);
      int ib = static_cast<int>(255.999 * b);

      img << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
  std::cerr << "\nDone\n";

  img.close();
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