| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

This test app collects `esp_cpu.h` coverage in one place.

It currently includes:

- safe Unity tests for basic CPU helpers and interrupt helpers
- a multi-stage reset test for `esp_cpu_reset()`
- a thread-pointer roundtrip test and per-core `esp_cpu_get_core_id()` coverage
- non-destructive sanity checks for breakpoint and watchpoint setup APIs
- non-recoverable breakpoint and watchpoint trigger tests, plus a multi-core stall test where supported
