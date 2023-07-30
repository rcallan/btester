#include <numeric>
#include <cmath>
#include "CrossCorr.hpp"

void CrossCorr::computeMeans() {
    for (unsigned i = 0; i < means.size(); ++i) {
        means[i] = std::accumulate(iters[i], iters[i] + windowSize, 0.0, [](long double a, const Tick& b) { return a + b.close; }) / windowSize;
    }
}

void CrossCorr::computeStdevs() {
    for (unsigned i = 0; i < means.size(); ++i) {
        long double sumOfSquareDifferences = 0.0;
        for (unsigned j = 0; j < windowSize; ++j) {
            sumOfSquareDifferences += ((iters[i] + j)->close - means[i]) * ((iters[i] + j)->close - means[i]);
        }
        stdevs[i] = sqrt(sumOfSquareDifferences / (windowSize - 1));
    }
}

void CrossCorr::computeCC() {
    for (unsigned i = 0; i < cc.n_rows; ++i) {
        for (unsigned j = 0; j < cc.n_cols; ++j) {
            if (cc(i, j) > 0.0) continue;
            long double numerator = 0.0;
            for (unsigned k = 0; k < windowSize; ++k) {
                numerator += ((iters[i] + k)->close - means[i]) * ((iters[j] + k)->close - means[j]);
            }
            cc(i, j) = numerator / ((long double)windowSize * stdevs[i] * stdevs[j]);
            cc(j, i) = cc(i, j);
        }
    }
}

arma::mat CrossCorr::getCC() {
    return cc;
}

void CrossCorr::computeEigenDecomp() {
    arma::eig_sym(eigval, eigvec, cc);

    std::cout << "size of eigval is " << size(eigval) << std::endl;
    std::cout << "size of eigvec is " << size(eigvec) << std::endl;
}

void CrossCorr::computeCholeskyDecomps() {
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

void CrossCorr::processNextWindow() {
    computeMeans();
    computeStdevs();
    computeCC();
}