#include <numeric>
#include <cmath>
#include "CrossCorr.hpp"

void CrossCorr::computeMeans(std::vector<std::vector<Tick>::iterator>& iters) {
    for (unsigned i = 0; i < means.size(); ++i) {
        means[i] = std::accumulate(iters[i], iters[i] + al.windowSize, 0.0, [](long double a, Tick b) { return a + b.close; }) / al.windowSize;
    }
}

void CrossCorr::computeStdevs(std::vector<std::vector<Tick>::iterator>& iters) {
    for (unsigned i = 0; i < means.size(); ++i) {
        long double sumOfSquareDifferences = 0.0;
        for (unsigned j = 0; j < al.windowSize; ++j) {
            sumOfSquareDifferences += ((iters[i] + j)->close - means[i]) * ((iters[i] + j)->close - means[i]);
        }
        stdevs[i] = sqrt(sumOfSquareDifferences / (al.windowSize - 1));
    }
}

void CrossCorr::computeCC(std::vector<std::vector<Tick>::iterator>& iters) {
    for (unsigned i = 0; i < cc.n_rows; ++i) {
        for (unsigned j = 0; j < cc.n_cols; ++j) {
            if (cc(i, j) > 0.0) continue;
            unsigned numVals = al.windowSize;
            long double numerator = 0.0;
            for (unsigned k = 0; k < numVals; ++k) {
                numerator += ((iters[i] + k)->close - means[i]) * ((iters[j] + k)->close - means[j]);
            }
            cc(i, j) = numerator / ((long double)numVals * stdevs[i] * stdevs[j]);
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

void CrossCorr::processNextWindow(std::vector<std::vector<Tick>::iterator>& iters) {
    computeMeans(iters);
    computeStdevs(iters);
    computeCC(iters);
}