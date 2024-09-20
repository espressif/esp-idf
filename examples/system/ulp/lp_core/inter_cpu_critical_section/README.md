| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# LP Core simple example with inter-CPU critical section:

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview
This example demonstrates the use of an inter-CPU critical section to safeguard shared resources between the main program and the ULP (Ultra-Low-Power) processor.

In this example, a global shared counter `shared_cnt` is incremented by both the high-performance CPU (HP CPU) and the low-power CPU (LP CPU) in turn. To ensure mutual exclusion, a global inter-CPU spinlock is used. With this protection in place, both the HP and LP CPUs attempt to increment the shared counter 100,000 times each.

The inter-CPU critical section is implemented using a global spinlock of type ulp_lp_core_spinlock_t. This type of spinlock is especially designed to protect shared resources between main program and ULP program. It supports multiple threads to attempt to enter critical section simultaneously, while eventually only one thread can succeed. This spinlock must be declared within the ULP program and shall be initialized in either the main program or the ULP program before using it. Note that this critical section doesn't support nested entering and exiting.

## How to use this example

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

To observe the mutual exclusion between HP and LP CPUs straight forward, the output of LP CPU is routed to HP CPU‘s console. 

## Example output
```
LP CPU's increment starts, shared counter = 0
core 0 started, cnt = 5868
HP CPU's increment ends, shared counter =  165882
LP CPU's increment ends, shared counter = 250000
...
```