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
        for (unsigned i = 0; i < means.size(); ++i) {
            means[i] = std::accumulate(al.tickManager.tick_store[i].begin(), al.tickManager.tick_store[i].end(), 0.0, [](long double a, Tick b) { return a + b.close; }) / al.tickManager.tick_store[i].size();
        }

        for (unsigned i = 0; i < means.size(); ++i) {
            long double sumOfSquareDifferences = 0.0;
            for (unsigned j = 0; j < al.tickManager.tick_store[i].size(); ++j) {
                sumOfSquareDifferences += (al.tickManager.tick_store[i][j].close - means[i]) * (al.tickManager.tick_store[i][j].close - means[i]);
            }
            stdevs[i] = sqrt(1.0 / al.tickManager.tick_store[i].size() * sumOfSquareDifferences);
            std::cout << "stdevs " << i << " " << stdevs[i] << std::endl;
        }

        for (unsigned i = 0; i < cc.n_rows; ++i) {
            for (unsigned j = 0; j < cc.n_cols; ++j) {
                if (cc(i, j) > 0.0) continue;
                unsigned numVals = al.tickManager.tick_store[i].size();
                long double numerator = 0.0;
                for (unsigned k = 0; k < numVals; ++k) {
                    numerator += (al.tickManager.tick_store[i][k].close - means[i]) * (al.tickManager.tick_store[j][k].close - means[j]);
                }
                cc(i, j) = numerator / ((long double)numVals * stdevs[i] * stdevs[j]);
                cc(j, i) = cc(i, j);
            }
        }
    }

    void print() {
        std::cout << cc;
    }
    
};