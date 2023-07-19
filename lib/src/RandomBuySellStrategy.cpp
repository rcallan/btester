#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

#include "RandomBuySellStrategy.hpp"

Trade RandomBuySellStrategy::processTick(Tick tick) {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    int min = 1;
    int max = 3;
    Trade::move mv;
    std::uniform_int_distribution<int> dist(min, max);
    int randomMove = dist(mt);

    if (randomMove == 1) {
        mv = Trade::BUY;
        if (DEBUG_FLAG) {
            std::cout << "RandomBuySellStrategy: BUY SIGNAL generated" << std::endl;
        }
    } else if (randomMove == 2) {
        mv = Trade::SELL;
        if (DEBUG_FLAG) {
            std::cout << "RandomBuySellStrategy: SELL SIGNAL generated" << std::endl;
        }
    } else if (randomMove == 3) {
        mv = Trade::PASS;
        if (DEBUG_FLAG) {
            std::cout << "RandomBuySellStrategy: PASS SIGNAL generated" << std::endl;
        }
    }

    int quantity = 10;

    return Trade(mv, quantity, tick);
}