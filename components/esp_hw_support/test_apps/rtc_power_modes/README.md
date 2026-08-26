| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- |

# RTC power test

This test app is to enter different sub power modes we have, so that the power consumption under different power modes can be measured.

See the api-reference/system/sleep_modes chapter in the Programming Guide for the details of these power modes.

(ESP32 to be added)
Changes:
- ESP32: DS 8M, DS Default, LS 8M, LS Default
- dbg_atten_slp NODROP when using INT8M as slow src on ESP32
