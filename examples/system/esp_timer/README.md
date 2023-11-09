| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP Timer Example (High Resolution Timer)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the ESP Timer feature. For detailed information on the functions and procedures used in this example, see [ESP Timer API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html).

> **Note**: By default, all links to [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/index.html) refer to the documentation for the ESP32. After clicking such a link, please remember to select your Espressif chip in the dropdown menu in the top-left corner.


## Overview

The ESP Timer facilitates creation and handling of timers that invoke callback functions (dispatch callbacks) on timeout. ESP Timer is mainly used to perform delayed or periodic actions, such as delayed device start/stop or periodic sampling of sensor data.

ESP Timer hides the complexity associated with managing multiple timers, dispatching callbacks, accounting for clock frequency changes (if dynamic frequency scaling is enabled), and maintaining correct time after light sleep.


## Usage

The subsections below give only absolutely necessary information. For full steps to configure ESP-IDF and use it to build and run projects, see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started).


### Required Hardware

* An Espressif development board
* USB A/micro USB B cable for power supply and serial communication
* Computer running Windows, Linux, or macOS

### Set Chip Target

In your example project directory, [set the chip target](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/idf-py.html#select-the-target-chip-set-target):

```
idf.py set-target <target>
```

For example, to set esp32 as the chip target, run

```
idf.py set-target esp32
```


### Review Kconfig Options

This project has some Kconfig options set as default for this particular example in [sdkconfig.defaults](./sdkconfig.defaults). For more information about those and other Kconfig options, see [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html).

To conveniently check or modify Kconfig options for this example, run:

```
idf.py menuconfig
```


### Build and Flash

Build the project, flash it to your development board, then run the monitor tool to view serial output:

```
idf.py build flash monitor
```

If the above command fails, try specifying your board's serial port name explicitly:

```
# Replace PORT with your board's serial port name
idf.py -p PORT build flash monitor
```

### Exit the Serial Monitor

To exit the serial monitor, use `Ctrl` + `]`.


## Example Output

If you see the following output log, your example should be running correctly:

```
...
I (294) example: Started timers, time since boot: 9662 us
periodic            500000        509644          1          0          0             0
one-shot                 0       5009654          1          0          0             0
I (794) example: Periodic timer called, time since boot: 509694 us
I (1294) example: Periodic timer called, time since boot: 1009671 us
I (1794) example: Periodic timer called, time since boot: 1509671 us
I (2294) example: Periodic timer called, time since boot: 2009671 us
periodic            500000       2509644          1          4          0           542
one-shot                 0       5009654          1          0          0             0
I (2794) example: Periodic timer called, time since boot: 2509671 us
I (3294) example: Periodic timer called, time since boot: 3009671 us
I (3794) example: Periodic timer called, time since boot: 3509671 us
I (4294) example: Periodic timer called, time since boot: 4009671 us
periodic            500000       4509644          1          8          0          1026
one-shot                 0       5009654          1          0          0             0
I (4794) example: Periodic timer called, time since boot: 4509671 us
I (5294) example: Periodic timer called, time since boot: 5009669 us
I (5294) example: One-shot timer called, time since boot: 5009788 us
I (5294) example: Restarted periodic timer with 1s period, time since boot: 5012675 us
I (6294) example: Periodic timer called, time since boot: 6012692 us
periodic           1000000       7012666          2         11          0          1391
one-shot                 0             0          1          1          0         11472
I (7294) example: Periodic timer called, time since boot: 7012692 us
I (8294) example: Periodic timer called, time since boot: 8012692 us
periodic           1000000       9012666          2         13          0          1639
one-shot                 0             0          1          1          0         11472
I (9294) example: Periodic timer called, time since boot: 9012692 us
I (10294) example: Periodic timer called, time since boot: 10012692 us
I (10314) example: Entering light sleep for 0.5s, time since boot: 10024351 us
I (10314) example: Woke up from light sleep, time since boot: 10525143 us
...
```


## Example Breakdown

The subsections below walk you through the important parts of the application example.

### Creating Callback Functions

Timers are used to execute a callback function as a delayed action. So the callback functions `periodic_timer_callback()` and `oneshot_timer_callback()` are the most important parts of this application example. They are defined for the periodic and one-shot timers respectively.


### Setting Timer Configuration Parameters

The struct `esp_timer_create_args_t` is used to initialize a timer. Its parameters are described in *ESP Timer API Reference* > [esp_timer_create_args_t](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html#esp_timer_create_args_t).

The defined struct instance is then used in the function `esp_timer_create()`.


### Creating and Starting Timers

To run a timer, it needs to be created first using the function `esp_timer_create()` and then started using either `esp_timer_start_periodic()` or `esp_timer_start_once()` depending on the timer type.


### Printing Timer Dumps

The function `esp_timer_dump()` is used to print the timer dumps which can be useful for debugging purposes. For details, see *ESP Timer API Reference* > [Debugging Timers](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html#debugging-timers).

To make the output of the dump function more detailed, this example's file `sdkconfig.defaults` has the option `CONFIG_ESP_TIMER_PROFILING` set (see [Review Kconfig Options](#review-kconfig-options)).


### Entering and Waking Up from Light Sleep

To demonstrate that timekeeping continues correctly after light sleep, the example briefly enters light sleep using `esp_sleep_enable_timer_wakeup()` and `esp_light_sleep_start()`.

During light sleep, the CPU is not running, so callbacks cannot be dispatched. On wakeup, the system attempts to execute all unhandled callbacks if any, then ESP Timer resumes its normal operation.


### Stopping and Deleting Timers

Once the timers are not needed anymore, they are stopped and deleted to free up memory using `esp_timer_stop()` and `esp_timer_delete()`.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.


## Reference

- [ESP-IDF ESP Timer](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started)
- [Kconfig Options](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html)
