#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

#include "cxxopts.hpp"

#include "TradingSystem.hpp"
#include "helpers.hpp"
#include "CrossCorrAnalyzer.hpp"

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

int main(int argc, char** argv) {
    cxxopts::ParseResult parseOutput = processClInput(argc, argv);

    // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

    if (parseOutput.count("analysis") && parseOutput["analysis"].as<bool>()) {
        uint windowSize = 100;
        YFMultiFileTickManager tm(parseOutput["input"].as<std::string>());
        MultiFileTickProcessor tp(tm);
        CrossCorrAnalyzer al(tm, tp, windowSize);

        // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        al.run();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start; // this is in ticks
        std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(diff); // ticks to time

        // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

        std::cout << d.count() << "ms" << std::endl;
    } else {
        long double startingBalance = 100000;
        TradingSystem ts(parseOutput, startingBalance);

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