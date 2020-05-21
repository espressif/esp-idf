# SLIP device client

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This provides SLIP support for connection to Contiki gateway devices, allowing the ESP32 to be used to bridge between low-power networks and IP (Wifi / Ethernet).

## How to use example

### Hardware Required

To run this example, you need an ESP32 dev board (e.g. ESP32-WROVER Kit) or ESP32 core board (e.g. ESP32-DevKitC).
For test purpose, you also need a SLIP capable gateway device, such as anything running [Contiki](https://github.com/contiki-os/contiki) gateway firmware.
You can also try other modules as long as they implement the SLIP protocol.

#### Pin Assignment

**Note:** The following pin assignments are used by default which can be changed in menuconfig.

| ESP32  | Gateway |
| ------ | -------------- |
| GPIO4  | RX             |
| GPIO36 | TX             |
| GND    | GND            |
| 3v3    | VCC            |

### Configure the project

Open the project configuration menu (`idf.py menuconfig`). Then go into `Example Configuration` menu.

- Choose the RX and TX pins

For use in external projects `SLIP support` must be enabled under the `components/lwip` menu.


### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.



## Troubleshooting

1. Invalid slip packets

Many slip devices use additional messages for things like ipv6 prefix configuration (or sending log messages over the SLIP serial port). This is supported in the driver through the use of an `rx_filter` function that is called on receipt of all packets and can be used to filter packets prior to passing them to the stack.

2. No packets received

The first layer to check is the serial port, you can enable debugging of the SLIP component by setting the global log level to `DEBUG`, or changing the slip component log levbel with `esp_log_level_set("esp-netif_lwip-slip", ESP_LOG_DEBUG);`

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
