#ifndef CURR_CONVERT
#define CURR_CONVERT 1

class CurrencyConversions {
public:
    static void removeCommasfromCurrency(std::string &str);
    static long double stodpre(std::string const &str, int const p);
};

#endif