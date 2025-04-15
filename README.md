# CALM (Consistent Asset and Liquidity Management) Trading System
**Active work in progress**

## About
This is an automated trading system using Interactive Broker's TWS C++ api.

"Consistent Asset and Liquidity Management" is the backronym for "calm", the state that traders and engineers wish to obtain 
in their own operations.

## Dependency
- spdlog `sudo apt install libspdlog-dev`
- Arrow (https://arrow.apache.org/install/)
- Hiredis (https://github.com/redis/hiredis/tree/master)
- MongoDB C++ Driver (https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/get-started/)

## Strategies
- Some simple strategies can be found at [calm strategies](https://github.com/wdwe/CALM-trading-strategies) which are 
used together with this trading system


## Work in Pipelines
- BacktestEngine (counterpart of `TradingEngine`) and Backtester (counterpart of `Trader` class)


## Future Performance Improvements
While good care and efforts are given to ensure the system is efficient and high-performance, ultra-low latency is not the main
consideration for this first iteration of design.

Below are some pointers for future/next iteration.
- [ ] use lock-free queue and atomic operations
- [ ] use more POD instead of shared pointers
- [ ] for hot path, use stack instead of heap storage (i.e. use `std::move` syntax with queue instead of creating shared
  pointer)
- [ ] use shared memory instead of message queue to bridge execution system and [algo/strategy program](https://github.com/wdwe/CALM-trading-strategies)
- [ ] replace the use of `string` with `char[]`/`string_view`/`inplace_string` 
- [ ] use more `curiously recurring template pattern` to speed up
- [ ] implement strategies in C++

## Todo
### Gateway/API
- [ ] handle TWS connection interruption e.g. reconnect and get order status
