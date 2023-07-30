#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include "constants.hpp"
#include "Tick.hpp"
#include "TickManager.hpp"

class YFMultiFileTickManager : public TickManager {
public:
    YFMultiFileTickManager(std::string fl);
    std::vector<std::string> symbols;
    std::vector<Tick> getNextTickV();
    int tick_counter;
    std::vector<std::vector<Tick>> tick_store;
    Tick parseTickfromString(std::string& line);
    long int parseDatefromString(std::string& date);
    bool hasNextTick();
};