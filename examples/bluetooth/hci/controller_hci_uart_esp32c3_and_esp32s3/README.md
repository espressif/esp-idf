| Supported Targets | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- |

ESP-IDF UART HCI Controller
=================================

This example demonstrates how to configure the Bluetooth Low Energy Controller's HCI (Host Controller Interface) to communicate over UART.

Using this example, BLE radio capabilities of ESP32-C3/ESP32-S3 chip, can be:

1. tested via standard HCI messages in Direct Test Mode

2. used with external Bluetooth host stack installed on PC, or other MCU.

This example uses UHCI, GDMA together with UART to implement the HCI UART transport.

This example uses LL/register access directly, because the UHCI driver hasn't been implemented yet.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32-C3/ESP32-S3 development board. To connect UART to PC, another board such as ESP_Test Board or FT232 USB UART board is usually needed.

In this example, two UARTs are used:

- UART0 is used as normal output or by IDF monitor

- UART1 is used to convey HCI messages


RTS and CTS lines of UART1 are required. GPIO4, GPIO5, GPIO6, GPIO7 are used as TxD, RxD, RTS, CTS PINs of UART1, respectively.

In a frequently-used scenario, if ESP_Test Board is used, connect the TX0, RX0, RTS0, CTS0 and GND of ESP_Test Board to ESP32-C3/ESP32-S3 UART1 PINs, and Attach ESP_Test board to the host PC.

### Configure the project

```
idf.py menuconfig
```

* Baudrate of UART1 can be configured in `Example Configuration > UART Baudrate for HCI`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example sets up the HCI UART transport and enable Bluetooth Controller, after started. UART1 PIN and baudrate settings is printed at serial output:

```
I (306) gpio: GPIO[4]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (306) gpio: GPIO[6]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (316) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (326) gpio: GPIO[7]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (336) BTDM_INIT: BT controller compile version [33175c8]
I (346) phy_init: phy_version 907,3369105-dirty,Dec  3 2021,14:55:12
I (406) system_api: Base MAC address is not set
I (406) system_api: read default base MAC address from EFUSE
I (406) BTDM_INIT: Bluetooth MAC: 7c:df:a1:61:e5:36

I (406) UHCI: HCI messages can be communicated over UART1:
--PINs: TxD 4, RxD 5, RTS 6, CTS 7
--Baudrate: 115200
```

After these output occurs, HCI messages can be commnunicated over UART1.

## Troubleshooting

## Example Breakdown

