#ifndef TRADE_HPP
#define TRADE_HPP 1

#include <iostream>
#include "Tick.hpp"

class Trade {
public:
    // todo - should probably switch this to an enumm class
    enum move {BUY, SELL, PASS, EMPTY};
    Trade::move mv;
    int quantity;
    Tick tick;
    Trade(Trade::move _mv, int _quantity, Tick _tick) : mv(_mv), quantity(_quantity), tick(_tick) { }
    void dump();
};

#endif