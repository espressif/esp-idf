| Supported Targets | ESP32 |
| ----------------- | ----- |

ESP-IDF UART HCI Controller
===========================

This is a btdm controller use UART as HCI IO. This require the UART device support RTS/CTS mandatory.

It can do the configuration of UART number and UART baudrate by menuconfig.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board. To connect UART to PC, another board such as ESP_Test Board or FT232 USB UART board is usually needed.

In this example, three UARTs are used:

- UART0 is used as normal output or by IDF monitor

- UART1 or UART2 is used to convey HCI messages

RTS and CTS lines of UART1 or UART2 are required. Pins 5, 18, 23, 19 are used as TxD, RxD, CTS, RTS PINs of UART1 or UART2, respectively.

In a frequently-used scenario, if ESP_Test Board is used, connect the TX0, RX0, RTS0, CTS0 and GND of ESP_Test Board to ESP32 UART1 or UART2 PINs, and Attach ESP_Test board to the host PC.

### Configure the project

```
idf.py menuconfig
```

- UART number can be configured in `HCI UART(H4) Options > UART Number for HCI`

- UART baudrate can be configured in `HCI UART(H4) Options > UART Baudrate for HCI`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example resets the HCI UART transport and enable Bluetooth Controller.

```
I (442) CONTROLLER_UART_HCI: HCI UART1 Pin select: TX 5, RX 18, CTS 23, RTS 19
I (442) BTDM_INIT: BT controller compile version [078d492]
I (452) system_api: Base MAC address is not set
I (452) system_api: read default base MAC address from EFUSE
I (462) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
```

After these output occurs, HCI messages can be commnunicated over UART1 or UART2. (UART1 in this example)

## Troubleshooting
