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
#include "MultiFileTickProcessor.hpp"

#include <armadillo>

template <typename T1, typename T2>
class CrossCorrAnalyzer {
private:
    std::vector<std::string> symbols;
    // std::string outputFile;
    
    uint windowSize;

    arma::vec eigval;
    arma::mat eigvec;
    
public:
    T1& tickManager;
    T2& tProcessor;

    std::vector<long double> means;
    std::vector<long double> stdevs;

    arma::mat cc;

    CrossCorrAnalyzer() = default;

    CrossCorrAnalyzer(T1& tm, T2& tp, uint _windowSize) : windowSize(_windowSize),
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

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::computeMeans() {
    std::vector<long double> temp(means.size());
    for (unsigned i = 0; i < means.size(); ++i) {
        // means[i] = std::accumulate(tProcessor.iters[i], tProcessor.iters[i] + windowSize, 0.0, [](long double a, const Tick& b) { return a + b.close; }) / windowSize;
        for (unsigned j = 0; j < windowSize; ++j) {
            temp[i] += tProcessor.getValue(i, j);
        }
        means[i] = temp[i] / windowSize;
    }
}

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::computeStdevs() {
    for (unsigned i = 0; i < means.size(); ++i) {
        long double sumOfSquareDifferences = 0.0;
        for (unsigned j = 0; j < windowSize; ++j) {
            sumOfSquareDifferences += (tProcessor.getValue(i, j) - means[i]) * (tProcessor.getValue(i, j) - means[i]);
        }
        stdevs[i] = sqrt(sumOfSquareDifferences / (windowSize - 1));
    }
}

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::computeCC() {
    for (unsigned i = 0; i < cc.n_rows; ++i) {
        for (unsigned j = 0; j < cc.n_cols; ++j) {
            if (cc(i, j) > 0.0) continue;
            long double numerator = 0.0;
            for (unsigned k = 0; k < windowSize; ++k) {
                numerator += (tProcessor.getValue(i, k) - means[i]) * (tProcessor.getValue(j, k) - means[j]);
            }
            cc(i, j) = numerator / ((long double)windowSize * stdevs[i] * stdevs[j]);
            cc(j, i) = cc(i, j);
        }
    }
}

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::computeEigenDecomp() {
    arma::eig_sym(eigval, eigvec, cc);

    std::cout << "size of eigval is " << size(eigval) << std::endl;
    std::cout << "size of eigvec is " << size(eigvec) << std::endl;
}

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::computeCholeskyDecomps() {
    arma::mat R1 = arma::chol(cc);
    arma::mat R2 = arma::chol(cc, "lower");

    arma::mat R3;
    bool ok = arma::chol(R3, cc);

    std::cout << "cholesky decomp okay: " << ok << std::endl;

    arma::mat R;
    arma::uvec P_vec;
    arma::umat P_mat;

    arma::chol(R, P_vec, cc, "upper", "vector");
    arma::chol(R, P_mat, cc, "lower", "matrix");

    // std::cout << "lower cholesky decomposition with permutation matrix\n" << R << std::endl;
}

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::processNextWindow() {
    computeMeans();
    computeStdevs();
    computeCC();
}

template <typename T1, typename T2>
void CrossCorrAnalyzer<T1, T2>::run() {
    bool done = false;
    
    while (!done) {
        processNextWindow();

        auto idx = tProcessor.iters[0] - tickManager.tick_store[0].begin();
        // std::cout << "iterator index is " << idx << std::endl;
        
        if (idx + windowSize > tickManager.tick_store[0].size()) {
            done = true;
            continue;
        }

        std::cout << "cross correlation matrix for closing prices of input symbols between " << tProcessor.iters[0]->time << " and " << (tProcessor.iters[0] + windowSize)->time << std::endl;
        cc.print();

        // slides window to next position in the data
        for (uint i = 0; i < tProcessor.iters.size(); ++i) {
            tProcessor.iters[i] += windowSize;
        }

        computeEigenDecomp();
        computeCholeskyDecomps();

        resetCC();
    }
}