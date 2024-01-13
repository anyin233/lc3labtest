#include <cstdint>
#include <string>

#include "inputter.h"
#include "interface.h"

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