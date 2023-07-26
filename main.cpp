#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

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

        auto start = std::chrono::high_resolution_clock::now();

        cc.process();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start; // this is in ticks
        std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(diff); // ticks to time

        // std::cout << diff.count() << "s\n";
        std::cout << d.count() << "ms\n";

        // cc.print();
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