#include "lab4.hpp"
#include "utils.hpp"
#include <cstdint>
#include <vector>

namespace lc3 {
namespace verifier {

uint16_t put(uint16_t n, uint16_t state, std::vector<uint16_t> &expected);

int remove(uint16_t n, uint16_t state, std::vector<uint16_t> &expected) {
  if (n == 0) {
    return state;
  }
  if (n == 1) {
    expected.emplace_back(state | 1);
    return state | 1;
  }
  uint16_t state1 = remove(n - 2, state, expected) | (1 << (n - 1));
  expected.emplace_back(state1);
  return remove(n - 1, put(n - 2, state1, expected), expected);
}

uint16_t put(uint16_t n, uint16_t state, std::vector<uint16_t> &expected) {
  if (n == 0) {
    return state;
  }
  if (n == 1) {
    expected.emplace_back(state & 0xFFFE);
    return state & 0xFFFE;
  }
  uint16_t state1 = remove(n - 2, put(n - 1, state, expected), expected) & (0xFFFE << (n - 1));
  expected.emplace_back(state1);
  return put(n - 2, state1, expected);
}

int lab4Test(lc3::sim &simulator, const std::string &obj_filename,
              std::vector<std::string> testInput) {
  uint32_t passed_count = 0;
  for (auto in : testInput) {
    // reset simulator
    sim_start(simulator, obj_filename);
    // set input
    uint16_t inNum = std::stoi(in, nullptr, 10);
    simulator.writeMem(0x3100, inNum);


    // get expected result
    std::vector<uint16_t> expected;
    remove(inNum, 0, expected);

    // start simulator
    simulator.runUntilHalt();

    // check result
    int i = 0;
    std::vector<uint16_t> result;
    while (simulator.readMem(0x3101 + i) != 0) {
        result.emplace_back(simulator.readMem(0x3101 + i));
        i++;
    }
    
    passed_count += check_result(in, expected, result);
  }
  return passed_count;
}
} // namespace verifier
} // namespace lc3