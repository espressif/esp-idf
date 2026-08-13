# BLE Log Performance Test App

## Overview

Performance test app for the ESP-IDF BLE Log module. It measures the system-level
behavior and per-API CPU cost of the Log Buffer Management (LBM) layer, so that
two LBM designs can be compared before and after an optimization.

The test runs on real hardware (any BLE-capable chip, currently tested on
ESP32-C6). The transport is replaced by a software model
(`CONFIG_BLE_LOG_PRPH_TEST=y`) that mimics DMA ownership transfer and link
bandwidth, so the measurements isolate the LBM layer itself.

## What Is Measured

| Dimension | Metrics |
| --- | --- |
| Throughput | sustained B/s against simulated links of 2 Mbps / 20 Mbps / unlimited, under a mixed load of all writer types |
| Frame loss | per-writer failed counts cross-checked against the LBM's own lost-frame statistics; both must match |
| API cycles | per-call CPU cycles of `ble_log_write_hex`, `ble_log_write_hex_ll` (task/ISR/append), and the compressed-log entry points, with avg/p50/p95/p99/max |
| Compression split | encode vs downstream write_hex cost, derived from a companion writer at the same record length (no instrumentation in the module) |

## Test Cases

- `throughput`: fixed 32B / 64B / 128B / mixed 8-64B payload profiles, each at
  2 Mbps, 20 Mbps, and unlimited link. Runs 3 write_hex writers + LL task + LL
  HCI + compressed writer + 1 kHz ISR writer concurrently.
- `write_hex cycles`: single writer, no link cap, payload 8/32/64/128 B.
- `write_hex drop path cycles`: saturated 2 Mbps link, measures the cost of a
  failed (dropped) write.
- `write_hex_ll cycles`: payload 8/32/64/128 B; plus a 32+32 B append case.
- `compressed write cycles`: workload matrix of the compressed entry points —
  U32 args (0/1/2/mixed), U64 values (full 8B / leading-zero LZ / zero),
  strings (8B / 128B), raw buffer (128B), and the `hex_printv` va_list entry.

## Build, Flash, Run

```bash
cd components/bt/common/ble_log/test_apps/ble_log_perf_test
idf.py set-target <chip>
idf.py -p <PORT> build flash monitor
```

The app boots into the Unity menu; enter a test number to run it. Every run
prints a `BLE_LOG_PERF` block (blank-line separated) with writer stats,
throughput, flush cost, and LBM statistics.

## Parsing Results

Capture the monitor output and turn it into tables/CSV:

```bash
idf.py -p <PORT> monitor | tee capture.log
python3 tools/parse_perf_log.py capture.log              # markdown tables
python3 tools/parse_perf_log.py capture.log --csv out.csv
```

Run the same capture twice (old vs new LBM) and diff the CSV.

## Supported Targets

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

CI builds are temporarily disabled until BLE Log test runners are available.
