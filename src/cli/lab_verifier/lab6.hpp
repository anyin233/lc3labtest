#ifndef __LAB6_HPP__
#define __LAB6_HPP__

#include "interface.h"
#include "input.hpp"

namespace lc3 {
    namespace verifier {
        int lab6Test(lc3::sim &simulator, const std::string &obj_filename, const std::string &stu_id, StringInputter &inputter);
    }
} // namespace lc3

#endif