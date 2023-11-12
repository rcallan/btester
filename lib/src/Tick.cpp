#include <iostream>

#include "Tick.hpp"

void Tick::dump() {
    std::cout << "dumping tick values" << std::endl;
    std::cout << "timestamp - " << this->time << std::endl;
    std::cout << "open - " << std::to_string(this->open) << std::endl;
    std::cout << "high - " << std::to_string(this->high) << std::endl;
    std::cout << "low - " << std::to_string(this->low) << std::endl;
    std::cout << "close - " << std::to_string(this->close) << std::endl;
    std::cout << "adjust close - " << std::to_string(this->adjust_close) << std::endl;
    std::cout << "volume - " << std::to_string(this->volume) << std::endl;
    std::cout << "vwap - " << std::to_string(this->vwap) << std::endl;
    std::cout << "txs - " << std::to_string(this->txs) << std::endl;

    std::cout << std::endl;
}