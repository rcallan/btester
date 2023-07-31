#pragma once

class TickProcessor {
public:
    virtual long double getValue(unsigned i, unsigned j) = 0;
};