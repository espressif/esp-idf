| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Heap Task Tracking Example

## Overview

The example creates a task which allocates random amount of memory in each iteration and demonstrates use of internal API to get heap info on per task basis running in a system.

Heap task tracking feature has dependency on some of the internal heap debugging features (e.g. heap poisoning) which allows to store task control block in metadata of each heap block.

This adds small memory overhead on per heap block and hence this feature should be used for debugging purpose only.

### Configure the project

To change the `Heap Corruption Detection level`, open the project configuration menu (`idf.py menuconfig`).

Navigate to `Component config -> Heap memory debugging` menu. In `Heap corruption detection` menu select either "Light Impact" or "Comprehensive".

**Note:** Enabling “Comprehensive” detection has a substantial runtime performance impact.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
Task: Pre-Scheduler allocs -> CAP_8BIT: 5360 CAP_32BIT: 0
Task: esp_timer -> CAP_8BIT: 1724 CAP_32BIT: 0
Task: ipc0 -> CAP_8BIT: 8316 CAP_32BIT: 0
Task: main -> CAP_8BIT: 3480 CAP_32BIT: 0
Task: ipc1 -> CAP_8BIT: 12 CAP_32BIT: 0
Task: example_task -> CAP_8BIT: 696 CAP_32BIT: 0
```
