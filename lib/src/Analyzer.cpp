#include "Analyzer.hpp"

void Analyzer::initCC() {
    cc.iters.resize(tickManager.tick_store.size());
    for (uint i = 0; i < cc.iters.size(); ++i) {
        cc.iters[i] = tickManager.tick_store[i].begin();
    }
}

void Analyzer::run() {
    bool done = false;

    initCC();
    
    while (!done) {
        cc.processNextWindow();

        auto idx = cc.iters[0] - tickManager.tick_store[0].begin();
        // std::cout << "iterator index is " << idx << std::endl;
        
        if (idx + windowSize > tickManager.tick_store[0].size()) {
            done = true;
            continue;
        }

        std::cout << "cross correlation matrix for closing prices of input symbols between " << cc.iters[0]->time << " and " << (cc.iters[0] + windowSize)->time << std::endl;
        cc.print();

        // slides window to next position in the data
        for (uint i = 0; i < cc.iters.size(); ++i) {
            cc.iters[i] += windowSize;
        }

        cc.computeEigenDecomp();
        cc.computeCholeskyDecomps();

        cc.resetCC();
    }
}