#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

#include "cxxopts.hpp"

#include "TradingSystem.hpp"
#include "helpers.hpp"
#include "Analyzer.hpp"
#include "CrossCorr.hpp"

struct AllocationMetrics {
    uint32_t totalAllocated = 0;
    uint32_t totalFreed = 0;

    uint32_t CurrentUsage() { return totalAllocated - totalFreed; }
};

static AllocationMetrics s_AllocationMetrics;

void* operator new(size_t size) {
    // std::cout << "allocating " << size << " bytes" << std::endl;
    s_AllocationMetrics.totalAllocated += size;

    return malloc(size);
}

void operator delete (void* memory, size_t size) {
    // std::cout << "freeing " << size << " bytes" << std::endl;
    s_AllocationMetrics.totalFreed += size;

    free(memory);
}

// todo
// should probably add some build and usage examples to the readme file
// would like to add a way to process data without applying a strategy to it and probably save results in a file or database
// could probably have options to select a time window and then possibly slide the window over the data and save the metrics being calculated as a function of time

int main(int argc, char** argv) {
    cxxopts::ParseResult result = processClInput(argc, argv);

    // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

    if (result.count("analysis") && result["analysis"].as<bool>()) {
        uint windowSize = 100;
        Analyzer al(result, windowSize);

        CrossCorr cc(al);

        // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::vector<Tick>::iterator> iters(al.tickManager.tick_store.size());
        for (uint i = 0; i < iters.size(); ++i) {
            iters[i] = al.tickManager.tick_store[i].begin();
        }

        bool done = false;
        
        while (!done) {
            cc.processNextWindow(iters);

            auto idx = iters[0] - al.tickManager.tick_store[0].begin();
            // std::cout << "iterator index is " << idx << std::endl;
            
            if (idx + al.windowSize > al.tickManager.tick_store[0].size()) {
                done = true;
                continue;
            }

            std::cout << "cross correlation matrix for closing prices of input symbols between " << iters[0]->time << " and " << (iters[0] + al.windowSize)->time << std::endl;
            cc.print();

            // slides window to next position in the data
            for (uint i = 0; i < iters.size(); ++i) {
                iters[i] += al.windowSize;
            }

            cc.computeEigenDecomp();
            cc.computeCholeskyDecomps();

            cc.resetCC();
        }

        

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start; // this is in ticks
        std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(diff); // ticks to time

        // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

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

    // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;
    
    return 0;
}