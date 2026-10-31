#ifndef WORKER_HPP
#define WORKER_HPP
#include <cstddef>
#include <sys/types.h>

namespace dirko {
  struct Worker {
    pid_t pid = -1;
    int input = -1;
    int output = -1;
  };

  void runWorker(int input, int output, size_t seed);
}

#endif
