#include "./utils.hpp"
#include "interface.h"
#include <cstdint>

namespace lc3 {
namespace verifier {
uint16_t lab1Result(uint16_t input) {
  uint16_t result = 0;
  if (input % 2 == 0) {
    input = (~input) + 1;
  }
  for (int _ = 0; _ < 16; _++) {
    result += (input & 1) == 0 ? 1 : 0;
    input = input >> 1;
  }
  return result;
}

int lab1Test(lc3::sim &simulator, const std::string &obj_filename,
             std::vector<std::string> testInput) {
  uint32_t passed_count = 0;
  for (auto in : testInput) {
    // reset simulator
    sim_start(simulator, obj_filename);
    simulator.writePC(0x3000);

    // set input
    uint16_t inNum = std::stoi(in, nullptr, 10);
    simulator.writeMem(0x3100, inNum);
    // set defualt student id to 10
    simulator.writeMem(0x3101, 0xA);

    // get expected result
    auto expected = lab1Result(inNum);

    // start simulator
    simulator.runUntilHalt();

    // check result
    auto student_id = simulator.readMem(0x3101);
    auto result = simulator.readMem(0x3102);
    if (expected + student_id == result) {
      std::cout << "Test case: " << inNum << " passed, "
                << "Student ID: " << student_id << ", "
                << "Expected: " << expected + student_id << ", Got: " << result
                << std::endl;
      passed_count++;
    } else {
      std::cout << "Test case: " << inNum << " failed, "
      << "Student ID: " << student_id << ", "
      << "Expected: " << expected + student_id << ", Got: " << result
                << std::endl;
    }
  }
  return passed_count;
}
} // namespace verifier
} // namespace lc3