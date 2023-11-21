#ifndef __LAB_UTILS__
#define __LAB_UTILS__

#include "interface.h"

inline void sim_start(lc3::sim &simulator, const std::string &obj_filename) {
  // reset simulator
  simulator.zeroState();

  // set start PC
  simulator.loadObjFile(obj_filename);
  simulator.writePC(0x3000);
  simulator.setRunInstLimit(100000);
}

#endif