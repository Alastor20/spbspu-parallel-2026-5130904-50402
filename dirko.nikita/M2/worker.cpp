#include "worker.hpp"
#include <chrono>
#include <cstddef>
#include <ratio>
#include <unistd.h>
#include "../common/area.hpp"
#include "../common/shapes.hpp"

namespace dirko {

  size_t take(int &err, int rd, void *buf, size_t k)
  {
    char *const p = static_cast< char * >(buf);
    size_t r = 0;
    while (r < k) {
      err = read(rd, p + r, k - r);
      if (err <= 0) {
        break;
      }
      r += err;
    }
    return r;
  }

  size_t send(int &err, int wr, const void *buf, size_t k)
  {
    const char *const p = static_cast< const char * >(buf);
    size_t r = 0;
    while (r < k) {
      err = write(wr, p + r, k - r);
      if (err <= 0) {
        break;
      }
      r += err;
    }
    return r;
  }

  void runWorker(int input, int output, size_t seed)
  {
    while (true) {
      size_t data[4] = {};
      int err = 0;
      take(err, input, data, sizeof(data));
      if (err <= 0) {
        break;
      }

      shapes_t shapes(data[3]);
      if (!shapes.empty()) {
        err = 0;
        take(err, input, shapes.data(), shapes.size() * sizeof(shape_t));
        if (err <= 0) {
          break;
        }
      }

      const auto start = std::chrono::high_resolution_clock::now();
      const double area_val = area(shapes, data[1], data[2], seed + data[0]).second;
      const auto end = std::chrono::high_resolution_clock::now();
      const double ms = std::chrono::duration< double, std::milli >(end - start).count();
      const Result result{data[0], ms, area_val};
      err = 0;
      send(err, output, &result, sizeof(result));
      if (err <= 0) {
        break;
      }
    }
  }
}
