| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# LP Core Touch Sensor example:

This example demonstrates how to program the LP coprocessor to read touch pad sensors.

LP Core program written in C can be found across `lp_core/main.c`. The build system compiles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the application running inside the main CPU loads ULP program into the `LP SRAM` memory region using `ulp_lp_core_load_binary` function. The main code then configures the ULP wakeup period and starts the coprocessor by using `ulp_lp_core_run`. Once the LP core program is started, it runs periodically, with the period set by the main program. The main program enables LP Core wakeup source and puts the chip into deep sleep mode.

The LP core program scans all touch pad sensors periodically. When the program finds a touch pad is active (touched), it captures the touch pad number in a variable and wakes up the main CPU. The main CPU reports which touch pad sensor was touched.

## How to use the example

### Hardware Required

* A development board with ESP32-P4

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

```
Not an LP core wakeup. Cause = 0
Initializing...
Touch [CH 0] enabled on GPIO2
Touch [CH 1] enabled on GPIO3
Touch [CH 2] enabled on GPIO4
Touch [CH 3] enabled on GPIO5
=================================
Initial benchmark and new threshold are:
Touch [CH 0] 0: 5177, 77         1: 5131, 76     2: 2541, 38
Touch [CH 1] 0: 5021, 75         1: 5043, 75     2: 2471, 37
Touch [CH 2] 0: 5103, 76         1: 5064, 75     2: 2495, 37
Touch [CH 3] 0: 4981, 74         1: 4999, 74     2: 2440, 36
LP core loaded with firmware successfully
Entering in deep sleep

// Touched channel 0, then wake-up by the LP core

LP core woke up the main CPU
T0 touched

Entering in deep sleep
```
