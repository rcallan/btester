#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "CurrencyConversions.hpp"
#include "YahooFinanceFileTickManager.hpp"

YahooFinanceFileTickManager::YahooFinanceFileTickManager(std::string file_path) {
    if (DEBUG_FLAG) {
        std::cout
            << "Running with YahooFinanceFileTickManager as default TickManager"
            << std::endl;
    }

    std::ifstream input_file (file_path);
    int first_line = 0;
    for (std::string line; std::getline(input_file, line);) {
        // we ignore the first line since it contains the table headers
        if (first_line == 0) {
            ++first_line;
            continue;
        }

        // yahoo finance can have null candles - we skip those
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

long int YahooFinanceFileTickManager::parseDatefromString(std::string line) {
    // int date;
    line.erase(std::remove(line.begin(), line.end(), '-'), line.end());
    return std::stol(line);
}

Tick YahooFinanceFileTickManager::parseTickfromString(std::string line) {
    Tick parsed_tick;
    std::vector<std::string> tokens;
    std::stringstream string_stream(line);
    std::string token;
    int col = 0;

    while (std::getline(string_stream, token, ',')) {

        if (col > 0) {
            CurrencyConversions::removeCommasfromCurrency(token);
        }

        if (col == 0) { // date
            parsed_tick.time = parseDatefromString(token);
        } else if (col == 1) { // opening price
            parsed_tick.open = std::stold(token);
        } else if (col == 2) { // high
            parsed_tick.high = std::stold(token);
        } else if (col == 3) { // low
            parsed_tick.low = std::stold(token);
        } else if (col == 4) { // close 
            parsed_tick.close = std::stold(token);
        } else if (col == 5) { // adj close
            parsed_tick.adjust_close = std::stold(token);
        } else { // col 6 is volume
            parsed_tick.volume = std::stol(token);
        }

        ++col;
    }

    return parsed_tick;
}

Tick YahooFinanceFileTickManager::getNextTick() {
    int idx = this->tick_counter;
    this->tick_counter++;
    return this->tick_store[idx];
}

// todo: not really sure why this function is here
Tick YahooFinanceFileTickManager::getNextTick(Tick last_tick) {
    return this->getNextTick();
}

bool YahooFinanceFileTickManager::hasNextTick() {
    bool hnt = (long unsigned int)this->tick_counter < this->tick_store.size();
    return hnt;
}