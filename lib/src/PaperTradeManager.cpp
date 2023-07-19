#include <iostream>
#include <memory>

#include "PaperTradeManager.hpp"

PaperTradeManager::PaperTradeManager(std::shared_ptr<VirtualBank> vb) {
    this->vb = vb;
    this->holdQuantity = 0;
}

int PaperTradeManager::performBuy(Trade trade) {
    std::cout << "paper trade buy" << std::endl;
    this->vb->bankDebit(trade.quantity * trade.tick.close);
    this->holdQuantity += trade.quantity;

    return 0;
}

int PaperTradeManager::performSell(Trade trade) {
    std::cout << "paper trade sell" << std::endl;
    this->vb->bankCredit(trade.quantity * trade.tick.close);
    this->holdQuantity -= trade.quantity;

    return 0;
}

bool PaperTradeManager::canBuy(Trade trade) {
    if (this->vb->bankCanDebit(trade.quantity * trade.tick.close)) {
        return true;
    }

    if (DEBUG_FLAG) {
        std::cout << "Could not perform a BUY signal, probably insufficient funds!"
              << std::endl;
    }

    return false;
}

bool PaperTradeManager::canSell(Trade trade) {
    if (trade.quantity <= this->holdQuantity) {
        return true;
    }

    if (DEBUG_FLAG) {
        std::cout << "Could not perform a SELL signal, you cant sell what you dont have"
            << std::endl;
    }

    return false;
}

bool PaperTradeManager::squareOff(Tick last_tick) {
    if (this->holdQuantity > 0) {
        Trade squareoff_trade(Trade::SELL, this->holdQuantity, last_tick);
        this->performTrade(squareoff_trade);
    }
    return false;
}

