| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

ESP-IDF UART HCI Controller
===========================

This example demonstrates how to run the Bluetooth controller with HCI over UART (H4), so the chip can be used as an external BLE/BTDM controller for a host on another device (PC or MCU).

HCI UART transport is initialized automatically in `esp_bt_controller_init()`. On ESP32 and ESP32-C3 / ESP32-S3, this is requested by the example through `CONFIG_BTDM_CTRL_HCI_UART_INIT_BY_CONTROLLER` and `CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER` in `sdkconfig.defaults.<target>`, since those targets also allow the application to own the HCI UART.

## Example Default Configuration

Per-target defaults are in `sdkconfig.defaults` and `sdkconfig.defaults.<target>`:

| Target group | HCI UART TX pin | HCI UART RX pin | HCI UART RTS / CTS | HCI UART baud rate | HW flow control |
|--------------|-----------------|-----------------|--------------------|--------------------|-----------------|
| ESP32 | 5 | 18 | 19 / 23 | 921600 | enabled |
| ESP32-C3 / ESP32-S3 | 4 | 5 | 6 / 7 | 115200 | enabled |
| ESP32-C2 / C5 / C6 / C61 / H2 / H21 | 8 | 9 | — | 115200 | disabled |
| ESP32-S31 / ESP32-H4 | 8 | 9 | — | 115200 | disabled |

UART0 is used for log output (IDF monitor). A second UART carries HCI traffic.

When using Bluetooth HCI UART with a third-party host stack, ensure Espressif Vendor-specific HCI commands are **not** enabled (disabled by default). See `components/bt/include/$IDF_TARGET/include/esp_bt_vs.h`.

## How to Use the Example

### Hardware Required

Connect the HCI UART to a USB-UART adapter or ESP-Test board on the host PC. Cross-connect data lines: device TX → host RX, device RX → host TX. On ESP32 / ESP32-C3 / ESP32-S3, also connect RTS/CTS when hardware flow control is enabled (default).

### Configure the Project

```
idf.py set-target <target>
idf.py menuconfig
```

### CTE Antenna Switching

This example can initialize antenna switching GPIOs in the same way as the NimBLE
CTE examples. On a target that supports CTE:

1. Enable **Enable Bluetooth LE Direction Finding (AoA/AoD)**
   (`CONFIG_BT_LE_CTE_FEATURE_ENABLED`) under the controller's **Enable BLE 5 feature** menu.
   On ESP32-H2, first disable **Enable BLE SM feature**
   (`CONFIG_BT_LE_SECURITY_ENABLE`), since CTE does not support encrypted communication.
2. Under **Example Configuration**, enable **Use GPIOs for CTE antenna switching**.
3. Set the number of GPIO bits (1–4) and their pin numbers to match your antenna
   switch wiring. Bit 0 is the least significant bit; N GPIO bits encode up to
   2^N antenna selections. The defaults are GPIO 0 and GPIO 1, not a board-specific
   pin assignment. Choose valid output GPIOs that do not overlap the HCI UART, log
   UART, flash, or other board peripherals.

The GPIOs are configured as outputs with pull-ups, then connected to the controller's
`CTE_ANTx` signals (`ANT_SELx` on targets using those signal names). Initialization
runs after controller initialization and before controller enable. If GPIO
configuration fails, the controller is deinitialized and startup stops.

GPIO routing is implemented in `main/ble_cte_antenna.c`. The
`ble_cte_antenna_init()` interface takes the board-specific GPIO array and bit count
from `main/main.c` and does not depend on the example's Kconfig options.

Enable this example option only when the local device needs antenna switching,
such as AoA reception or AoD transmission. AoA transmission does not require it.
The option is disabled by default and does not require the console REPL. It only
initializes GPIOs: the external host must configure CTE operations and the antenna
switching pattern through HCI commands.

### Build and Flash

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

**ESP32:**

```
I (442) BTDM_INIT: HCI UART1 Pin select: TX 5, RX 18, CTS 23, RTS 19 Baudrate:921600
I (442) BTDM_INIT: BT controller compile version [...]
I (452) system_api: read default base MAC address from EFUSE
```

**ESP32-C3 / ESP32-S3:**

```
I (336) BT_HCI_UART: HCI messages can be communicated over UART1:
--PINs: TxD 4, RxD 5, RTS 6, CTS 7
--Baudrate: 115200
I (336) BLE_INIT: BT controller compile version [...]
I (406) system_api: read default base MAC address from EFUSE
```

**ESP32-C2 / C5 / C6 / C61 / H2 / H21:**

```
I (...) hci_driver_uart: set uart pin tx:8, rx:9.
I (...) hci_driver_uart: set baud_rate:115200.
I (...) hci_driver_uart: set flow_ctrl:0.
I (346) system_api: read default base MAC address from EFUSE
```

**ESP32-S31 / H4:**

```
I (537) hci_uart_config: set uart pin tx:8, rx:9.
I (537) hci_uart_config: set rts:-1, cts:-1.
I (537) hci_uart_config: set baud_rate:115200.
I (547) hci_uart_config: set flow_ctrl:0.
I (547) uart: ALREADY NULL
I (547) uart: queue free spaces: 1
I (557) hci_uart: hci transport task create successfully, prio:23, stack size: 1024
I (557) BTDM_INIT: BTDM controller init OK
```

After startup, HCI messages can be exchanged over the configured HCI UART.

## Console Commands

The console REPL on the log UART is optional (`CONFIG_EXAMPLE_HCI_CONSOLE`, default off). The Bluetooth controller still runs without it. Extra command groups can be enabled separately.

To enable the REPL, run `idf.py menuconfig` and select **Example Configuration → Enable HCI console REPL**.

| Group | Commands | Enabled by |
|-------|----------|------------|
| Common (BLE and BR/EDR) | `reconfig_hci_uart_pin -t <tx> -r <rx>` | `CONFIG_EXAMPLE_HCI_CONSOLE` |
| BLE DTM | `set_ble_tx_power -i <index>`, `get_ble_tx_power`, `get_ble_rx_rssi` | `CONFIG_EXAMPLE_BLE_DTM_CONFIGURATION_COMMAND` |

The common group is available on every target whose HCI UART pins can be changed at runtime. The BLE DTM group targets direct test mode: on ESP32-C3/S3 enabling it also configures the HCI UART without hardware flow control, so only TX/RX have to be wired.

## Troubleshooting

- Match UART baud rate on the host to the controller configuration (921600 for ESP32, 115200 for other targets in this example).
- On ESP32 / ESP32-C3 / ESP32-S3, connect RTS/CTS when hardware flow control is enabled.
- Do not enable Espressif VS HCI commands when using a third-party host stack.
