#ifndef SHAPES_HPP
#define SHAPES_HPP
#include <cstddef>
namespace dirko {
  struct point_t {
    double x, y;
  };
  struct borders_t {
    dirko::point_t max, min;
  };
  enum class ShapeType { CIRCLE, ELLIPSE };
  class Shape {
  public:
    virtual ~Shape() = default;
    virtual bool isInside(point_t) const noexcept = 0;
    virtual point_t getMinCorner() const noexcept = 0;
    virtual point_t getMaxCorner() const noexcept = 0;
    virtual ShapeType getShape() const noexcept = 0;
  };
  class Circle final: public Shape {
  public:
    Circle(size_t, point_t);
    bool isInside(point_t) const noexcept override;
    point_t getMaxCorner() const noexcept override;
    point_t getMinCorner() const noexcept override;
    ShapeType getShape() const noexcept override;
  private:
    double radius_;
    point_t position_;
  };
  class Ellipse final: public Shape {
  public:
    Ellipse(double, double, point_t) noexcept;
    bool isInside(point_t) const noexcept override;
    point_t getMinCorner() const noexcept override;
    point_t getMaxCorner() const noexcept override;
    ShapeType getShape() const noexcept override;

  private:
    double horizontal_radius_;
    double vertical_radius_;
    point_t position_;
  };
}
#endif
