#include "cxxopts.hpp"
#include "constants.hpp"
#include "TradingSystem.hpp"

cxxopts::ParseResult processClInput(int argc, char **argv) {
    cxxopts::Options options("btester v0.0.2", "btester - historical data backtester");

    options.add_options()
        ("i, input", "Input csv file, will read ticks from here",
            cxxopts::value<std::string>())
        ("o, output", "Output csv file, will output trade / results here",
            cxxopts::value<std::string>()->default_value("output.csv"))
        ("d, debug", "Enable debugging",
            cxxopts::value<bool>()->default_value("false"))
        ("h, help", "Print usage")
        ("a, analysis", "Analysis mode",
            cxxopts::value<bool>()->default_value("false"))
        ("p, polygon", "pg mode",
            cxxopts::value<bool>()->default_value("false"));

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



void printResults(TradingSystem& ts) {
    // show the final balance and profit/loss value

    std::string final_bank_balance = std::to_string(ts.vb.getBankBalance());
    std::string profit_or_loss = std::to_string(ts.vb.getBankBalance() - ts.startingBalance);

    std::cout << "total bank balance: " << final_bank_balance << std::endl;
    std::cout << "profit or loss: " << profit_or_loss << std::endl;
}