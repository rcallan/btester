#include <fstream>
#include <iostream>
#include <random>

#include "constants.hpp"
#include "cxxopts.hpp"
#include "staticmsgs.hpp"

#include "YahooFinanceFileTickManager.hpp"
#include "RandomBuySellStrategy.hpp"
#include "SmaStrategy.hpp"
#include "VwmaStrategy.hpp"
#include "VirtualBank.hpp"
#include "PaperTradeManager.hpp"
#include "TradingSystem.hpp"
#include "helpers.hpp"

// todo
// would probably like to have a tool which can retrieve data from yahoo finance which would then be ready for processing
// should probably add some build and usage examples to the readme file
// would like to add a way to process data without applying a strategy to it and probably save results in a file or database
// could probably have options to select a time window and then possibly slide the window over the data and save the metrics being calculated as a function of time

int main(int argc, char **argv) {
    cxxopts::ParseResult result = processClInput(argc, argv);
    
    long double startingBalance = 100000;
    TradingSystem ts(result, startingBalance);

    // start the core event loop
    Tick last_traded_tick;
    while (ts.tickManager.hasNextTick()) {
        Tick t = ts.tickManager.getNextTick();
        last_traded_tick = t;

        Trade tr = ts.strategy.processTick(t);
        ts.tradeManager.performTrade(tr);
    }

    ts.tradeManager.squareOff(last_traded_tick);
    ts.tradeManager.dumpTrades();

    printResults(ts);
    
    return 0;
}