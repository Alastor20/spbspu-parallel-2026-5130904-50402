#ifndef AREA_HPP
#define AREA_HPP
#include <cstddef>
#include <utility>
#include "shapes.hpp"
namespace dirko {
  std::pair< double, double > area(const shapes_t &shapes, size_t threads, size_t tests, size_t seed);
}
#endif
