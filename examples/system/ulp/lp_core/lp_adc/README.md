| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# LP ADC Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the LP ADC peripheral on the LP core. The example sets up the LP ADC from the HP core before putting it to sleep.
It then reads the configured LP ADC channels from the LP core and prints the raw and converted voltage values to the LP UART console.

## How to use example

### Hardware Required

* A development board with ESP SoC such as the ESP32-P4 which supports LP ADC.
* A USB cable for power supply and programming
* A USB to UART converter to monitor the LP core serial console

In this example, you need to connect a voltage source (e.g. a DC power supply) to the GPIO pins for the input ADC channels. The ADC channels to read can be configured from the menuconfig option for this example.

**Note:** The following pin assignments are used by default.

#### LP UART Pin Assignment:

|                         | UART Tx Pin |
| ----------------------- | ------------|
| ESP32-P4                | GPIO14      |

The UART Tx Pin must be connected to the UART Rx pin on the host machine.

#### LP ADC Pin Assignments:

|                         | LP ADC Channel Number     | Pin    |
| ----------------------- | --------------------------| ------ |
| ESP32-P4                | Channel 4                 | GPIO20 |
| ESP32-P4                | Channel 5                 | GPIO21 |

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:
Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

Use another serial monitor program/instance such as idf.py monitor, minicom or miniterm to send and receive data from the LP core.
The default baudrate used for the example is 115200. Care must be taken that the configuration matches on both the device and the serial terminal.

## Example Output

Running this example, you will see the following log output on the LP core serial monitor:

```
ESP-ROM-LP:esp32p4-20230811
Build:Aug 11 2023
rst:0x14 (SW_RST), wakeup:0x40000
entry:0x50108180
lpadc1 chan0 raw value = 0
lpadc1 chan0 converted value = 0 mV
lpadc1 chan1 raw value = 3894
lpadc1 chan1 converted value = 4167 mV
```
