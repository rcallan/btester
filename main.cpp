#include <fstream>
#include <iostream>
#include <random>
#include <memory>

#include "constants.hpp"
#include "cxxopts.hpp"
#include "staticmsgs.hpp"

#include "YahooFinanceFileTickManager.hpp"
#include "RandomBuySellStrategy.hpp"
#include "SmaStrategy.hpp"
#include "VwmaStrategy.hpp"
#include "VirtualBank.hpp"
#include "PaperTradeManager.hpp"

cxxopts::ParseResult processClInput(int argc, char **argv) {
    cxxopts::Options options("btester v0.0.2", "btester - historical data backtester");

    options.add_options()
        ("i, input", "Input csv file, will read ticks from here",
            cxxopts::value<std::string>())
        ("o, output", "Output csv file, will output trade / results here",
            cxxopts::value<std::string>()->default_value("output.csv"))
        ("d, debug", "Enable debugging",
            cxxopts::value<bool>()->default_value("false"))
        ("h, help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    DEBUG_FLAG = result["debug"].as<bool>();

    // check if input file was specified - exit if it was not
    if (result.count("input")) {
        // inputFile = result["input"].as<std::string>();
    } else {
        std::cout << staticmsgs::noinputfile << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    return result;
}

class TradingSystem {
public:
    TradingSystem() = default;
    TradingSystem(cxxopts::ParseResult& result, long double sb) : startingBalance(sb) {
        inputFile = result["input"].as<std::string>();
        outputFile = result["output"].as<std::string>();

        if (DEBUG_FLAG) {
            std::cout << staticmsgs::printargs << std::endl;
            std::cout << "Path to input file " << inputFile << std::endl;
            std::cout << "Path to output file " << outputFile << std::endl;
        }

        tickManager = std::make_unique<YahooFinanceFileTickManager>(inputFile);

        // std::unique_ptr<Strategy> strategy(new RandomBuySellStrategy());
        // std::unique_ptr<Strategy> strategy(new SmaStrategy());
        strategy = std::make_unique<VwmaStrategy>();

        vb = std::make_shared<VirtualBank>(startingBalance);

        tradeManager = std::make_unique<PaperTradeManager>(vb);
    }

    std::string inputFile;
    std::string outputFile;
    std::unique_ptr<TickManager> tickManager;
    std::unique_ptr<Strategy> strategy;
    std::shared_ptr<VirtualBank> vb;
    std::unique_ptr<TradeManager> tradeManager;
    long double startingBalance;
};

void printResults(TradingSystem& ts) {
    // show the final balance and profit/loss value
    
    std::string final_bank_balance = std::to_string(ts.vb->getBankBalance());
    std::string profit_or_loss = std::to_string(ts.vb->getBankBalance() - ts.startingBalance);

    std::cout << "total bank balance: " << final_bank_balance << std::endl;
    std::cout << "profit or loss: " << profit_or_loss << std::endl;
}

int main(int argc, char **argv) {
    cxxopts::ParseResult result = processClInput(argc, argv);
    
    long double startingBalance = 100000;
    TradingSystem ts(result, startingBalance);

    // start the core event loop
    Tick last_traded_tick;
    while (ts.tickManager->hasNextTick()) {
        Tick t = ts.tickManager->getNextTick();
        last_traded_tick = t;

        Trade tr = ts.strategy->processTick(t);
        ts.tradeManager->performTrade(tr);
    }

    ts.tradeManager->squareOff(last_traded_tick);
    ts.tradeManager->dumpTrades();

    printResults(ts);
    
    return 0;
}