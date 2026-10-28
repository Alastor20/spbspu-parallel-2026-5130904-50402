#include "cmds.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <iostream>
#include "../common/shapes.hpp"

namespace dirko {

  namespace {
    std::ostream &operator<<(std::ostream &out, dirko::point_t point)
    {
      return out << "(" << point.x << " " << point.y << ")";
    }

    void printShape(std::ostream &out, const dirko::shape_t &shape)
    {
      using namespace dirko;
      if (shape.type == shapeType::CIRCLE) {
        out << shape.rx << " " << shape.position << "\n";
      } else {
        out << shape.rx << " " << shape.ry << " " << shape.position << "\n";
      }
    }
  }

  void cmdCircle(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    double r = 0.0, x = 0.0, y = 0.0;
    if (!(in >> name >> r >> x >> y)) {
      throw std::invalid_argument("invalid");
    }
    if (data.shapes.count(name)) {
      throw std::invalid_argument("invalid");
    }
    if (r <= 0) {
      throw std::invalid_argument("circle");
    }
    data.shapes[name] = {shapeType::CIRCLE, r, r, {x, y}};
  }

  void cmdSet(std::istream &in, std::ostream &, Data &data)
  {
    std::string name;
    size_t n = 0;
    if (!(in >> name >> n)) {
      throw std::invalid_argument("invalid");
    }
    if (n == 0) {
      throw std::invalid_argument("invalid");
    }
    if (data.sets.count(name)) {
      throw std::invalid_argument("invalid");
    }
    Set s;
    s.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      std::string shape;
      if (!(in >> shape)) {
        throw std::invalid_argument("invalid");
      }
      if (!data.shapes.count(shape)) {
        throw std::invalid_argument("invalid");
      }
      s.push_back(std::move(shape));
    }
    data.sets[name] = std::move(s);
  }

  void cmdShow(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.shapes.find(name);
    if (it == data.shapes.end()) {
      throw std::invalid_argument("invalid");
    }
    printShape(out, it->second);
  }

  void cmdShowSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.sets.find(name);
    if (it == data.sets.end()) {
      throw std::invalid_argument("invalid");
    }
    for (const std::string &shape_name : it->second) {
      printShape(out, data.shapes.at(shape_name));
    }
  }

  void cmdFrame(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.shapes.find(name);
    if (it == data.shapes.end()) {
      throw std::invalid_argument("invalid");
    }
    const point_t min_point = getMinCorner(it->second);
    const point_t max_point = getMaxCorner(it->second);
    out << min_point << " " << max_point << "\n";
  }

  void cmdFrameSet(std::istream &in, std::ostream &out, Data &data)
  {
    std::string name;
    if (!(in >> name)) {
      throw std::invalid_argument("invalid");
    }
    auto it = data.sets.find(name);
    if (it == data.sets.end()) {
      throw std::invalid_argument("invalid");
    }

    point_t min_point = getMinCorner(data.shapes.at(it->second.front()));
    point_t max_point = getMaxCorner(data.shapes.at(it->second.front()));
    for (const std::string &shape_name : it->second) {
      const shape_t &shape = data.shapes.at(shape_name);
      const point_t a = getMinCorner(shape);
      const point_t b = getMaxCorner(shape);
      min_point.x = std::min(min_point.x, a.x);
      min_point.y = std::min(min_point.y, a.y);
      max_point.x = std::max(max_point.x, b.x);
      max_point.y = std::max(max_point.y, b.y);
    }
    out << min_point << " " << max_point << "\n";
  }

}
