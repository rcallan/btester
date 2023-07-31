#pragma once

#include <string>
#include <vector>

#include "constants.hpp"
#include "cxxopts.hpp"
#include "staticmsgs.hpp"

#include "YFMultiFileTickManager.hpp"
#include "MultiFileTickProcessor.hpp"
#include "MultiFileTickManager.hpp"

#include <armadillo>

class CrossCorrAnalyzer {
private:
    std::vector<std::string> symbols;
    
    uint windowSize;

    arma::vec eigval;
    arma::mat eigvec;
    
public:
    MultiFileTickManager& tickManager;
    TickProcessor& tProcessor;

    std::vector<long double> means;
    std::vector<long double> stdevs;

    arma::mat cc;

    CrossCorrAnalyzer() = default;

    CrossCorrAnalyzer(MultiFileTickManager& tm, TickProcessor& tp, uint _windowSize) : windowSize(_windowSize),
                                                    tickManager(tm),
                                                    tProcessor(tp),
                                                    means(std::vector<long double>(tickManager.getTickStoreSize())),
                                                    stdevs(std::vector<long double>(tickManager.getTickStoreSize())),
                                                    cc(arma::mat(tickManager.getTickStoreSize(), tickManager.getTickStoreSize())) { }

    void run();

    void processNextWindow();
    void computeMeans();
    void computeStdevs();
    void computeCC();

    void computeEigenDecomp();
    void computeCholeskyDecomps();

    void resetCC() { cc = arma::eye(means.size(), means.size()); }

    void print() {
        std::cout << cc << std::endl;
    }
};
