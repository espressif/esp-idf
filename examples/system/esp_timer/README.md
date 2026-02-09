| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# ESP Timer Example (High Resolution Timer)

(For general overview of examples and their usage, see the [README](../../README.md) file in the upper level `examples` directory.)

> [!NOTE]
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
I (266) example: Started timers, time since boot: 30808 us
Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
periodic              500000      530773        1             0             0             0
timed periodic        500000      3000000       1             0             0             0
one-shot              0           5030792       1             0             0             0
timed one-shot        0           6000000       1             0             0             0
I (766) example: Periodic timer called, time since boot: 530850 us
I (1266) example: Periodic timer called, time since boot: 1030802 us
I (1766) example: Periodic timer called, time since boot: 1530802 us
I (2266) example: Periodic timer called, time since boot: 2030802 us
Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
periodic              500000      2530773       1             4             0             999
timed periodic        500000      3000000       1             0             0             0
one-shot              0           5030792       1             0             0             0
timed one-shot        0           6000000       1             0             0             0
I (2766) example: Periodic timer called, time since boot: 2530826 us
I (3236) example: Timed periodic timer called, time since boot: 3000029 us
I (3266) example: Periodic timer called, time since boot: 3030802 us
I (3736) example: Timed periodic timer called, time since boot: 3500029 us
I (3766) example: Periodic timer called, time since boot: 3530802 us
I (4236) example: Timed periodic timer called, time since boot: 4000029 us
I (4266) example: Periodic timer called, time since boot: 4030802 us
Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
timed periodic        500000      4500000       1             3             0             709
periodic              500000      4530773       1             8             0             1947
one-shot              0           5030792       1             0             0             0
timed one-shot        0           6000000       1             0             0             0
I (4736) example: Timed periodic timer called, time since boot: 4500053 us
I (4766) example: Periodic timer called, time since boot: 4530802 us
I (5236) example: Timed periodic timer called, time since boot: 5000030 us
I (5266) example: Periodic timer called, time since boot: 5030803 us
I (5266) example: One-shot timer called, time since boot: 5031025 us
I (5266) example: Restarted periodic timer with 1s period, time since boot: 5031920 us
I (5736) example: Timed periodic timer called, time since boot: 5500029 us
I (6236) example: Timed one-shot timer called, time since boot: 6000028 us
I (6236) example: Restarted timed periodic timer with 1s period, time since boot: 6000268 us
I (6266) example: Periodic timer called, time since boot: 6031945 us
Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
timed periodic        1000000     7000000       1             6             0             1438
periodic              1000000     7031916       2             11            0             2644
timed one-shot        0           0             1             1             0             3722
one-shot              0           0             1             1             0             8517
I (7236) example: Timed periodic timer called, time since boot: 7000053 us
I (7266) example: Periodic timer called, time since boot: 7031945 us
I (8236) example: Timed periodic timer called, time since boot: 8000029 us
I (8266) example: Periodic timer called, time since boot: 8031945 us
Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
timed periodic        1000000     9000000       1             8             0             1940
periodic              1000000     9031916       2             13            0             3106
timed one-shot        0           0             1             1             0             3722
one-shot              0           0             1             1             0             8517
I (9236) example: Timed periodic timer called, time since boot: 9000053 us
I (9266) example: Periodic timer called, time since boot: 9031945 us
I (10236) example: Timed periodic timer called, time since boot: 10000029 us
I (10266) example: Periodic timer called, time since boot: 10031945 us
I (10476) example: Entering light sleep for 0.5s, time since boot: 10239360 us
I (10476) example: Woke up from light sleep, time since boot: 10739673 us
I (10736) example: Timed periodic timer called, time since boot: 11000033 us
I (10766) example: Periodic timer called, time since boot: 11031945 us
I (11736) example: Timed periodic timer called, time since boot: 12000029 us
I (11766) example: Periodic timer called, time since boot: 12031945 us
I (12486) example: Stopped and deleted timers
...
```


## Example Breakdown

The subsections below walk you through the important parts of the application example.


### Creating Callback Functions

Timers are used to execute a callback function as a delayed action. So the callback functions `periodic_timer_callback()`, `timed_periodic_timer_callback()`, `oneshot_timer_callback()` and `timed_oneshot_timer_callback()`, are crucial parts of this application example.


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
