#pragma once

#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>
#include "Analyzer.hpp"

#include <armadillo>

class CrossCorr {
    Analyzer& al;
    std::vector<long double> means;
    std::vector<long double> stdevs;
    arma::mat cc;

public:

    CrossCorr(Analyzer& _al) : al(_al), means(std::vector<long double>(al.tickManager.tick_store.size())), cc(arma::mat(means.size(), means.size())) {
        stdevs.reserve(means.size());
    }

    void process();

    void print() {
        std::cout << cc << std::endl;
    }
    
};