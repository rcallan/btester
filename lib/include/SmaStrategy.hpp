#pragma once

#include "constants.hpp"
#include "Strategy.hpp"

class SmaStrategy : public Strategy {
public:
    bool isPosition = false;
    int tick_count;
    long double total_closing_price;
    long double last_bought_price;
    Trade processTick(Tick t);
};