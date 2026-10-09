#include <iostream>
#include <vector>

namespace dirko
{
  struct point_t
  {
    double x, y;
  };

  struct Circle
  {
    double radius;
    point_t position;
    Circle(size_t radius, point_t position):
      radius(radius),
      position(position)
    {}
  };
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
  while (std::cin >> radius) {
    std::cin >> placeHolder >> x >> y;
    shapes.push_back(dirko::Circle(radius, dirko::point_t{x, y}));
  }
}
