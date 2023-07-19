#include <iostream>

#include "PaperTradeManager.hpp"

PaperTradeManager::PaperTradeManager(VirtualBank& _vb) : vb(_vb) {
    this->holdQuantity = 0;
}

int PaperTradeManager::performBuy(Trade trade) {
    std::cout << "paper trade buy" << std::endl;
    vb.bankDebit(trade.quantity * trade.tick.close);
    holdQuantity += trade.quantity;

    return 0;
}

int PaperTradeManager::performSell(Trade trade) {
    std::cout << "paper trade sell" << std::endl;
    vb.bankCredit(trade.quantity * trade.tick.close);
    holdQuantity -= trade.quantity;

    return 0;
}

bool PaperTradeManager::canBuy(Trade trade) {
    if (vb.bankCanDebit(trade.quantity * trade.tick.close)) {
        return true;
    }

    if (DEBUG_FLAG) {
        std::cout << "Could not perform a BUY signal, probably insufficient funds!"
              << std::endl;
    }

    return false;
}

bool PaperTradeManager::canSell(Trade trade) {
    if (trade.quantity <= holdQuantity) {
        return true;
    }

    if (DEBUG_FLAG) {
        std::cout << "Could not perform a SELL signal, you cant sell what you dont have"
            << std::endl;
    }

    return false;
}

bool PaperTradeManager::squareOff(Tick last_tick) {
    if (holdQuantity > 0) {
        Trade squareoff_trade(Trade::SELL, holdQuantity, last_tick);
        performTrade(squareoff_trade);
    }
    return false;
}

