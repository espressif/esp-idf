# C++ pthread Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Support for the [C++ threads](http://www.cplusplus.com/reference/thread/thread/) in ESP-IDF is implemented on top of the [ESP-pthread](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_pthread.html#overview) component. Thus, C++ threads created using the standard thread class constructor will automatically inherit the current ESP-pthread configuration. This example demonstrates how to leverage the thread configuration functions provided by ESP-pthread (e.g., `esp_pthread_get_default_config()` and `esp_pthread_set_cfg()`) to modify the stack sizes, priorities, names, and core affinities of the C++ threads. 

**Note: Due to the use of the C++ threads, this example is written in C++ instead of C.**

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

* The default ESP-pthread configuration may also be modified under `Component config > PThreads`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The following log output should appear when the example runs (note that the bootloader log has been omitted).

```
...
I (380) Thread 1: Core id: 0, prio: 5, minimum free stack: 2068 bytes.
I (0) pthread: This thread (with the default name) may run on any core.Core id: 1, prio: 5, minimum free stack: 2056 bytes.
I (390) Thread 1: This is the INHERITING thread with the same parameters as our parent, including name. Core id: 0, prio: 5, minimum free stack: 2092 bytes.
I (410) Thread 2: Core id: 1, prio: 5, minimum free stack: 2088 bytes.
I (410) main: core id: 0, prio: 1, minimum free stack: 2928 bytes.
```
