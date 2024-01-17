#include "lab6.hpp"
#include "utils.hpp"
#include <cctype>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace lc3 {
    namespace verifier {
        bool testLoop(lc3::sim &simulator, const std::string &obj_filename, StringInputter &inputter, std::vector<std::string> const & testInput, bool privilege = false) {
           auto &printer = static_cast<StringVerifier&>(simulator.getPrinter());
            printer.clear_matched();
           std::cout << testInput.size() << std::endl;
        //    int counter = 0;
            for (const auto & input : testInput) {
                // std::cout << counter ++ << std::endl;
                sim_start(simulator, obj_filename);
                simulator.setRunInstLimit(200000);
                simulator.setIgnorePrivilege(!privilege);
                inputter.setCharDelay(60000);
                // set input
                inputter.setString(input);

                printer.resetExpected();
                std::cout << "test input: " << input << std::endl;
                std::stringstream expected;
                if (std::isdigit(input[0])) {
                    int digit = std::stoi(input);
                    if (digit >= 8) {
                        expected << "istoolargeforLC-3";
                        printer.appendExpected(expected.str());
                    } else {
                        expected << input << "!=" << int(tgamma(digit + 1));
                        printer.appendExpected(expected.str());
                        // // expected.clear();
                        // std::stringstream expected1;
                        // expected1 << input + " != " << int(tgamma(digit + 1));
                        // printer.appendExpected(expected1.str());
                        // std::stringstream expected2;
                        // expected2 << input << "! = " << tgamma(digit + 1);
                    }
                } else {
                    expected << "isnotadecimaldigit.";
                    printer.appendExpected(expected.str());
                    printer.appendExpected("isnotademicaldigit");
                }
                std::cout << "Expected result: ";
                for (const auto& candidcate: printer.getExpected()) {
                    std::cout << candidcate << " ";
                }
                std::cout << std::endl;
                
                // start simulator
                simulator.run();
                // passed_count += printer.matched() ? 1 : 0;
                // std::cout << (printer.matched() ? "Matched\n" : "Unmatched\n");
                std::cout << std::endl;
            }

            return printer.matched_count() == (testInput.size() * 2 + 5);
        }

        int lab6Test(lc3::sim &simulator, const std::string &obj_filename, const std::string &stu_id,
                     StringInputter &inputter) {
            uint32_t passed_count = 0;
            std::cout << "checking " << stu_id << std::endl;
            std::vector<std::string> testInput;
            testInput.emplace_back(1, rand() % 16 + 0x20);
            // testInput.emplace_back(1, rand() % 16 + 0x20);
            testInput.emplace_back(1, rand() % 69 + 0x3a);
            // testInput.emplace_back(1, rand() % 69 + 0x3a);
            testInput.emplace_back("0");
            testInput.emplace_back(1, rand() % 8 + 0x31);
            testInput.emplace_back("9");
            // std::cout << testInput.size(); 

            std::cout << "!!! Lab6 with ignore privilege mode disabled !!!\n"
                      << std::endl;
            
            // auto &printer = static_cast<StringVerifier&>(simulator.getPrinter());
            // for (auto &input : testInput) {
            //     printer.resetExpected();
            //     std::cout << "test input: " << input << std::endl;
            //     std::string expected;
            //     if (std::isdigit(input[0])) {
            //         int digit = std::stoi(input);
            //         if (digit >= 8) {
            //             expected = "is too large for LC-3";
            //             printer.appendExpected(expected);
            //         } else {
            //             expected = digit + "!=" + int(tgamma(digit + 1));
            //             printer.appendExpected(expected);
            //             expected = digit + " != " + int(tgamma(digit + 1));
            //             printer.appendExpected(expected);
            //         }
            //     } else {
            //         expected = "is not a decimal digit.";
            //         printer.appendExpected(expected);
            //     }
            //     sim_start(simulator, obj_filename);
            //     simulator.setRunInstLimit(200000);
            //     simulator.setIgnorePrivilege(false);
            //     inputter.setCharDelay(60000);
            //     // set input
            //     inputter.setString(input);
                
            //     // start simulator
            //     simulator.run();
            //     passed_count += printer.matched() ? 1 : 0;
            //     std::cout << std::endl;
            // }
            if (testLoop(simulator, obj_filename, inputter, testInput, false)) {
                passed_count += 5; // running with privilege
            }

            std::cout << "!!! Lab6 with ignore privilege mode enabled !!!\n"
                      << std::endl;

            testLoop(simulator, obj_filename, inputter, testInput, true);
            // for (auto &input : testInput) {
            //     std::cout << "test input: " << input << std::endl;
            //     sim_start(simulator, obj_filename);
            //     simulator.setRunInstLimit(200000);
            //     simulator.setIgnorePrivilege(true);
            //     inputter.setCharDelay(60000);
            //     // set input
            //     inputter.setString(input);

            //     // start simulator
            //     simulator.run();
            //     std::cout << std::endl;
            // }
            std::cout << "matched cases: " << dynamic_cast<StringVerifier&>(simulator.getPrinter()).matched_count() << std::endl;
            return dynamic_cast<StringVerifier&>(simulator.getPrinter()).matched_count() + passed_count + 5; // program halt
        }
    } // namespace verifier
} // namespace lc3
