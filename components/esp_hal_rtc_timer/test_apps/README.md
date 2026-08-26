| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# RTC timer HAL tests

This test app validates the `esp_hal_rtc_timer` component directly.

It focuses on:

1. Reading RTC timer counters.
2. Converting RTC slow-clock ticks into elapsed time and checking the result against a measured delay.
3. Exercising alarm interrupt raise/clear behavior on targets with the newer multi-target RTC timer hardware.
