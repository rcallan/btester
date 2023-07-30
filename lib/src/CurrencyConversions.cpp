#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "CurrencyConversions.hpp"

void CurrencyConversions::removeCommasfromCurrency(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), ','), str.end());
}

long double CurrencyConversions::stodpre(std::string const &str, int const p) {
    std::stringstream sstrm;
    sstrm << std::setprecision(p) << std::fixed << str << std::endl;

    long double d;
    sstrm >> d;

    return d;
}