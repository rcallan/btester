#include <iostream>
#include <vector>

#include "constants.hpp"
#include "Tick.hpp"
#include "TickManager.hpp"

class PolygonFileTickManager : public TickManager {
public:
    PolygonFileTickManager(std::string file_path);
    Tick getNextTick(Tick last_tick);
    Tick getNextTick();
    int tick_counter {};
    std::vector<Tick> tick_store;
    Tick parseTickfromString(std::string line);
    long int parseDatefromString(std::string date);
    bool hasNextTick();
    size_t getTickStoreSize();
};