#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <limits>

#include "CurrencyConversions.hpp"
#include "YFMultiFileTickManager.hpp"

YFMultiFileTickManager::YFMultiFileTickManager(std::string fileList) {
    if (DEBUG_FLAG) {
        std::cout
            << "Running with YahooFinanceFileTickManager as default TickManager"
            << std::endl;
    }

    std::ifstream inFile (fileList);
    for (std::string line; std::getline(inFile, line);) {
        symbols.push_back(std::move(line));
    }

    std::vector<std::ifstream> ifiles(symbols.size());

    for (int i = 0; i < ifiles.size(); ++i) {
        ifiles[i] = std::ifstream(symbols[i]);
        tick_store.push_back(std::vector<Tick>());
    }

    std::vector<int> firstLines(ifiles.size());

    bool done = false;
    uint counter = 0;

    long latestTime = std::numeric_limits<long>::min();
    std::vector<bool> flags(ifiles.size(), true);
    std::vector<Tick> temp(ifiles.size());

    std::string line;

    // should loops through all files and only add data to tick store if all files have data for some particular date value
    while (!done) {
        ++counter;
        for (int i = 0; i < ifiles.size(); ++i) {
            if (counter <= 2 || flags[i]) {
                
                std::getline(ifiles[i], line);
                if (ifiles[i].eof() || line == "") {
                    std::cout << "finished parsing and equalizing input data" << std::endl;
                    done = true;
                    break;
                }

                // we ignore the first line since it contains the table headers
                if (firstLines[i] == 0) {
                    ++firstLines[i];
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

                // std::cout << line << std::endl;

                Tick currentTick = parseTickfromString(line);

                // tick_store[i].push_back(currentTick);
                temp[i] = currentTick;
                // std::cout << "current tick time is " << currentTick.time << std::endl;
                latestTime = (currentTick.time > latestTime) ? currentTick.time : latestTime;
                // std::cout << "latest time is " << latestTime << std::endl;
            }
        }
        // if (first) first = false;
        bool allSame = true;
        for (int i = 0; i < ifiles.size(); ++i) {
            if (temp[i].time < latestTime) {
                // std::cout << "time and latest time are " << temp[i].time << " " << latestTime << std::endl;
                flags[i] = true;
                allSame = false;
            } else {
                flags[i] = false;
            }
        }
        if (allSame && counter > 1) {
            // std::cout << "all same for time " << latestTime << std::endl;
            for (int i = 0; i < ifiles.size(); ++i) {
                tick_store[i].push_back(std::move(temp[i]));
                flags[i] = true;
            }
        }
    }

    std::cout << "number of data points is " << tick_store[0].size() << std::endl;    
}

long int YFMultiFileTickManager::parseDatefromString(std::string& line) {
    // int date;
    line.erase(std::remove(line.begin(), line.end(), '-'), line.end());
    return std::stol(line);
}

Tick YFMultiFileTickManager::parseTickfromString(std::string& line) {
    Tick parsed_tick;
    // std::vector<std::string> tokens;
    // std::stringstream string_stream(line);
    std::string token;
    int col = 0;

    size_t last = 0;
    size_t next = 0;
    while ((next = line.find(",", last)) != std::string::npos) {
        // std::cout << line.substr(last, next - last) << std::endl;
        token = line.substr(last, next - last);
        last = next + 1;
    

    // while (std::getline(string_stream, token, ',')) {
    // while (std::getline(line, token, ',')) {

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
    // std::cout << s.substr(last) << std::endl;
    token = line.substr(last);
    parsed_tick.volume = std::stol(token);

    return parsed_tick;
}

std::vector<Tick> YFMultiFileTickManager::getNextTickV() {
    int idx = this->tick_counter;
    this->tick_counter++;
    return this->tick_store[idx];
}

// todo: not really sure why this function is here
std::vector<Tick> YFMultiFileTickManager::getNextTickV(Tick last_tick) {
    return this->getNextTickV();
}

bool YFMultiFileTickManager::hasNextTick() {
    bool hnt = (long unsigned int)this->tick_counter < this->tick_store.size();
    return hnt;
}