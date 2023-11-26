#include "lab3.hpp"
#include "utils.hpp"
#include <cstdint>

namespace lc3 {
namespace verifier {

uint16_t lab3Result(std::string str1, std::string str2) {
  for (unsigned int i = 0; i < std::min(str1.length(), str2.length()); i++) {
    if (str1[i] != str2[i]) {
        return str1[i] - str2[i];
    }
  }
  if (str1.length() < str2.length()) {
    return -str2[str1.length()];
  } else if (str1.length() > str2.length()) {
    return str1[str2.length()];
  }

  return 0;
}

int lab3Test(lc3::sim &simulator, const std::string &obj_filename,
              std::vector<std::string> testInput) {
  uint32_t passed_count = 0;
  for (auto in : testInput) {
    // reset simulator
    sim_start(simulator, obj_filename);
    // set input
    std::string in1 = in.substr(0, in.find(":"));
    std::string in2 = in.substr(in.find(":") + 1);


    for (unsigned int i = 0; i < in1.length(); i++) {
      simulator.writeMem(0x3100 + i, in1[i]);
    }

    for (unsigned int i = 0; i < in2.length(); i++) {
      simulator.writeMem(0x3200 + i, in2[i]);
    }

    // get expected result
    int16_t expected = lab3Result(in1, in2);

    // start simulator
    simulator.runUntilHalt();

    // check result
    int16_t result = simulator.readMem(0x3300);

    passed_count += check_result(in, expected, result, true);
  }
  return passed_count;
}
} // namespace verifier
} // namespace lc3