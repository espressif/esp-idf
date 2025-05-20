| Supported Targets | ESP32-C2 | ESP32-C5 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- | -------- |

ESP-IDF UART HCI Controller
===========================

This is a BLE controller use UART as HCI interface. 

It can do the configuration of UART number and UART baudrate by menuconfig.

When using Bluetooth HCI URAT, please make sure that the Espressif Vendor-specific HCI command is not enabled (Disabled by default).
For detailed description information, please refer to `components/bt/include/$IDF_TARGET/include/esp_bt_vs.h`.

## BLE HCI example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example start controller with uart hci.

It uses ESP32C2's Bluetooth controller.

In this example, two UARTs are used:

- UART0 is used as normal output or by IDF monitor

- UART1 is used to convey HCI messages

Pins 8, 9 are used as TxD, RxD PINs of UART1.


## How to use example

### Configure the project

```
idf.py menuconfig
```


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when bleprph is connected and characteristic is read:

```
set nimble port tx:8, rx:9.
set baud:115200.
controller lib commit: [fb738d4]
controller rom commit: [3314f9d]
I (346) system_api: Base MAC address is not set
I (356) system_api: read default base MAC address from EFUSE

```

