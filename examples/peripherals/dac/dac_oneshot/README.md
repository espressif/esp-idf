| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Oneshot Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows the oneshot usage of outputting a voltage directly by the DAC driver.

Both DAC channels ramp the digital code by one eighth of the full-scale range every 500 ms, and wrap back to 0 periodically. Channel 1 starts 500 ms later than channel 0 so the two outputs are staggered.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
* DAC channel to GPIO mapping: see [GPIO Summary](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#gpio-summary) (switch the chip target on the documentation page if needed)
* Note that some ESP32-S2 DevKits have an LED on the DAC channel 1 pin, so that channel's output may not go fully low
* (Optional) An oscilloscope to monitor the output voltage

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example prints the digital code written by each channel. On ESP32 / ESP32-S2 the step is 32:

```
DAC oneshot example started
dac_chan0 = 0
dac_chan0 = 32
dac_chan1 = 0
dac_chan0 = 64
dac_chan1 = 32
...
```

If monitoring the DAC channels with an oscilloscope, there will be two stepped voltages with a one-period (500 ms) time offset.
