#include "cmds.hpp"
#include <cstddef>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>
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

    std::vector< std::string > readArgs(std::istream &in)
    {
      std::string line;
      std::getline(in, line);

      std::vector< std::string > arguments;
      size_t i = 0;

      while (i < line.size()) {
        if (i < line.size() && std::isspace(line[i])) {
          ++i;
          continue;
        }

        const size_t start = i;

        while (i < line.size() && !std::isspace(line[i])) {
          ++i;
        }

        if (start < i) {
          arguments.push_back(line.substr(start, i - start));
        }
      }

      return arguments;
    }
    size_t parseSize(const std::string &token)
    {
      if (token.empty() || token[0] == '-') {
        throw std::invalid_argument("invalid");
      }

      std::size_t pos = 0;
      const size_t value = std::stoull(token, &pos, 10);
      if (pos != token.size()) {
        throw std::invalid_argument("invalid");
      }
      return value;
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

  void cmdSpawn(std::istream &in, std::ostream &, Data &data)
  {
    std::vector< std::string > args = readArgs(in);
    if (args.size() != 2) {
      throw std::invalid_argument("invalid");
    }

    const std::string &name = args[0];
    const size_t seed = parseSize(args[1]);

    if (data.workers.count(name)) {
      throw std::invalid_argument("invalid");
    }

    int pipe_in[2] = {-1, -1};
    int pipe_out[2] = {-1, -1};

    if (pipe(pipe_in) < 0) {
      throw std::invalid_argument("pipe");
    }

    if (pipe(pipe_out) < 0) {
      close(pipe_in[0]);
      close(pipe_in[1]);
      throw std::invalid_argument("pipe");
    }

    const int flags = fcntl(pipe_out[0], F_GETFL, 0);
    if (flags < 0 || fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK) < 0) {
      close(pipe_in[0]);
      close(pipe_in[1]);
      close(pipe_out[0]);
      close(pipe_out[1]);
      throw std::runtime_error("set flag");
    }

    const pid_t pid = fork();

    if (pid < 0) {
      close(pipe_in[0]);
      close(pipe_in[1]);
      close(pipe_out[0]);
      close(pipe_out[1]);
      throw std::invalid_argument("fork");
    }

    if (pid == 0) {
      close(pipe_in[1]);
      close(pipe_out[0]);

      for (const auto &kv : data.workers) {
        if (kv.second.input >= 0) {
          close(kv.second.input);
        }
        if (kv.second.output >= 0) {
          close(kv.second.output);
        }
      }

      runWorker(pipe_in[0], pipe_out[1], seed);
      _exit(0);
    }

    close(pipe_in[0]);
    close(pipe_out[1]);

    Worker w;
    w.pid = pid;
    w.input = pipe_in[1];
    w.output = pipe_out[0];
    data.workers[name] = w;
  }
}
