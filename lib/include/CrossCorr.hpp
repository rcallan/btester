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

    void process() {
        uint windowSize = 100;

        std::vector<std::vector<Tick>::iterator> iters(means.size());
        for (uint i = 0; i < iters.size(); ++i) {
            iters[i] = al.tickManager.tick_store[i].begin();
        }

        bool done = false;
        
        while (!done) {
            for (unsigned i = 0; i < means.size(); ++i) {
                // means[i] = std::accumulate(al.tickManager.tick_store[i].begin(), al.tickManager.tick_store[i].end(), 0.0, [](long double a, Tick b) { return a + b.close; }) / al.tickManager.tick_store[i].size();
                means[i] = std::accumulate(iters[i], iters[i] + windowSize, 0.0, [](long double a, Tick b) { return a + b.close; }) / windowSize;
            }

            for (unsigned i = 0; i < means.size(); ++i) {
                long double sumOfSquareDifferences = 0.0;
                for (unsigned j = 0; j < windowSize; ++j) {
                    sumOfSquareDifferences += ((iters[i]+j)->close - means[i]) * ((iters[i] + j)->close - means[i]);
                }
                // stdevs[i] = sqrt(1.0 / al.tickManager.tick_store[i].size() * sumOfSquareDifferences);
                stdevs[i] = sqrt(1.0 / windowSize * sumOfSquareDifferences);
                // std::cout << "stdevs " << i << " " << stdevs[i] << std::endl;
            }

            for (unsigned i = 0; i < cc.n_rows; ++i) {
                for (unsigned j = 0; j < cc.n_cols; ++j) {
                    if (cc(i, j) > 0.0) continue;
                    // unsigned numVals = al.tickManager.tick_store[i].size();
                    unsigned numVals = windowSize;
                    long double numerator = 0.0;
                    for (unsigned k = 0; k < numVals; ++k) {
                        // numerator += (al.tickManager.tick_store[i][k].close - means[i]) * (al.tickManager.tick_store[j][k].close - means[j]);
                        numerator += ((iters[i] + k)->close - means[i]) * ((iters[j] + k)->close - means[j]);
                    }
                    cc(i, j) = numerator / ((long double)numVals * stdevs[i] * stdevs[j]);
                    cc(j, i) = cc(i, j);
                }
            }

            auto idx = iters[0] - al.tickManager.tick_store[0].begin();
            std::cout << "iterator index is " << idx << std::endl;
            
            if (idx + windowSize > al.tickManager.tick_store[0].size()) {
                done = true;
                continue;
            }

            std::cout << "cross correlation matrix for closing prices of input symbols between " << iters[0]->time << " and " << (iters[0] + windowSize)->time << std::endl;

            for (uint i = 0; i < iters.size(); ++i) {
                iters[i] += windowSize;
            }

            print();

            // trying cholesky decomposition here

            arma::mat X = cc.t() * cc;

            arma::cx_vec eigval;
            arma::cx_mat eigvec;

            arma::eig_gen(eigval, eigvec, cc);
            arma::eig_gen(eigval, eigvec, cc, "balance");

            std::cout << "size of eigval is " << size(eigval) << std::endl;
            std::cout << "size of eigvec is " << size(eigvec) << std::endl;

            arma::mat R1 = chol(X);
            arma::mat R2 = chol(X, "lower");

            arma::mat R3;
            bool ok = arma::chol(R3, X);

            std::cout << "cholesky decomp okay: " << ok << std::endl;

            arma::mat R;
            arma::uvec P_vec;
            arma::umat P_mat;

            arma::chol(R, P_vec, X, "upper", "vector");
            arma::chol(R, P_mat, X, "lower", "matrix");

            // std::cout << "cholesky decomposition\n" << R << std::endl;
            std::cout << "cholesky decomposition\n" << R(5, 8) << std::endl;

            cc = arma::mat(means.size(), means.size());
        }
    }

    void print() {
        std::cout << cc << std::endl;
    }
    
};