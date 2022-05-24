| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Basic Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows the basic usage of outputting a voltage directly by the DAC driver.

The output voltage will increase a step every 500 ms, and it will reset to 0 periodically.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
    - Note that some ESP32-S2 DevKits have LED on it which is connected to GPIO18 (same pin as DAC channel2), so the output voltage of DAC channel 1 can't go down due the this LED.
* (Optional) An oscilloscope to monitor the output wave

### Configure the Project

There is a macro `EXAMPLE_DAC_USE_SEPARATE_CHANNEL` in the example to choose whether put the two DAC channels into a same group

    - If `EXAMPLE_DAC_USE_SEPARATE_CHANNEL` is 1, DAC channel 0 and channel 2 can be set to different voltage separately by their own group handle.
    - If `EXAMPLE_DAC_USE_SEPARATE_CHANNEL` is 0, DAC channel 0 and channel 2 will be set to a same voltage by the same group handle.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The DAC channels can be read by ADC channels internally. The ADC read period is 100 ms, the following log is the raw ADC value read from the DAC channels, it shows the output voltage is increasing every 500 ms.

```
DAC channel 0 vaule:   37       DAC channel 1 vaule:    0
DAC channel 0 vaule:   37       DAC channel 1 vaule:    0
DAC channel 0 vaule:   38       DAC channel 1 vaule:    0
DAC channel 0 vaule:   38       DAC channel 1 vaule:    0
DAC channel 0 vaule:   34       DAC channel 1 vaule:    0
DAC channel 0 vaule:  179       DAC channel 1 vaule:  117
DAC channel 0 vaule:  176       DAC channel 1 vaule:  117
DAC channel 0 vaule:  178       DAC channel 1 vaule:  122
DAC channel 0 vaule:  179       DAC channel 1 vaule:  118
DAC channel 0 vaule:  177       DAC channel 1 vaule:  115
DAC channel 0 vaule:  316       DAC channel 1 vaule:  261
DAC channel 0 vaule:  317       DAC channel 1 vaule:  263
DAC channel 0 vaule:  311       DAC channel 1 vaule:  261
DAC channel 0 vaule:  317       DAC channel 1 vaule:  260
DAC channel 0 vaule:  317       DAC channel 1 vaule:  262
DAC channel 0 vaule:  458       DAC channel 1 vaule:  406
DAC channel 0 vaule:  456       DAC channel 1 vaule:  406
DAC channel 0 vaule:  454       DAC channel 1 vaule:  403
DAC channel 0 vaule:  457       DAC channel 1 vaule:  406
DAC channel 0 vaule:  459       DAC channel 1 vaule:  407
...
```

If monitoring the DAC channels with an oscilloscope, there will be a direct voltage on the screen and it will be updated every 500 ms.
