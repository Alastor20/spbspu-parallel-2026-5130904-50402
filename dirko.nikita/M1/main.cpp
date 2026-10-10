#include <cstddef>
#include <future>
#include <iostream>
#include <random>
#include <thread>
#include <utility>
#include <vector>

namespace dirko {
  struct point_t {
    double x, y;
  };

  struct Circle {
    double radius;
    point_t position;
    Circle(size_t radius, point_t position):
      radius(radius),
      position(position)
    {}
    bool isInside(point_t p) const noexcept
    {
      return (p.x - position.x) * (p.x - position.x) + (p.y - position.y) * (p.y - position.y) <= radius * radius;
    }
  };
  using circles_t = std::vector< Circle >;
  std::pair< size_t, size_t > calculate(const circles_t &circles, point_t min, point_t max, size_t tests, size_t seed)
  {
    std::default_random_engine gen(seed);
    std::uniform_real_distribution< double > distX(min.x, max.x);
    std::uniform_real_distribution< double > distY(min.y, max.y);
    size_t intersectionCount = 0;
    size_t combinationCount = 0;
    for (size_t i = 0; i < tests; i++) {
      bool isInsideAny = false;
      bool isInsideAll = true;
      point_t p{distX(gen), distY(gen)};
      for (const Circle &circle : circles) {
        if (circle.isInside(p)) {
          isInsideAny = true;
        } else {
          isInsideAll = false;
        }
      }
      if (isInsideAll) {
        intersectionCount++;
      }
      if (isInsideAny) {
        combinationCount++;
      }
    }
    return {intersectionCount, combinationCount};
  }
  std::pair< point_t, point_t > getBorders(const circles_t &circles)
  {
    const double inf = std::numeric_limits< double >::infinity();
    point_t max{-inf, -inf};
    point_t min{inf, inf};
    for (const Circle &circle : circles) {
      const point_t circleMax = {circle.position.x + circle.radius, circle.position.y + circle.radius};
      const point_t circleMin = {circle.position.x - circle.radius, circle.position.y - circle.radius};
      max.x = std::max(max.x, circleMax.x);
      max.y = std::max(max.y, circleMax.y);
      min.x = std::min(min.x, circleMin.x);
      min.y = std::min(min.y, circleMin.y);
    }
    return {max, min};
  }

  std::pair< double, double > area(const circles_t &circles, size_t threads, size_t tests, size_t seed)
  {
    if (!tests) {
      throw std::invalid_argument("zero tests");
    }
    if (circles.empty()) {
      return {0.0, 0.0};
    }
    if (threads == 0) {
      threads = 1;
    }
    const size_t maxThreads = std::thread::hardware_concurrency();
    if (threads > maxThreads) {
      threads = maxThreads;
    }
    std::vector< std::future< std::pair< size_t, size_t > > > futures;
    const size_t testsPerThread = tests / threads;
    const size_t remainder = tests % threads;
    const std::pair< point_t, point_t > border = getBorders(circles);
    for (size_t i = 0; i < threads; i++) {
      const size_t part = i < remainder ? testsPerThread + 1 : testsPerThread;
      futures.push_back(
          std::async(std::launch::async, calculate, std::cref(circles), border.first, border.second, part, seed + i));
    }
    size_t inters = 0;
    size_t covers = 0;
    for (auto &future : futures) {
      const std::pair< size_t, size_t > result = future.get();
      inters += result.first;
      covers += result.second;
    }
    const double bordrArea = (border.first.x - border.second.x) * (border.first.y - border.second.y);
    return {bordrArea * (static_cast< double >(covers) / tests), bordrArea * (static_cast< double >(inters) / tests)};
  }
}
int main(int argc, char **argv)
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Invalid parameters\n";
    return 1;
  }
  size_t threads = 0;
  size_t tries = 0;
  size_t seed = 0;
  try {
    if (argv[1][0] == '-' || argv[2][0] == '-') {
      std::cerr << "Negative arguments\n";
      return 1;
    }
    threads = std::stoull(argv[1]);
    tries = std::stoull(argv[2]);
    if (argc == 4) {
      if (argv[3][0] == '-') {
        std::cerr << "Negative seed\n";
        return 1;
      }
      seed = std::stoull(argv[3]);
    }
  } catch (const std::invalid_argument &) {
    std::cerr << "Not a number in arguments\n";
    return 1;
  } catch (const std::out_of_range &) {
    std::cerr << "Overflow in arguments\n";
    return 1;
  }
  std::vector< dirko::Circle > shapes;
  double radius = 0;
  double placeHolder = 0;
  double x = 0;
  double y = 0;
  while (std::cin >> radius >> placeHolder >> x >> y) {
    shapes.push_back(dirko::Circle(radius, dirko::point_t{x, y}));
  }
  if (!std::cin.eof()) {
    std::cerr << "Invalid input\n";
    return 1;
  }
}
