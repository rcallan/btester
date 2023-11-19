#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

#include "cxxopts.hpp"

#include "TradingSystem.hpp"
#include "helpers.hpp"
#include "CrossCorrAnalyzer.hpp"

#include <deque>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <vector>
#include <thread>
#include <atomic>

#include <thread_pool/thread_pool.h>
#include <thread_pool/version.h>

#include <concurrentqueue/blockingconcurrentqueue.h>

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

class DataProcessor {
public:

    DataProcessor() : yearHisto(5), avgCooldown(5), minBank(bank.load()) {}

    void processFile(std::string pth) {
        // PolygonFileTickManager tm(parseOutput["input"].as<std::string>());
        PolygonFileTickManager tm(pth);
        // TradingSystem ts(parseOutput, true);

        std::deque<long double> closePrices;
        int numEvents = 0;

        int cooldownCounter = 0;
        long double minClose;
        int yearIdx;
        long double origClose;
        bool madeWin = false;
        double endClose;

        Tick lastTradedTick;
        long lastTs = 0;
        while (tm.hasNextTick()) {
            lastTradedTick = tm.getNextTick();

            if (lastTs == 0) {
                lastTs = lastTradedTick.time;
            } else {
                double numMins = (lastTradedTick.time - lastTs) / 1000.0 / 60.0;
                lastTs = lastTradedTick.time;
                if (numMins > 31) {
                    closePrices.clear();
                    // continue;
                    if (cooldownCounter > 0) {
                        cooldownCounter = 0;
                        --numEvents;
                    }
                }
            }            

            closePrices.push_front(lastTradedTick.close);
            if (closePrices.size() > 20) {
                closePrices.pop_back();
            }
            auto minIt = std::min_element(closePrices.begin(), closePrices.end());
            // auto maxIt = std::max_element(closePrices.begin(), closePrices.end());

            // std::cout << *maxIt << " " << *minIt << std::endl;
            if (lastTradedTick.close / (*minIt) > 1.02) { 
                // std::cout << "found a possibly useful event" << std::endl;
                if (cooldownCounter == 0) {
                    std::cout << lastTradedTick.time / 1000 << std::endl;
                    ++numEvents;
                    ++cooldownCounter;
                    // 1640995200
                    // 1609459200
                    // 1577836800
                    // 1546300800
                    if (lastTradedTick.time / 1000 > 1672531200) {
                        yearHisto[0]++;
                        yearIdx = 0;
                    } else if (lastTradedTick.time / 1000 > 1640995200) {
                        yearHisto[1]++;
                        yearIdx = 1;
                    } else if (lastTradedTick.time / 1000 > 1609459200) {
                        yearHisto[2]++;
                        yearIdx = 2;
                    } else if (lastTradedTick.time / 1000 > 1577836800) {
                        yearHisto[3]++;
                        yearIdx = 3;
                    } else {
                        yearHisto[4]++;
                        yearIdx = 4;
                    }
                    minClose = lastTradedTick.close;
                    origClose = lastTradedTick.close;
                    std::cout << "close at time when signal was found is " << minClose << std::endl;
                }
            }

            if (cooldownCounter > 0) {
                ++cooldownCounter;
                if (!madeWin)
                    if (lastTradedTick.close < minClose) minClose = lastTradedTick.close;
                if (minClose / origClose <= 0.98) {
                    madeWin = true;
                    endClose = minClose;
                }
                if (cooldownCounter == 60) {
                    std::cout << "min close during cooldown is " << minClose << " " << lastTradedTick.close << std::endl;
                    avgCooldown[yearIdx] += minClose / origClose;
                    bank -= 50.0;
                    if (madeWin)
                        bank += 50.0 * origClose / endClose;
                    else
                        bank += 50.0 * origClose / lastTradedTick.close;
                    if (bank < minBank) minBank = bank.load();
                    moneyPutIn += 50;
                    cooldownCounter = 0;
                    if (madeWin) {
                        ++totalWins;
                        madeWin = false;
                    } else if (lastTradedTick.close < origClose) {
                        ++totalLSWins;
                    }
                }
                
            }

            // std::cout << lastTradedTick.close << std::endl;
            
        }

        if (numEvents > 0) {
            std::cout << "found " << numEvents << " possibly useful events for " << pth << std::endl;
            ++numStocks;
            totalEvents += numEvents;
        }
    }

    void printResults() {
        std::cout << "number of stocks which had positive number of events is " << numStocks << std::endl;
        std::cout << "total number of events is " << totalEvents << std::endl;
        std::cout << "total number of wins is " << totalWins << std::endl;
        std::cout << "total number of close out wins is " << totalLSWins << std::endl;
        std::cout << "winrate was " << (double)(totalWins + totalLSWins) / (double)totalEvents << std::endl;
        std::cout << "final bank is " << bank << std::endl;
        std::cout << "money put in was " << moneyPutIn << std::endl;
        std::cout << "bank min value during history was " << minBank << std::endl;

        for (int i = 0; i < 5; ++i) {
            std::cout << yearHisto[i] << std::endl;
            avgCooldown[i] /= (double)yearHisto[i];
            std::cout << avgCooldown[i] << std::endl;
        }
    }

private:
    std::atomic<int> numStocks {0};
    std::atomic<int> totalEvents {0};
    std::vector<int> yearHisto;
    std::vector<double> avgCooldown;
    std::atomic<int> totalWins {0};
    std::atomic<int> totalLSWins {0};
    std::atomic<double> bank {10000.0};
    std::atomic<double> minBank;
    std::atomic<int> moneyPutIn {0};
};

int main(int argc, char** argv) {
    cxxopts::ParseResult parseOutput = processClInput(argc, argv);

    // std::cout << "current memory usage is " << s_AllocationMetrics.CurrentUsage() << " bytes" << std::endl;

    if (parseOutput.count("polygon")) {
        // const std::filesystem::path dataDir{"../pg_data"};
        const std::filesystem::path dataDir{"../../polygon/s&p600_approx5yrs_1minIntervals"};

        DataProcessor dp;
    
        std::cout << "directory_iterator:\n";
        // directory_iterator can be iterated using a range-for loop
        for (auto const& dir_entry : std::filesystem::directory_iterator{dataDir}) {
            dp.processFile(dir_entry.path());
        }
        
        dp.printResults();


    } else if (parseOutput.count("analysis") && parseOutput["analysis"].as<bool>()) {
        uint windowSize = 100;
        YFMultiFileTickManager tm(parseOutput["input"].as<std::string>());
        MultiFileTickProcessor tp(tm);
        CrossCorrAnalyzer<MultiFileTickProcessor> al(tm, tp, windowSize);

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
        while (ts.tickManager->hasNextTick()) {
            lastTradedTick = ts.tickManager->getNextTick();

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