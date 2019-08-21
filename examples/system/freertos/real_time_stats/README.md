# FreeRTOS Real Time Stats Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

FreeRTOS provides the function `vTaskGetRunTimeStats()` to obtain CPU usage statistics of tasks. However, these statistics are with respect to the entire runtime of FreeRTOS (i.e. **run time stats**). Furthermore, statistics of `vTaskGetRunTimeStats()` are only valid whilst the timer for run time statistics has not overflowed.

This example demonstrates how to get CPU usage statistics of tasks with respect to a specified duration (i.e. **real time stats**) rather than over the entire runtime of FreeRTOS. The `print_real_time_stats()` function of this example demonstrates how this can be achieved.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

* Enable FreeRTOS to collect run time stats under `Component Config->FreeRTOS`

* The clock source of reference timer used for FreeRTOS statistics can be configured under `Component Config->FreeRTOS`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
Getting real time stats over 100 ticks
| Task | Run Time | Percentage
| stats | 1304 | 0%
| IDLE0 | 206251 | 10%
| IDLE1 | 464785 | 23%
| spin2 | 225389 | 11%
| spin0 | 227174 | 11%
| spin4 | 225303 | 11%
| spin1 | 207264 | 10%
| spin3 | 225331 | 11%
| spin5 | 225369 | 11%
| Tmr Svc | 0 | 0%
| esp_timer | 0 | 0%
| ipc1 | 0 | 0%
| ipc0 | 0 | 0%
Real time stats obtained
```

- When compiled in dual core mode, the percentage is with respect to the combined run time of both CPUs. Thus, `50%` would indicate full utilization of a single CPU. 
- In single core mode, the percentage is with respect to a single CPU. Thus, `100%` would indicate full utilization of the CPU.

The unit of `Run Time` is the period of the timer clock source used for FreeRTOS statistics.

## Troubleshooting

```
Getting real time stats over 100 ticks
Error getting real time stats
```
If the above is output when running the example, users should check the return value of `print_real_time_stats()` to determine the reason for failure.
