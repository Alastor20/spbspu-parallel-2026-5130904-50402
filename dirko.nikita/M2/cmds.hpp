#ifndef CMDS_HPP
#define CMDS_HPP

#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include "../common/shapes.hpp"

namespace dirko {
  using Set = std::vector< std::string >;

  struct Data {
    std::map< std::string, shape_t > shapes;
    std::map< std::string, Set > sets;
  };

  void cmdCircle(std::istream &, std::ostream &, Data &);
  void cmdSet(std::istream &, std::ostream &, Data &);
  void cmdShow(std::istream &, std::ostream &, Data &);
  void cmdShowSet(std::istream &, std::ostream &, Data &);
  void cmdFrame(std::istream &, std::ostream &, Data &);
  void cmdFrameSet(std::istream &, std::ostream &, Data &);
}
#endif
