| Supported Targets | ESP32 | ESP32-P4 | ESP32-S31 |
| ----------------- | ----- | -------- | --------- |

# Basic Ethernet Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of internal EMAC and `Ethernet driver` together with `esp_netif`. The workflow of the example is as follows:

1. Install Ethernet driver
2. Attach the driver to `esp_netif`
3. Send DHCP requests and wait for a DHCP lease
4. If get IP address successfully, then you will be able to ping the device

If you have a new simple Ethernet application to go (for example, connect to IoT cloud via Ethernet), try this as a basic template, then add your own code.

>[!TIP]
> For initialization of a wider range of Ethernet PHY chips, including SPI Ethernet modules and for advanced Ethernet configuration options, please use [Component for Ethernet Initialization](https://components.espressif.com/components/espressif/ethernet_init/).

## How to use example

### Hardware Required

To run this example, it's recommended that you have an official Espressif Ethernet capable development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-ethernet-kit/index.html), [ESP32-P4-Function-EV-Board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/index.html) or ESP32-S31 Functional-Core Board. This example should also work for 3rd party ESP32 board as long as it's integrated with a IEEE 802.3 compliant Ethernet PHY chip and with the default dataplane GPIO configuration.

> [!NOTE]
> `Generic 802.3 PHY` basic functionality should always work for PHY compliant with IEEE 802.3. However, some specific features might be limited. A typical example is loopback functionality, where certain PHYs may require setting a specific speed mode to operate correctly. If this is a case, use driver tailored to that specific chip.

#### Pin Assignment

This example uses the default GPIO configuration as defined by `ETH_ESP32_EMAC_DEFAULT_CONFIG` for the specific ESP32 chip (ESP32, ESP32P4, ESP32S31, etc.).

#### ESP32-S31 with YT8531 PHY (RGMII)

The ESP32-S31 Functional-Core Board uses a YT8531 PHY connected via RGMII. While the example uses the Generic 802.3 PHY driver for standard PHY operations, the YT8531 requires additional chip-specific initialization to work correctly in RGMII mode. This is handled automatically when the `CONFIG_EXAMPLE_ETH_PHY_YT8531_INIT` option is enabled (default for ESP32-S31 targets):

- **Auto-negotiation re-enable**: The YT8531 disables auto-negotiation upon hardware reset (undocumented behavior), so it must be explicitly re-enabled after the Generic 802.3 driver finishes initialization.
- **RGMII clock delay tuning**: Tx and Rx internal clock delays (~2 ns each) are configured via the PHY's extended register interface to ensure reliable RGMII data sampling.

These steps are performed via the standard `esp_eth_ioctl()` API without requiring a dedicated YT8531 driver. The implementation can also serve as a template for configuring other PHYs that need post-init tuning when used with the Generic 802.3 driver.

### Configure the project

```
idf.py menuconfig
```

See common configurations for Ethernet examples from [upper level](../README.md#common-configurations).

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (394) eth_example: Ethernet Started
I (3934) eth_example: Ethernet Link Up
I (3934) eth_example: Ethernet HW Addr 30:ae:a4:c6:87:5b
I (5864) esp_netif_handlers: eth ip: 192.168.2.151, mask: 255.255.255.0, gw: 192.168.2.2
I (5864) eth_example: Ethernet Got IP Address
I (5864) eth_example: ~~~~~~~~~~~
I (5864) eth_example: ETHIP:192.168.2.151
I (5874) eth_example: ETHMASK:255.255.255.0
I (5874) eth_example: ETHGW:192.168.2.2
I (5884) eth_example: ~~~~~~~~~~~
```

Now you can ping your ESP32 in the terminal by entering `ping 192.168.2.151` (it depends on the actual IP address you get).

## Troubleshooting

See common troubleshooting for Ethernet examples from [upper level](../README.md#common-troubleshooting).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
