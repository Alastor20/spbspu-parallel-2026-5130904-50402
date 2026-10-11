#include "shapes.hpp"
#include <cstddef>

dirko::Circle::Circle(size_t radius, point_t position):
  radius_(radius),
  position_(position)
{}
bool dirko::Circle::isInside(point_t p) const noexcept
{
  return (p.x - position_.x) * (p.x - position_.x) + (p.y - position_.y) * (p.y - position_.y) <= radius_ * radius_;
}
dirko::point_t dirko::Circle::getMaxCorner() const noexcept
{
  return {position_.x + radius_, position_.y + radius_};
}
dirko::point_t dirko::Circle::getMinCorner() const noexcept
{
  return {position_.x - radius_, position_.y - radius_};
}
