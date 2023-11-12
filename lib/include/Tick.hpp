#ifndef TICK_H
#define TICK_H 1

class Tick {
    public:
        long double open;
        long double low;
        long double close;
        long double high;
        long double adjust_close;
        long double vwap;
        long int volume;
        long int time;
        long int txs;

        void dump();
};

#endif