#pragma once

#include <string>
#include <vector>

#include "constants.hpp"
#include "cxxopts.hpp"
#include "staticmsgs.hpp"

#include "YFMultiFileTickManager.hpp"
#include "RandomBuySellStrategy.hpp"
#include "SmaStrategy.hpp"
#include "VwmaStrategy.hpp"
#include "VirtualBank.hpp"
#include "PaperTradeManager.hpp"

class Analyzer {
public:
    Analyzer() = default;

    Analyzer(cxxopts::ParseResult& result, uint _windowSize) : fileList(result["input"].as<std::string>()), tickManager(YFMultiFileTickManager(fileList)) {
        outputFile = result["output"].as<std::string>();

        windowSize = _windowSize;

        if (DEBUG_FLAG) {
            std::cout << staticmsgs::printargs << std::endl;
            std::cout << "Path to symbols file " << fileList << std::endl;
            std::cout << "Path to output file " << outputFile << std::endl;
        }
    }

    uint getNumFiles() { return fileList.size(); }

private:
    std::string fileList;
    std::vector<std::string> symbols;
    std::string outputFile;
public:
    YFMultiFileTickManager tickManager;
    VwmaStrategy strategy;
    VirtualBank vb;
    PaperTradeManager tradeManager;
    uint windowSize;
};