#include "Trade.hpp"

void Trade::dump() {
    // std::cout << "dumping a trade sequence" << std::endl;
    std::string trade_move;

    if (this->mv == Trade::BUY)
        trade_move = "BUY";
    else if (this->mv == Trade::SELL)
        trade_move = "SELL";
    else
        trade_move = "PASS";

    std::cout << "trade move - " << trade_move << std::endl;
    std::cout << "trade quantity - " << this->quantity << std::endl;
    std::cout << "tick info -- " << std::endl;
    this->tick.dump();
    // std::cout << std::endl;
}