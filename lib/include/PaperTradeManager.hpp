#ifndef PAPER_TRADE_MANAGER_H
#define PAPER_TRADE_MANAGER_H 1

#include "TradeManager.hpp"
#include "VirtualBank.hpp"

class PaperTradeManager : public TradeManager {
public:
    int holdQuantity;
    VirtualBank vb;
    PaperTradeManager(VirtualBank& vb);
    int performBuy(Trade trade);
    int performSell(Trade trade);
    bool canBuy(Trade trade);
    bool canSell(Trade trade);
    bool squareOff(Tick last_tick);
};

#endif