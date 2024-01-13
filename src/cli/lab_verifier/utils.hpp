#ifndef __LAB_UTILS__
#define __LAB_UTILS__

#define API_VER 2
#include "interface.h"
#include "console_inputter.h"
#include "console_inputter.h"
#include "console_printer.h"
#include "../file_printer.h"
#include "interface.h"
#include "verifiers.hpp"
#include <bitset>

struct CLIArgs {
    uint32_t print_level = 3;
    bool enable_liberal_asm = false;
    uint32_t id_last_num = 0;
    std::string log_file = "";
    std::string test_input_file = "";
    uint32_t lab_id = 0;
    std::string stu_folder = "";
    bool result_in_output = false;
};

inline void sim_start(lc3::sim &simulator, const std::string &obj_filename) {
  // reset simulator
  simulator.zeroState();

  // set start simulator
  simulator.loadObjFile(obj_filename);
  
  simulator.setRunInstLimit(1000000);
}

inline int check_result(const std::string &in, uint16_t expected, uint16_t result, bool sign) {
  if (sign) {
    int16_t s_expected = static_cast<int16_t>(expected);
    int16_t s_result = static_cast<int16_t>(result);
    if (s_expected == s_result) {
      std::cout << "Test case " << in << " passed, got: " << s_result << std::endl;
      return 1;
    } else {
      std::cout << "Test case " << in << " failed," << " Expected: " << s_expected << ", got: " << s_result << std::endl;
      return 0;
    }
  }
  if (expected == result) {
    std::cout << "Test case " << in << " passed, got: " << result << std::endl;
    return 1;
  } else {
    std::cout << "Test case " << in << " failed," << " Expected: " << expected << ", got: " << result << std::endl;
    return 0;
  }

}

inline int check_result(const std::string &in, std::vector<uint16_t> &expected, std::vector<uint16_t> &result) {
  if (expected == result) {
    std::cout << "Test case " << in << " passed, got: " << std::endl;
    for (auto i : result) {
      std::cout << std::bitset<16>(i) << std::endl;
    }
    std::cout << std::endl;
    return 1;
  } else {
    std::cout << "Test case " << in << " failed," << " Expected: " << std::endl;
    for (auto i : expected) {
      std::cout << std::bitset<16>(i) << std::endl;
    }
    std::cout << std::endl << " got: " << std::endl;
    for (auto i : result) {
      std::cout << std::bitset<16>(i) << std::endl;
    }
    std::cout << std::endl;
    return 0;
  }
}

inline std::vector<std::string> loadTestSet(const CLIArgs &args) {
  // load test input from file
  const std::string &test_input_file = args.test_input_file;
  std::ifstream ifs(test_input_file);
  if (!ifs.is_open()) {
    std::cerr << "Error: cannot open test input file: " << test_input_file
              << std::endl;
    exit(1);
  }
  std::vector<std::string> testInput;
  std::string line;
  while (std::getline(ifs, line)) {
    testInput.push_back(line);
  }
  ifs.close();
  return testInput;
}

inline double labTestWrapper(std::string obj_filename, const CLIArgs &args) {
    std::shared_ptr<lc3::utils::IPrinter> iprinter;
    if (args.log_file != "") {
        iprinter = std::make_shared<lc3::FilePrinter>(args.log_file);
    } else {
        iprinter = std::make_shared<lc3::ConsolePrinter>();
    }

    StringInputter inputter;
    lc3::sim simulator(*iprinter, inputter, args.print_level);
    std::vector<std::string> testInput;
    if (args.lab_id != 6) {
        testInput = loadTestSet(args);
    }

    int passed_count = 0;
    switch (args.lab_id) {
    case 1:
        passed_count = lc3::verifier::lab1Test(simulator, obj_filename,
                                               testInput, args.id_last_num);
        break;
    case 2:
        passed_count =
            lc3::verifier::lab2Test(simulator, obj_filename, testInput);
        break;
    case 3:
        passed_count =
            lc3::verifier::lab3Test(simulator, obj_filename, testInput);
        break;
    case 4:
        passed_count =
            lc3::verifier::lab4Test(simulator, obj_filename, testInput);
        break;
    case 6:
        passed_count =
            lc3::verifier::lab6Test(simulator, obj_filename, inputter);
        break;
    default:
        std::cerr << "Error: lab id " << args.lab_id << " not supported."
                  << std::endl;
        exit(1);
    }

    std::cout << "-------------------------"
              << std::endl << "Passed " << passed_count << " out of " << testInput.size()
              << " test cases." << std::endl;
    return (double)passed_count / testInput.size();
}


#endif