| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

Runs a build test to check ldgen places libraries, objects and symbols
correctly as specified in the linker fragments. Specifically, this app
tests the placement for the main component, as specified in `main/linker.lf`
The Python script that performs the checks, `check_placements.py`, automatically
runs after the app is built.

ESP32-C2 doesn't have rtc memory, IDF-3834
