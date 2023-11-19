#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "CurrencyConversions.hpp"
#include "PolygonFileTickManager.hpp"

PolygonFileTickManager::PolygonFileTickManager(std::string file_path) {
    
    if (DEBUG_FLAG) {
        std::cout
            << "Running with PolygonFileTickManager as default TickManager"
            << std::endl;
    }

    std::ifstream input_file (file_path);
    int first_line = 0;
    for (std::string line; std::getline(input_file, line);) {
        // we ignore the first line since it contains the headers
        if (first_line == 0) {
            ++first_line;
            continue;
        }

        // not sure if polygon files can have null candles - keeping this here for now so they are skipped if they exist
        if (line.find("null") != std::string::npos) {

            if (DEBUG_FLAG) {
                std::cout << "Null candle found, ignoring line - " << line << std::endl;
            }

            continue;
        }

        if (DEBUG_FLAG) {
            std::cout << "Trying to parse line - " << line << std::endl;
        }

        this->tick_store.push_back(parseTickfromString(line));
    }
}

long int PolygonFileTickManager::parseDatefromString(std::string line) {
    // int date;
    line.erase(std::remove(line.begin(), line.end(), '-'), line.end());
    return std::stol(line);
}

Tick PolygonFileTickManager::parseTickfromString(std::string line) {

    Tick parsed_tick;
    // std::vector<std::string> tokens;
    // std::stringstream string_stream(line);
    std::string token;
    std::string delim = ",";
    int col = 0;

    // std::cout << line << std::endl;
    // std::cin.get();

    size_t pos = 0;
    // while (std::getline(string_stream, token, ',')) {
    while ((pos = line.find(delim)) != std::string::npos) {
        token = line.substr(0, pos);

        // if (col > 0) {
        //     CurrencyConversions::removeCommasfromCurrency(token);
        // }

        if (col == 0) {         // open
            parsed_tick.open = std::stold(token);

            // std::cout << token << " " << parsed_tick.open << std::endl;
            // std::cin.get();
        } else if (col == 1) {  // high
            parsed_tick.high = std::stold(token);

            // std::cout << token << " " << parsed_tick.high << std::endl;
            // std::cin.get();
        } else if (col == 2) {  // low
            parsed_tick.low = std::stold(token);

            // std::cout << token << " " << parsed_tick.low << std::endl;
            // std::cin.get();
        } else if (col == 3) {  // close 
            parsed_tick.close = std::stold(token);

            // std::cout << token << " " << parsed_tick.close << std::endl;
            // std::cin.get();
        } else if (col == 4) {  // volume
            parsed_tick.volume = std::stol(token);

            // std::cout << token << " " << parsed_tick.volume << std::endl;
            // std::cin.get();
        } else if (col == 5) {  // vwap
            if (parsed_tick.volume == 0) {
                parsed_tick.vwap = 0.0;
            } else {
                parsed_tick.vwap = std::stold(token);
            }

            // std::cout << token << " " << parsed_tick.vwap << std::endl;
            // std::cin.get();
        } else if (col == 6) {  // timestamp
            parsed_tick.time = std::stol(token);

            // std::cout << token << " " << parsed_tick.time << std::endl;
            // std::cin.get();
        } else {                // col 7 is txs
            
        }

        ++col;
        line.erase(0, pos + delim.length());
    }

    // std::cout << line << std::endl;

    // getting the last column here
    if (parsed_tick.volume == 0) {
        parsed_tick.txs = 0;
    } else {
        parsed_tick.txs = std::stol(line);
    }

    // std::cout << line << " uhhhhh " << parsed_tick.txs << std::endl;
    // std::cin.get();

    return parsed_tick;
}

Tick PolygonFileTickManager::getNextTick() {
    int idx = this->tick_counter;
    this->tick_counter++;
    return this->tick_store[idx];
}

// todo: this is not implemented yet. could probably get the index of last tick in the tick store map
// and then return the following tick
Tick PolygonFileTickManager::getNextTick(Tick last_tick) {
    return this->getNextTick();
}

bool PolygonFileTickManager::hasNextTick() {
    // std::cout << tick_counter << " " << tick_store.size() << std::endl;
    bool hnt = this->tick_counter < this->tick_store.size();
    return hnt;
}

size_t PolygonFileTickManager::getTickStoreSize() {
    return this->tick_store.size();
}