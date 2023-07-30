#pragma once

#include <iostream>
#include <vector>
#include "Tick.hpp"

#include <armadillo>

class CrossCorr {
    std::vector<long double> means;
    std::vector<long double> stdevs;
    arma::mat cc;
    uint windowSize;

    arma::vec eigval;
    arma::mat eigvec;

public:
    CrossCorr(uint sz, uint ws) : means(std::vector<long double>(sz)), stdevs(std::vector<long double>(sz)), cc(arma::mat(sz, sz)), windowSize(ws) { }

    void processNextWindow();
    void computeMeans();
    void computeStdevs();
    void computeCC();

    void computeEigenDecomp();
    void computeCholeskyDecomps();

    arma::mat getCC();
    void resetCC() { cc = arma::eye(means.size(), means.size()); }

    void print() {
        std::cout << cc << std::endl;
    }

    std::vector<std::vector<Tick>::iterator> iters;
};