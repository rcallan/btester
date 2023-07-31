#pragma once

#include <string>

#include "constants.hpp"
#include "cxxopts.hpp"
#include "staticmsgs.hpp"

#include "YahooFinanceFileTickManager.hpp"
#include "RandomBuySellStrategy.hpp"
#include "SmaStrategy.hpp"
#include "VwmaStrategy.hpp"
#include "VirtualBank.hpp"
#include "PaperTradeManager.hpp"

// todo - should probably update this to be like the cross corr analyzer class, so subobjects are references instead of copies of objects being passed in
// this allows us to use polymorphic types when passing these objects in
class TradingSystem {
public:
    TradingSystem() = default;
    TradingSystem(cxxopts::ParseResult& result, long double sb) : inputFile(result["input"].as<std::string>()), startingBalance(sb), tickManager(YahooFinanceFileTickManager(inputFile)), 
                                                                    vb(VirtualBank(startingBalance)), tradeManager(PaperTradeManager(vb)) {
        // inputFile = result["input"].as<std::string>();
        outputFile = result["output"].as<std::string>();

        if (DEBUG_FLAG) {
            std::cout << staticmsgs::printargs << std::endl;
            std::cout << "Path to input file " << inputFile << std::endl;
            std::cout << "Path to output file " << outputFile << std::endl;
        }

        // std::unique_ptr<Strategy> strategy(new RandomBuySellStrategy());
        // std::unique_ptr<Strategy> strategy(new SmaStrategy());
        // strategy = std::make_unique<VwmaStrategy>();
    }

    TradingSystem(cxxopts::ParseResult& result) : inputFile(result["input"].as<std::string>()), tickManager(YahooFinanceFileTickManager(inputFile)) {
        outputFile = result["output"].as<std::string>();

        if (DEBUG_FLAG) {
            std::cout << staticmsgs::printargs << std::endl;
            std::cout << "Path to input file " << inputFile << std::endl;
            std::cout << "Path to output file " << outputFile << std::endl;
        }
    }

private:
    std::string inputFile;
    std::string outputFile;
public:
    long double startingBalance;
    YahooFinanceFileTickManager tickManager;
    VwmaStrategy strategy;
    VirtualBank vb;
    PaperTradeManager tradeManager;
};