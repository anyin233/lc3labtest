#include <cstdint>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "callback.h"
#include "inputter.h"
#include "interface.h"
#include "printer.h"

class StringInputter : public lc3::utils::IInputter {
public:
    StringInputter(void) : StringInputter("") {}
    StringInputter(std::string const &source)
        : pos(0), reset_inst_delay(0), cur_inst_delay(0) {
        setString(source);
    }

    void setString(std::string const &source);
    void setCharDelay(uint32_t inst_count);
    virtual void beginInput(void) override {}
    virtual bool getChar(char &c) override;
    virtual void endInput(void) override {}
    virtual bool hasRemaining(void) const override {
        return pos == source.size();
    }

private:
    std::string source;
    uint32_t pos;
    uint32_t reset_inst_delay, cur_inst_delay;
};

class StringVerifier : public lc3::utils::IPrinter {
private:
    std::string current_line;
    std::vector<std::string> expected;
    std::string stu_id;
    bool stu_id_matched = false;
    int match_count = 0;
public:
    explicit StringVerifier(std::string stu_id): stu_id(std::move(stu_id)) {}

    void setColor(lc3::utils::PrintColor color) override {}
    void print(std::string const &string) override {
        // match = false;
        current_line += string;
        std::cout << string << std::flush;
    }
    void newline() override {
        remove(current_line.begin(), current_line.end(), ' ');
        for (const auto& candidcate: expected) {
            // std::cout << "Matching " << current_line << "with" << candidcate << std::endl;
            if (current_line.find(candidcate) != std::string::npos) {
                match_count += 2; // input result matched
                std::cout << "Matched\n";
                break;
            }
        }
        if (!stu_id_matched && current_line.find(stu_id) != std::string::npos) {
            match_count += 5; // student_id matched 0.5
            stu_id_matched = true;
        }
        std::cout << std::endl;
        current_line.clear();
    }

    void appendExpected(const std::string& expected) {
        this->expected.emplace_back(expected);
    }

    void resetExpected() {
        this->expected.clear();
    }

    [[nodiscard]] std::vector<std::string> const &getExpected() const {
        return expected;
    }

    StringVerifier() {
        match_count = 0;
    }

    int matched_count() const {
        return match_count;
    }

    void clear_matched() {
        match_count = 0;
        stu_id_matched = false;
    }
};