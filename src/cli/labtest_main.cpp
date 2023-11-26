#include <bitset>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
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
#include "lab_verifier/verifiers.hpp"

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

double labTestWrapper(std::string obj_filename, const CLIArgs &args) {
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
        passed_count = lc3::verifier::lab1Test(simulator, obj_filename,
                                               testInput, args.id_last_num);
        break;
    case 2:
        passed_count =
            lc3::verifier::lab2Test(simulator, obj_filename, testInput);
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

int main(int argc, char *argv[]) {
    CLIArgs args;
    std::vector<std::pair<std::string, std::string>> parsed_args =
        parseCLIArgs(argc, argv);
    for (auto const &arg : parsed_args) {
        if (std::get<0>(arg) == "id") {
            args.id_last_num =
                std::get<1>(arg)[std::get<1>(arg).size() - 1] - '0';
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
        } else if (std::get<0>(arg) == "stu_folder") {
            args.stu_folder = std::get<1>(arg);
        } else if (std::get<0>(arg) == "result-in-output") {
            args.result_in_output = true;
        } else if (std::get<0>(arg) == "h" || std::get<0>(arg) == "help") {
            std::cout << "usage: " << argv[0] << " [OPTIONS] FILE [FILE...]\n";
            std::cout << "\n";
            std::cout << "  -h,--help              Print this message\n";
            std::cout << "  --id=PB......          Student ID\n";
            std::cout << "  --lab=N                Lab ID\n";
            std::cout << "  --test-input=FILE      Test set file\n";
            std::cout << "  --print-level=N        Output verbosity [0-9]\n";
            std::cout
                << "  --enable-liberal-asm   Enable liberal assembly mode\n";
            std::cout << "  --log=FILE             Log to FILE\n";
            std::cout << "  --stu_folder=PATH      Path to student folder\n";
            std::cout
                << "  --result-in-output     Print result in output.txt\n";
            return 0;
        }
    }

    lc3::ConsolePrinter printer;
    lc3::as assembler(printer, args.print_level, args.enable_liberal_asm);
    lc3::conv converter(printer, args.print_level);

    if (args.stu_folder != "") {
        // get all sub directories
        std::vector<std::string> sub_dirs;
        for (auto &p : std::filesystem::directory_iterator(args.stu_folder)) {
            if (p.is_directory()) {
                sub_dirs.push_back(p.path().string());
            }
        }

        // get all files in each sub directory
        for (auto &sub_dir : sub_dirs) {
            std::cout << "Begin grading " << sub_dir << std::endl;

            std::vector<std::string> files;
            for (auto &p : std::filesystem::directory_iterator(sub_dir)) {
                if (p.is_regular_file()) {
                    files.push_back(p.path().string());
                }
            }
            // dirname: PBxxx_name_labN_逾期
            std::string stu_id = sub_dir.substr(0, sub_dir.find_first_of('_'));
// delete path
#ifdef _WIN32
            stu_id = stu_id.substr(stu_id.find_last_of('\\') + 1);
#else
            stu_id = stu_id.substr(stu_id.find_last_of('/') + 1);
#endif
            std::string name = sub_dir.substr(
                sub_dir.find_first_of('_') + 1,
                sub_dir.find_last_of('_') - sub_dir.find_first_of('_') - 1);
            bool is_late = sub_dir.find("逾期") != std::string::npos;
            args.id_last_num = std::stoi(stu_id.substr(stu_id.size() - 1));

            // create output.txt
            std::string output_file = sub_dir + "/output.txt";
            std::ofstream ofs(output_file);
            std::streambuf *oldcout;
            if (!ofs.is_open()) {
                std::cerr << "Error: cannot open output file: " << output_file
                          << std::endl;
                continue;
            }
            ofs << "Student ID: " << stu_id;
            ofs << " Name: " << name << std::endl;

            // find .bin/.asm file
            std::string bin_file, asm_file;
            for (auto &file : files) {
                if (endsWith(file, ".bin")) {
                    bin_file = file;
                } else if (endsWith(file, ".asm")) {
                    asm_file = file;
                }
            }
            if (bin_file == "" && asm_file == "") {
                ofs << "Files: " << std::endl;
                for (auto &file : files) {
                    ofs << file << std::endl;
                }
                ofs << "Error: cannot find .bin or .asm file" << std::endl;
                continue;
            }
            if (bin_file != "" && asm_file != "") {
                ofs << "Warning: both .bin and .asm file exist, using .bin file"
                    << std::endl;
            }
            // compile
            std::string obj_filename;
            if (bin_file != "") {
                converter.convertBin(bin_file);
                obj_filename = std::string(
                    bin_file.substr(0, bin_file.find_last_of('.')) + ".obj");
            } else {
                assembler.assemble(asm_file);
                obj_filename = std::string(
                    asm_file.substr(0, asm_file.find_last_of('.')) + ".obj");
            }
            // if .obj file not found
            if (!std::filesystem::exists(obj_filename)) {
                ofs << "Error: cannot find .obj file" << std::endl;
                continue;
            }

            if (args.result_in_output) {
                // 重定向cout到output.txt
                oldcout = std::cout.rdbuf(ofs.rdbuf());
            }

            double testpoints = 0.0;
            testpoints = 100 * labTestWrapper(obj_filename, args);

            if (args.result_in_output) {
                // 重定向cout到标准输出
                std::cout.rdbuf(oldcout);
                std::cout << "Testpoints: " << testpoints << std::endl;
            }
            
            // write testpoints to output.txt
            ofs << "Testpoints: " << testpoints << std::endl;
            if (is_late) {
                ofs << "Warning: late submission" << std::endl;
            }
            ofs.close();

            std::cout << "Finished grading " << sub_dir << std::endl << std::endl;
        }
    } else {
        std::string obj_filename;
        for (int i = 1; i < argc; i += 1) {
            std::string filename(argv[i]);
            if (filename[0] != '-') {
                if (endsWith(filename, ".bin")) {
                    converter.convertBin(filename);
                } else {
                    assembler.assemble(filename);
                }
                obj_filename = std::string(
                    filename.substr(0, filename.find_last_of('.')) + ".obj");
                std::cout << "Compiled file: " << obj_filename << std::endl;
                break;
            }
        }

        labTestWrapper(obj_filename, args);
    }
}