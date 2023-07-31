#pragma once

#include <vector>
#include "Tick.hpp"
#include "YFMultiFileTickManager.hpp"
#include "TickProcessor.hpp"

class MultiFileTickProcessor : public TickProcessor {
public:
    MultiFileTickProcessor(YFMultiFileTickManager& tm) {
        iters.resize(tm.tick_store.size());
        for (uint i = 0; i < iters.size(); ++i) {
            iters[i] = tm.tick_store[i].begin();
        }
    }

    long double getValue(unsigned i, unsigned j) final { return (*(iters[i] + j)).close; }
};