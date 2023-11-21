#include "lab_verifier/lab2.hpp"
#include <bitset>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <vector>
#include <chrono>

#define API_VER 2
#include "common.h"
#include "console_inputter.h"
#include "console_printer.h"
#include "file_printer.h"
#include "interface.h"
#include "lab_verifier/verifiers.hpp"

struct CLIArgs {
  uint32_t print_level = DEFAULT_PRINT_LEVEL;
  bool enable_liberal_asm = false;
  uint32_t id_last_num = 0;
  std::string log_file = "";
  std::string test_input_file = "";
  uint32_t lab_id = 0;
};

bool endsWith(std::string const &search, std::string const &suffix) {
  if (suffix.size() > search.size()) {
    return false;
  }
  return std::equal(suffix.rbegin(), suffix.rend(), search.rbegin());
}

std::vector<std::string> loadTestSet(const CLIArgs &args) {
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

void labTestWrapper(std::string obj_filename, const CLIArgs &args) {
  std::shared_ptr<lc3::utils::IPrinter> iprinter;
  if (args.log_file != "") {
    iprinter = std::make_shared<lc3::FilePrinter>(args.log_file);
  } else {
    iprinter = std::make_shared<lc3::ConsolePrinter>();
  }

  lc3::ConsoleInputter inputter;
  lc3::sim simulator(*iprinter, inputter, args.print_level); 
  auto testInput = loadTestSet(args);

  int passed_count = 0;
  switch (args.lab_id) {
    case 1:
      passed_count = lc3::verifier::lab1Test(simulator, obj_filename, testInput);
      break;
    case 2:
      passed_count = lc3::verifier::lab2Test(simulator, obj_filename, testInput);
      break;
    default:
      std::cerr << "Error: lab id " << args.lab_id << " not supported." << std::endl;
      exit(1);
  }

  std::cout << "Passed " << passed_count << " out of " << testInput.size()
            << " test cases." << std::endl;
}

int main(int argc, char *argv[]) {
  CLIArgs args;
  std::vector<std::pair<std::string, std::string>> parsed_args =
      parseCLIArgs(argc, argv);
  for (auto const &arg : parsed_args) {
    if (std::get<0>(arg) == "id") {
      args.id_last_num = std::get<1>(arg)[std::get<1>(arg).size() - 1] - '0';
    } else if (std::get<0>(arg) == "enable-liberal-asm") {
      args.enable_liberal_asm = true;
    } else if (std::get<0>(arg) == "print-level") {
      args.print_level = std::stoi(std::get<1>(arg));
    } else if (std::get<0>(arg) == "log") {
      args.log_file = std::get<1>(arg);
    } else if (std::get<0>(arg) == "test-input") {
      args.test_input_file = std::get<1>(arg);
    } else if (std::get<0>(arg) == "lab") {
      args.lab_id = std::stoi(std::get<1>(arg));
    } else if (std::get<0>(arg) == "h" || std::get<0>(arg) == "help") {
      std::cout << "usage: " << argv[0] << " [OPTIONS] FILE [FILE...]\n";
      std::cout << "\n";
      std::cout << "  -h,--help              Print this message\n";
      std::cout << "  --id=PB......          Student ID\n";
      std::cout << "  --lab=N                Lab ID\n";
      std::cout << "  --test-input=FILE      Test set file\n";
      std::cout << "  --print-level=N        Output verbosity [0-9]\n";
      std::cout << "  --enable-liberal-asm   Enable liberal assembly mode\n";
      std::cout << "  --log=FILE             Log to FILE\n";

      return 0;
    }
  }

  lc3::ConsolePrinter printer;
  lc3::as assembler(printer, args.print_level, args.enable_liberal_asm);
  lc3::conv converter(printer, args.print_level);

  std::string obj_filename;
  for (int i = 1; i < argc; i += 1) {
    std::string filename(argv[i]);
    if (filename[0] != '-') {
      if (endsWith(filename, ".bin")) {
        converter.convertBin(filename);
      } else {
        assembler.assemble(filename);
      }
      obj_filename = std::string(filename.substr(0, filename.find_last_of('.')) + ".obj");
      std::cout << "Compiled file: " << obj_filename << std::endl;
      break;
    }
  }

  labTestWrapper(obj_filename, args);
}