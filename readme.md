# btester - backtester / trading engine

```
Usage:
  btester v0.0.2 [OPTION...]

  -i, --input arg   Input csv file, will read ticks from here (for strategy mode), csv file containing symbol data locations for analysis mode
  -o, --output arg  Output csv file, will output trade / results here
                    (default: output.csv)
  -a, --analysis    Use analysis mode instead of applying a signal generating strategy to the data
  -d, --debug       Enable debugging
  -h, --help        Print usage
 ```

### Key Terms
    - Tick: a standard stock candle
    - TickManager: responsible for fetching ticks from any source e.g web/database
    - TradeManager: implements basic operations for your trading exchange/portal.
    - Strategy: implements processTick()

### Examples

To download data for multiple symbols at once, create a text file containing all the desired symbols and use the `getYahooHD.sh` script as follows

```bash
./getYahooHD.sh syms.txt 07/20/2015 07/18/2023
```

where the second and third arguments are the start and end dates for the data you would like to retrieve