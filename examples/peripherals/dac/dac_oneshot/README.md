| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Oneshot Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows the oneshot usage of outputting a voltage directly by the DAC driver.

The output voltage will increase a step every 500 ms, and it will reset to 0 periodically.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
    - Note that some ESP32-S2 DevKits have LED on it which is connected to GPIO18 (same pin as DAC channel1), so the output voltage of DAC channel 1 can't go down due the this LED.
* (Optional) An oscilloscope to monitor the output wave

### Build and Flash

Note that as we use the ADC to monitor the output data, we need to set false to `CONFIG_ADC_DISABLE_DAC_OUTPUT` in the menuconfig, otherwise the ADC will shutdown the DAC power to guarantee it won't be affect by DAC.

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
DAC channel 0 value:   37       DAC channel 1 value:    0
DAC channel 0 value:   37       DAC channel 1 value:    0
DAC channel 0 value:   38       DAC channel 1 value:    0
DAC channel 0 value:   38       DAC channel 1 value:    0
DAC channel 0 value:   34       DAC channel 1 value:    0
DAC channel 0 value:  179       DAC channel 1 value:  117
DAC channel 0 value:  176       DAC channel 1 value:  117
DAC channel 0 value:  178       DAC channel 1 value:  122
DAC channel 0 value:  179       DAC channel 1 value:  118
DAC channel 0 value:  177       DAC channel 1 value:  115
DAC channel 0 value:  316       DAC channel 1 value:  261
DAC channel 0 value:  317       DAC channel 1 value:  263
DAC channel 0 value:  311       DAC channel 1 value:  261
DAC channel 0 value:  317       DAC channel 1 value:  260
DAC channel 0 value:  317       DAC channel 1 value:  262
DAC channel 0 value:  458       DAC channel 1 value:  406
DAC channel 0 value:  456       DAC channel 1 value:  406
DAC channel 0 value:  454       DAC channel 1 value:  403
DAC channel 0 value:  457       DAC channel 1 value:  406
DAC channel 0 value:  459       DAC channel 1 value:  407
...
```

If monitoring the DAC channels with an oscilloscope, there will be a direct voltage on the screen and it will be updated every 500 ms.
