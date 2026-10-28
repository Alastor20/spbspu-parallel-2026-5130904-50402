#include "cmds.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
#include "../common/shapes.hpp"

namespace dirko {

  namespace {
    std::ostream &operator<<(std::ostream &out, dirko::point_t p)
    {
      return out << "(" << p.x << " " << p.y << ")";
    }

    void printShape(std::ostream &out, const dirko::shape_t &sh)
    {
      using namespace dirko;
      if (sh.type == shapeType::CIRCLE) {
        out << sh.rx << " " << sh.position << "\n";
      } else {
        out << sh.rx << " " << sh.ry << " " << sh.position << "\n";
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
    for (const auto &s : it->second) {
      printShape(out, data.shapes.at(s));
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
    const point_t mn = getMinCorner(it->second);
    const point_t mx = getMaxCorner(it->second);
    out << mn << " " << mx << "\n";
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

    point_t mn = getMinCorner(data.shapes.at(it->second.front()));
    point_t mx = getMaxCorner(data.shapes.at(it->second.front()));
    for (const auto &s : it->second) {
      const shape_t &sh = data.shapes.at(s);
      const point_t a = getMinCorner(sh);
      const point_t b = getMaxCorner(sh);
      mn.x = std::min(mn.x, a.x);
      mn.y = std::min(mn.y, a.y);
      mx.x = std::max(mx.x, b.x);
      mx.y = std::max(mx.y, b.y);
    }
    out << mn << " " << mx << "\n";
  }

}
