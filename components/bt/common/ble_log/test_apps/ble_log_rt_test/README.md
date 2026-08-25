# BLE Log Runtime Test App

## Overview

On-target regression and latency suite for the BLE Log runtime dispatch layer:
the esp_timer deferred batch dispatch and the submission ownership gate.

The app runs on real hardware (any BLE-capable chip, currently tested on
ESP32-C6 and ESP32-C5). The transport is replaced by a software model
(`CONFIG_BLE_LOG_PRPH_TEST=y`) that mimics DMA ownership transfer and stamps
the hand-over time, so dispatch latency is measured without a real link.

## Test Cases

- `runtime dispatch latency`: 32 single submissions and 32 four-transport
  bursts. Verifies exact delivery order/count and prints enqueue-to-receipt
  min/avg/p50/p95/max latency (`BLE_LOG_RT_PERF` lines) for the single,
  first-in-burst, and last-in-burst paths.
- `runtime` regressions: shared ESP Timer task fairness, periodic timestamp
  delivery without light-sleep wakeups, ISR-only submission delivery,
  consumer-independent receipt timing, extra-marker detection, exact 1 ms
  first-submission defer scheduling, full task-pool snapshot dispatch, deinit
  racing submissions (SMP-pinned writer, failure-safe recovery), bounded
  inflight-peak statistics, and monotonic millisecond waits at both supported
  tick rates.

The first-deadline and full task-pool regressions require dispatch exclusion
while enqueueing, so they run on single-core builds only (they are ignored on
SMP targets). The deinit-race regression pins its writer to the other core on
SMP.

## Build, Flash, Run

```bash
cd components/bt/common/ble_log/test_apps/ble_log_rt_test
idf.py set-target <chip>
idf.py -p <PORT> build flash monitor
```

Timeout regressions are verified at both supported tick rates: the default
build uses the production 100 Hz; the 1000 Hz variant has a dedicated overlay:

```bash
idf.py -B build_1000 -p <PORT> -D SDKCONFIG=sdkconfig.1000 \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.tick_1000" \
  set-target <chip> build flash monitor
```

The app boots into the Unity menu; enter a test number to run it.

## Parsing Results

The latency lines can be rendered into tables/CSV with the parser shipped in
the perf test app:

```bash
idf.py -p <PORT> monitor | tee capture.log
python3 ../ble_log_perf_test/tools/parse_perf_log.py capture.log
python3 ../ble_log_perf_test/tools/parse_perf_log.py capture.log --csv out.csv
```

## Supported Targets

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

CI builds are temporarily disabled until BLE Log test runners are available.
