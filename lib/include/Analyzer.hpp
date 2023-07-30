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

#include "CrossCorr.hpp"

class Analyzer {
private:
    std::string fileList;
    std::vector<std::string> symbols;
    std::string outputFile;
public:
    uint windowSize;
    YFMultiFileTickManager tickManager;
    CrossCorr cc;

    Analyzer() = default;

    Analyzer(cxxopts::ParseResult& result, uint _windowSize) : fileList(result["input"].as<std::string>()), 
                                                                windowSize(_windowSize),
                                                                tickManager(YFMultiFileTickManager(fileList)),
                                                                cc(CrossCorr(tickManager.tick_store.size(), windowSize)) {
        outputFile = result["output"].as<std::string>();

        if (DEBUG_FLAG) {
            std::cout << staticmsgs::printargs << std::endl;
            std::cout << "Path to symbols file " << fileList << std::endl;
            std::cout << "Path to output file " << outputFile << std::endl;
        }
    }

    size_t getNumFiles() { return fileList.size(); }

    void initCC();
    void run();
};