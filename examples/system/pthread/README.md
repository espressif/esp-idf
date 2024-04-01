| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Pthread Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

ESP-IDF provides implementations for many standard POSIX Threads (pthread) APIs. These APIs are impmlemented as thin wrappers around FreeRTOS APIs.

This example demonstrates how to create threads using this API, and how to use the ESP-IDF pthreads extension API to set default thread parameters for the related FreeRTOS tasks. For complete details about supported Pthreads APIs, consult the ESP-IDF Programmers Guide.

## How to use example

### Hardware Required

This example should be able to run on any supported Espressif SoC development board.

### Configure the project

```
idf.py set-target esp32
```

(Replace `esp32` with the name of the chip target you are using.)

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
Created thread 0x3ffaff74
Created larger stack thread 0x3ffb7ca8
This thread has ID 0x3ffb7ca8 and 15896 bytes free stack
This thread has ID 0x3ffaff74 and 2616 bytes free stack
Thread 0x3ffaff74 exiting
Thread 0x3ffb7ca8 exiting
Threads have exited

Created thread 0x3ffb44c8 with new default config
This thread has ID 0x3ffb44c8 and 32312 bytes free stack
Thread 0x3ffb44c8 exiting
Thread has exited
```
