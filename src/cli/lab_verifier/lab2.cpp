#include "lab2.hpp"
#include "utils.hpp"
#include <cstdint>

namespace lc3 {
namespace verifier {

uint16_t lab2Result(uint16_t N) {
  int result = 3;
  int b_n = true;
  for (int i = 1; i < N; i++) {
    if (b_n) {
      result = 2 * result + 2;
    } else {
      result = 2 * result - 2;
    }

    if (result % 8 == 0 || result % 10 == 8) {
      b_n = !b_n;
    }

    result = result % 4096;
  }

  return result;
}

void lab2Test(lc3::sim &simulator, const std::string &obj_filename,
              std::vector<std::string> testInput) {
  for (auto in : testInput) {
    // reset simulator
    sim_start(simulator, obj_filename);

    // set input
    uint16_t inNum = std::stoi(in, nullptr, 10);
    simulator.writeMem(0x3102, inNum);

    // get expected result
    auto expected = lab2Result(inNum);

    // start simulator
    simulator.run();

    // check result
    auto result = simulator.readMem(0x3103);
    if (expected == result) {
      std::cout << "Test case: " << in << " passed." << std::endl;
    } else {
      std::cout << "Test case: " << in << " failed." << std::endl;
      std::cout << "Expected: " << expected << ", got: " << result
                << std::endl;
    }
  }
}
} // namespace verifier
} // namespace lc3