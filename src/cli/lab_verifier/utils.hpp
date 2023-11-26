#ifndef __LAB_UTILS__
#define __LAB_UTILS__

#include "interface.h"

inline void sim_start(lc3::sim &simulator, const std::string &obj_filename) {
  // reset simulator
  simulator.zeroState();

  // set start simulator
  simulator.loadObjFile(obj_filename);
  
  simulator.setRunInstLimit(1000000);
}

inline int check_result(const std::string &in, uint16_t expected, uint16_t result) {
  if (expected == result) {
    std::cout << "Test case" << in << " passed," << " Expected: " << expected << ", got: " << result << std::endl;
    return 1;
  } else {
    std::cout << "Test case" << in << " failed," << " Expected: " << expected << ", got: " << result << std::endl;
    return 0;
  }
}

#endif