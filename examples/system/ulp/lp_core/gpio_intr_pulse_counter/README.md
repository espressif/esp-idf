| Supported Targets | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- |

# LP Core Pulse Counting Example

This example demonstrates how to program the ULP Core coprocessor to count pulses on an IO while the main CPUs are either running some other code or are in deep sleep. See the README.md file in the upper level 'examples' directory for more information about examples.

At runtime, the main code running on the ESP (found in lp_core_pulse_counter_example_main.c) loads ULP program into the `RTC_SLOW_MEM` memory region using `ulp_lp_core_load_binary` function. Main code configures the ULP program by setting up values of some variables and then starts it using `ulp_lp_core_run`. Once the ULP program is started, it monitors the IO pin for pulses.

When the ULP program finds an edge in the input signal, it performs debouncing and increments the variable maintaining the total edge count. Once the edge count reaches certain value, ULP triggers wake up from deep sleep. Note that the ULP program keeps running and monitoring the input signal even when the SoC is woken up.

## How to use example

### Hardware Required

To run this example, you should have a development board based on any of the chips listed in the supported targets table at the top and a host machine with a serial input connection.

#### Pin Assignment:

**Note:** The following pin assignments are used by default.


|                         | Uart Tx | Pulse Count Input |
| ----------------------- | ------- | ----------------- |
| ESP32-C6                | GPIO5   | GPIO6             |
| ESP32-P4                | GPIO14  | GPIO6             |
| Host machine            | Rx      | N/A               |

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

Use another serial monitor program/instance such as idf.py monitor, minicom or miniterm to send and receive data from the LP core.
The default baudrate used for the example is 115200. Care must be taken that the configuration matches on both the device and the serial terminal.

## Example Output

The log output from the serial monitor connected to the main core should indicate that the LP core and the LP UART peripheral have been successfully initialized. The main CPU would then enter deep sleep mode.

```bash
Using pin 6 as pulse counter input
ULP will wake up processor after every 10 pulses
Not a ULP wakeup, initializing it!
Entering in deep sleep
...
rst:0x5 (SLEEP_WAKEUP),boot:0xc (SPI_FAST_FLASH_BOOT)
...
ULP woke up the main CPU!
Pulse count: 11
Entering in deep sleep
```

The log output from the serial monitor connected to the LP core should display output as below -

```bash
LP Core pulse counter started
Pulse count: 10, wake-up main CPU
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
