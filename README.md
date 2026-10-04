# Low-Latency Limit Order Book & Matching Engine

A C++17 implementation of a simplified electronic trading order book and matching engine. The project is designed as a learning project around market microstructure, data structures, order matching, and performance measurement.

## Current scope

- Limit and market orders
- Buy and sell order books
- Price-time priority
- Partial fills
- Order cancellation
- Trade generation
- Integer price representation to avoid floating-point price errors
- Basic nanosecond timing around a matching operation
- CMake build system
- Unit tests

## Architecture

```text
Order Submission
      |
      v
+------------------+
|    OrderBook     |
|                  |
| Buy price levels |
| Sell price levels|
+--------+---------+
         |
         v
+------------------+
| Matching Engine  |
+--------+---------+
         |
         v
     Trade Events
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/hft_engine
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Next phases

This repository is intentionally a starting point. Planned extensions include:

- Benchmark harness with large synthetic order streams
- Latency distribution and throughput measurements
- Better order-storage structures and allocation strategies
- Multithreaded ingestion architecture
- Market-data replay
- TCP/UDP feed simulation
- Linux performance profiling
- Cache-aware optimizations

Performance numbers will be added only after they are measured on the target machine.
