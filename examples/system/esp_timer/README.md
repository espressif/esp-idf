| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP Timer Example (High Resolution Timer)

(For general overview of examples and their usage, see the [README](../../README.md) file in the upper level `examples` directory.)

> **Note**:
>
> After you click any link to [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/index.html), go to the top of the sidebar, then make sure you have the appropriate **Espressif chip** (target) and **ESP-IDF version** selected in the dropdown menus.

This example shows how to use the [ESP Timer](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html) feature to create timers and execute callback functions. On the linked page, you will find detailed information about the feature as well as the functions and procedures used in this example.

In addition, the [Sleep Modes](https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/sleep_modes.html) feature is used to briefly enter light sleep. This demonstrates that timekeeping continues correctly after light sleep.

The example starts by creating and starting a periodic and a one-shot timer. Their callback functions print the time elapsed since the example was booted. Then the `oneshot_timer_callback()` function redefines the period of the periodic timer and restarts it.

After that, the chip enters light sleep for 0.5 seconds. Once the chip wakes up, the timers execute a couple of more callback functions, then the timers are stopped and deleted to free up memory.


## Usage

The subsections below give only absolutely necessary information. For full steps to configure ESP-IDF and use it to build and run projects, see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started).


### Hardware Required

* An Espressif development board based on a chip listed in supported targets
* A USB cable for power supply and serial communication
* Computer with ESP-IDF installed and configured


### Set Chip Target

First of all, your target must be supported by both:

- **By your ESP-IDF version**: For the full list of supported targets, run:
  ```
  idf.py --list-targets
  ```
- **By this example**: For the full list of supported targets,  refer to the supported targets table at the top of this README.

After you make sure that your target is supported, go to your example project directory and [set the chip target](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/idf-py.html#select-the-target-chip-set-target):

```
idf.py set-target <target>
```

For example, to set esp32 as the chip target, run:

```
idf.py set-target esp32
```


### Configure the Project

This example does not need configuration. The required Kconfig options differing from the ESP-IDF defaults are pre-set for this particular example in [sdkconfig.defaults](./sdkconfig.defaults).

For more information about those and other Kconfig options, see [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html#esp-timer-high-resolution-timer) > ESP Timer (High Resolution Timer).

To conveniently check or modify Kconfig options for this example in a project configuration menu, run:

```
idf.py menuconfig
```


### Build and Flash

Execute the following command to build the project, flash it to your development board, and run the monitor tool to view the serial output:

```
idf.py build flash monitor
```

This command can be reduced to `idf.py flash monitor`.

If the above command fails, check the log on the serial monitor which usually provides information on the possible cause of the issue.

To exit the serial monitor, use `Ctrl` + `]`.


## Example Output

If you see the following console output, your example should be running correctly:

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

Timers are used to execute a callback function as a delayed action. So the callback functions `periodic_timer_callback()` and `oneshot_timer_callback()` are crucial parts of this application example.


### Printing Timer Dumps

The function `esp_timer_dump()` is used to print the timer dumps which can be useful for debugging purposes. For details, see *ESP Timer API Reference* > [Debugging Timers](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html#debugging-timers).

To make the output of the dump function more detailed, this example's file `sdkconfig.defaults` has the option `CONFIG_ESP_TIMER_PROFILING` set (see [Configure the Project](#configure-the-project)).


### Entering and Waking Up from Light Sleep

To demonstrate that timekeeping continues correctly after light sleep, the example briefly enters light sleep using `esp_sleep_enable_timer_wakeup()` and `esp_light_sleep_start()`.

During light sleep, the CPU is not running, so callbacks cannot be dispatched. On wakeup, the system attempts to execute all unhandled callbacks if any, then ESP Timer resumes its normal operation.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub.


## Reference

- [ESP-IDF: ESP Timer feature](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html)
- [ESP-IDF: Sleep Modes feature](https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/sleep_modes.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started)
- [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html#esp-timer-high-resolution-timer) (Kconfig Options)
