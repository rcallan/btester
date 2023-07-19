# btester - mini backtesting/trading engine

```bash
Usage:
  btester v0.0.1 [OPTION...]

  -i, --input arg   Input csv file, will read ticks from here
  -o, --output arg  Output csv file, will output trade / results here
                    (default: output.csv)
  -d, --debug       Enable debugging
  -h, --help        Print usage
 ```

### Key Terms
    - Tick: a standard stock candle
    - TickManager: responsible for fetching ticks from any source e.g web/database
    - TradeManager: implements basic operations for your trading exchange/portal.
    - Strategy: implements processTick()