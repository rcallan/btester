#include "CrossCorrAnalyzer.hpp"

void CrossCorrAnalyzer::computeMeans() {
    std::vector<long double> temp(means.size());
    for (unsigned i = 0; i < means.size(); ++i) {
        // means[i] = std::accumulate(tProcessor.iters[i], tProcessor.iters[i] + windowSize, 0.0, [](long double a, const Tick& b) { return a + b.close; }) / windowSize;
        for (unsigned j = 0; j < windowSize; ++j) {
            temp[i] += tickProcessor.getValue(i, j);
        }
        means[i] = temp[i] / windowSize;
    }
}

void CrossCorrAnalyzer::computeStdevs() {
    for (unsigned i = 0; i < means.size(); ++i) {
        long double sumOfSquareDifferences = 0.0;
        for (unsigned j = 0; j < windowSize; ++j) {
            sumOfSquareDifferences += (tickProcessor.getValue(i, j) - means[i]) * (tickProcessor.getValue(i, j) - means[i]);
        }
        stdevs[i] = sqrt(sumOfSquareDifferences / (windowSize - 1));
    }
}

void CrossCorrAnalyzer::computeCC() {
    for (unsigned i = 0; i < cc.n_rows; ++i) {
        for (unsigned j = 0; j < cc.n_cols; ++j) {
            if (cc(i, j) > 0.0) continue;
            long double numerator = 0.0;
            for (unsigned k = 0; k < windowSize; ++k) {
                numerator += (tickProcessor.getValue(i, k) - means[i]) * (tickProcessor.getValue(j, k) - means[j]);
            }
            cc(i, j) = numerator / ((long double)(windowSize - 1) * stdevs[i] * stdevs[j]);
            cc(j, i) = cc(i, j);
        }
    }
}

void CrossCorrAnalyzer::computeEigenDecomp() {
    arma::eig_sym(eigval, eigvec, cc);

    std::cout << "size of eigval is " << size(eigval) << std::endl;
    std::cout << "size of eigvec is " << size(eigvec) << std::endl;
}

void CrossCorrAnalyzer::computeCholeskyDecomps() {
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

void CrossCorrAnalyzer::processNextWindow() {
    computeMeans();
    computeStdevs();
    computeCC();
}

void CrossCorrAnalyzer::run() {
    bool done = false;
    
    while (!done) {
        processNextWindow();

        auto idx = tickProcessor.iters[0] - tickManager.tick_store[0].begin();
        // std::cout << "iterator index is " << idx << std::endl;
        
        if (idx + windowSize > tickManager.tick_store[0].size()) {
            done = true;
            continue;
        }

        std::cout << "cross correlation matrix for closing prices of input symbols between " << tickProcessor.iters[0]->time << " and " << (tickProcessor.iters[0] + windowSize)->time << std::endl;
        cc.print();

        // slides window to next position in the data
        for (uint i = 0; i < tickProcessor.iters.size(); ++i) {
            tickProcessor.iters[i] += windowSize;
        }

        // could probably have the analyzer take another object which handles additional processing on the cc matrix, such as the eigen and cholesky decompositions

        computeEigenDecomp();
        computeCholeskyDecomps();

        resetCC();
    }
}