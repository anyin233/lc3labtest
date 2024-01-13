#include "input.hpp"

void StringInputter::setString(std::string const &source) {
    this->source = source;
    this->pos = 0;
}

void StringInputter::setCharDelay(uint32_t inst_count) {
    this->reset_inst_delay = inst_count;
    this->cur_inst_delay = inst_count;
}

bool StringInputter::getChar(char &c) {
    if (cur_inst_delay > 0) {
        --cur_inst_delay;
        return false;
    }

    if (pos == source.size()) {
        return false;
    }

    c = source[pos];
    ++pos;
    cur_inst_delay = reset_inst_delay;
    return true;
}
