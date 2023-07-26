#include <fstream>
#include <iostream>
#include <random>
#include <numeric>

#include "cxxopts.hpp"

#include "TradingSystem.hpp"
#include "helpers.hpp"
#include "Analyzer.hpp"

// todo
// would probably like to have a tool which can retrieve data from yahoo finance which would then be ready for processing
// should probably add some build and usage examples to the readme file
// would like to add a way to process data without applying a strategy to it and probably save results in a file or database
// could probably have options to select a time window and then possibly slide the window over the data and save the metrics being calculated as a function of time

int main(int argc, char** argv) {
    cxxopts::ParseResult result = processClInput(argc, argv);

    if (result.count("analysis") && result["analysis"].as<bool>()) {
        Analyzer al(result);

        std::vector<long double> means(al.tickManager.tick_store.size());
        std::vector<long double> stdevs(means.size());

        for (unsigned i = 0; i < means.size(); ++i) {
            means[i] = std::accumulate(al.tickManager.tick_store[i].begin(), al.tickManager.tick_store[i].end(), 0.0, [](long double a, Tick b) { return a + b.close; }) / al.tickManager.tick_store[i].size();
        }

        for (unsigned i = 0; i < means.size(); ++i) {
            long double sumOfSquareDifferences = 0.0;
            for (unsigned j = 0; j < al.tickManager.tick_store[i].size(); ++j) {
                sumOfSquareDifferences += (al.tickManager.tick_store[i][j].close - means[i]) * (al.tickManager.tick_store[i][j].close - means[i]);
            }
            stdevs[i] = sqrt(1.0 / al.tickManager.tick_store[i].size() * sumOfSquareDifferences);
            std::cout << "stdevs " << i << " " << stdevs[i] << std::endl;
        }

        std::vector<std::vector<long double>> crosscorr(means.size(), std::vector<long double>(means.size(), 0.0));

        for (unsigned i = 0; i < crosscorr.size(); ++i) {
            for (unsigned j = 0; j < crosscorr[i].size(); ++j) {
                if (crosscorr[i][j] > 0.0) continue;
                unsigned numVals = al.tickManager.tick_store[i].size();
                long double numerator = 0.0;
                for (unsigned k = 0; k < numVals; ++k) {
                    numerator += (al.tickManager.tick_store[i][k].close - means[i]) * (al.tickManager.tick_store[j][k].close - means[j]);
                }
                crosscorr[i][j] = numerator / ((long double)numVals * stdevs[i] * stdevs[j]);
                crosscorr[j][i] = crosscorr[i][j];
                std::cout << i << " " << j << " " << crosscorr[i][j] << std::endl;
            }
        }

        // start the core event loop
        // std::vector<Tick> lastTradedTick;
        // while (al.tickManager.hasNextTick()) {
        //     lastTradedTick = al.tickManager.getNextTickV();
        // }
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