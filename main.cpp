#include <fstream>
#include <iostream>
#include <random>

#include "cxxopts.hpp"

#include "TradingSystem.hpp"
#include "helpers.hpp"
#include "Analyzer.hpp"
#include "CrossCorr.hpp"

// todo
// should probably add some build and usage examples to the readme file
// would like to add a way to process data without applying a strategy to it and probably save results in a file or database
// could probably have options to select a time window and then possibly slide the window over the data and save the metrics being calculated as a function of time

int main(int argc, char** argv) {
    cxxopts::ParseResult result = processClInput(argc, argv);

    if (result.count("analysis") && result["analysis"].as<bool>()) {
        Analyzer al(result);

        CrossCorr cc(al);

        cc.process();
        cc.print();
    } else {
        long double startingBalance = 100000;
        TradingSystem ts(result, startingBalance);

        // start the core event loop
        Tick lastTradedTick;
        while (ts.tickManager.hasNextTick()) {
            lastTradedTick = ts.tickManager.getNextTick();

            Trade tr = ts.strategy.processTick(lastTradedTick);
            ts.tradeManager.performTrade(tr);
        }

        ts.tradeManager.squareOff(lastTradedTick);
        ts.tradeManager.dumpTrades();

        printResults(ts);
    }
    
    return 0;
}