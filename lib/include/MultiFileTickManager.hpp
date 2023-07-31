#pragma once

#include <iostream>
#include "Tick.hpp"

class MultiFileTickManager {
public:
    std::vector<std::vector<Tick>> tick_store;

    // virtual Tick getNextTick(Tick last_tick) = 0;
    // virtual Tick getNextTick() = 0;
    virtual bool hasNextTick() = 0;
    virtual size_t getTickStoreSize() = 0;
};