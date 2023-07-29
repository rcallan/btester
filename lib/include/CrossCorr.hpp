#pragma once

#include <iostream>
#include <vector>
#include "Analyzer.hpp"

#include <armadillo>

class CrossCorr {
    Analyzer& al;
    std::vector<long double> means;
    std::vector<long double> stdevs;
    arma::mat cc;

    arma::vec eigval;
    arma::mat eigvec;

public:
    CrossCorr(Analyzer& _al) : al(_al), means(std::vector<long double>(al.tickManager.tick_store.size())), cc(arma::mat(means.size(), means.size())) {
        stdevs.reserve(means.size());
    }

    void processNextWindow(std::vector<std::vector<Tick>::iterator>& iters);
    void computeMeans(std::vector<std::vector<Tick>::iterator>& iters);
    void computeStdevs(std::vector<std::vector<Tick>::iterator>& iters);
    void computeCC(std::vector<std::vector<Tick>::iterator>& iters);

    void computeEigenDecomp();
    void computeCholeskyDecomps();

    arma::mat getCC();
    void resetCC() { cc = arma::eye(means.size(), means.size()); }

    void print() {
        std::cout << cc << std::endl;
    }
};