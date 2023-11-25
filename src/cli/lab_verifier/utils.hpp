#ifndef __LAB_UTILS__
#define __LAB_UTILS__

#include "interface.h"

inline void sim_start(lc3::sim &simulator, const std::string &obj_filename) {
  // reset simulator
  simulator.zeroState();

  // set start simulator
  simulator.loadObjFile(obj_filename);
  // simulator.setIgnorePrivilege(true);
  simulator.setRunInstLimit(1000000);
}

#endif