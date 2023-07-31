#pragma once

class TickProcessor {
public:
    std::vector<std::vector<Tick>::iterator> iters;
    virtual long double getValue(unsigned i, unsigned j) = 0;
};