#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include "constants.hpp"
#include "Tick.hpp"
#include "MultiFileTickManager.hpp"

class YFMultiFileTickManager : public MultiFileTickManager {
public:
    std::vector<std::string> symbols;
    int tick_counter;

    YFMultiFileTickManager(std::string fl);
    Tick parseTickfromString(std::string& line);
    long int parseDatefromString(std::string& date);
    std::vector<Tick> getNextTickV();
    bool hasNextTick();
    size_t getTickStoreSize() final { return tick_store.size(); }
};