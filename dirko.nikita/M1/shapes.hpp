#ifndef SHAPES_HPP
#define SHAPES_HPP
#include <cstddef>
#include <memory>
#include <vector>
namespace dirko {
  struct point_t {
    double x, y;
  };
  class Shape {
  public:
    virtual ~Shape() = default;
    virtual bool isInside(point_t) const noexcept = 0;
    virtual point_t getMinCorner() const noexcept = 0;
    virtual point_t getMaxCorner() const noexcept = 0;
  };
  class Circle final: public Shape {
  public:
    Circle(size_t radius, point_t position);
    bool isInside(point_t) const noexcept;
    point_t getMaxCorner() const noexcept;
    point_t getMinCorner() const noexcept;
  private:
    double radius_;
    point_t position_;
  };
  using shapes_t = std::vector< std::unique_ptr< Shape > >;
}
#endif
