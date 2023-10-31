| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP-RISC-V Touch Sensor example:

This example demonstrates how to program the ULP RISC-V coprocessor to read touch pad sensors.

ULP program written in C can be found across `ulp/main.c`. The build system compiles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the application running inside the main CPU loads ULP program into the `RTC_SLOW_MEM` memory region using `ulp_riscv_load_binary` function. The main code then configures the ULP wakeup period and starts the coprocessor by using `ulp_riscv_run`. Once the ULP program is started, it runs periodically, with the period set by the main program. The main program enables ULP wakeup source and puts the chip into deep sleep mode.

The ULP Program scans all touch pad sensors periodically. When the ULP program finds a touch pad is active (touched), it captures the touch pad number in a variable and wakes up the main CPU. The main CPU reports which touch pad sensor was touched.

## How to use the example

### Hardware Required

* A development board with ESP32-S2 or ESP32-S3

The following capacitive touch pads are supported on ESP32-S2/S3:

```
  ----------------------------------------------------------
  | Touch Pad  |                  GPIO Pin                 |
  |------------|-------------------------------------------|
  |    T0      | Internal channel, not connected to a GPIO |
  |    T1      |                  GPIO1                    |
  |    T2      |                  GPIO2                    |
  |    T3      |                  GPIO3                    |
  |    T4      |                  GPIO4                    |
  |    T5      |                  GPIO5                    |
  |    T6      |                  GPIO6                    |
  |    T7      |                  GPIO7                    |
  |    T8      |                  GPIO8                    |
  |    T9      |                  GPIO9                    |
  |    T10     |                  GPIO10                   |
  |    T11     |                  GPIO11                   |
  |    T12     |                  GPIO12                   |
  |    T13     |                  GPIO13                   |
  |    T14     |                  GPIO14                   |
  ----------------------------------------------------------
```

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
Not a ULP wakeup, initializing it!
Entering in deep sleep

...

ULP-RISC-V woke up the main CPU!
T3 touched
T4 touched

Entering in deep sleep

```
