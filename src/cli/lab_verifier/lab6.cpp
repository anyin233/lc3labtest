#include "lab6.hpp"
#include "utils.hpp"
#include <cstdint>
#include <vector>

namespace lc3 {
    namespace verifier {

        int lab6Test(lc3::sim &simulator, const std::string &obj_filename,
                     StringInputter &inputter) {
            uint32_t passed_count = 0;
            std::vector<std::string> testInput;
            testInput.emplace_back(std::string(2, rand() % 16 + 0x20));
            testInput.emplace_back(std::string(2, rand() % 69 + 0x3a));
            testInput.emplace_back("0");
            testInput.emplace_back(std::string(1, rand() % 8 + 0x30));
            testInput.emplace_back("9");

            std::cout << "!!! Lab6 with ignore privilege mode disabled !!!\n"
                      << std::endl;

            for (auto &input : testInput) {
                sim_start(simulator, obj_filename);
                simulator.setRunInstLimit(200000);
                simulator.setIgnorePrivilege(false);
                inputter.setCharDelay(60000);
                // set input
                inputter.setString(input);

                // start simulator
                simulator.run();
                std::cout << std::endl;
            }

            std::cout << "!!! Lab6 with ignore privilege mode enabled !!!\n"
                      << std::endl;

            for (auto &input : testInput) {
                sim_start(simulator, obj_filename);
                simulator.setRunInstLimit(200000);
                simulator.setIgnorePrivilege(true);
                inputter.setCharDelay(60000);
                // set input
                inputter.setString(input);

                // start simulator
                simulator.run();
                std::cout << std::endl;
            }

            return passed_count;
        }
    } // namespace verifier
} // namespace lc3
