#include <iostream>

#include "VwmaStrategy.hpp"

Trade VwmaStrategy::processTick(Tick tick) {
    int quantity = 10;
    int smaDayRange = 5;
    int differenceTolerance = 10;
    Trade::move mv;

    this->tick_count += 1;
    this->total_closing_price += (tick.close * tick.volume);

    // the following should ensure that we exit the position if we are making a profit
    if (this->last_bought_price != 0 && (tick.close - this->last_bought_price) > differenceTolerance) {
        mv = Trade::SELL;
        if (DEBUG_FLAG) {
            std::cout << "SmaStrategy: Safe SELL SIGNAL generated" << std::endl;
        }
        return Trade(mv, quantity, tick);
    }

    if (this->tick_count >= smaDayRange) {
        long double ma = this->total_closing_price / smaDayRange;
        this->tick_count = 0;
        this->total_closing_price = 0;

        if ((tick.close * tick.volume) > ma) {
            this->isPosition = true;
            mv = Trade::BUY;
            this->last_bought_price = tick.close;
            if (DEBUG_FLAG) {
                std::cout << "SmaStrategy: BUY SIGNAL generated" << std::endl;
            }
        } else {
            mv = Trade::PASS;
            if (DEBUG_FLAG) {
                std::cout << "SmaStrategy: PASS SIGNAL generated" << std::endl;
            }
        }
    } else {
        mv = Trade::PASS;
        if (DEBUG_FLAG) {
            std::cout << "SmaStrategy: PASS SIGNAL generated" << std::endl;
        }
    }

    return Trade(mv, quantity, tick);
}