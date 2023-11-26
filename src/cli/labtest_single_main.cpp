#include <ostream>
#include <string>
#include <sys/types.h>
#include <vector>

#define API_VER 2
#include "common.h"
#include "console_inputter.h"
#include "console_printer.h"
#include "file_printer.h"
#include "interface.h"
#include "lab_verifier/utils.hpp"


bool endsWith(std::string const &search, std::string const &suffix) {
  if (suffix.size() > search.size()) {
    return false;
  }
  return std::equal(suffix.rbegin(), suffix.rend(), search.rbegin());
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