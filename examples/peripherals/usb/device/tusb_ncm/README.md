| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# TinyUSB Network Control Model Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Network Control Model (NCM) is a sub-class of Communication Device Class (CDC) USB Device for Ethernet-over-USB applications.

In this example, we implemented the ESP development board to transmit WiFi data to the Linux or Windows host via USB, so that the host could access the Internet.

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

Any ESP board that have USB-OTG supported.

#### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the Wi-Fi configuration.
    * Set `WiFi SSID`.
    * Set `WiFi Password`.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output at idf monitor:

```
I (399) main_task: Calling app_main()
I (429) USB_NCM: USB NCM device initialization
W (429) TinyUSB: The device's configuration descriptor is not provided by user, using default.
W (429) TinyUSB: The device's string descriptor is not provided by user, using default.
W (439) TinyUSB: The device's device descriptor is not provided by user, using default.
I (449) tusb_desc:
┌─────────────────────────────────┐
│  USB Device Descriptor Summary  │
├───────────────────┬─────────────┤
│bDeviceClass       │ 239         │
├───────────────────┼─────────────┤
│bDeviceSubClass    │ 2           │
├───────────────────┼─────────────┤
│bDeviceProtocol    │ 1           │
├───────────────────┼─────────────┤
│bMaxPacketSize0    │ 64          │
├───────────────────┼─────────────┤
│idVendor           │ 0x303a      │
├───────────────────┼─────────────┤
│idProduct          │ 0x4001      │
├───────────────────┼─────────────┤
│bcdDevice          │ 0x100       │
├───────────────────┼─────────────┤
│iManufacturer      │ 0x1         │
├───────────────────┼─────────────┤
│iProduct           │ 0x2         │
├───────────────────┼─────────────┤
│iSerialNumber      │ 0x3         │
├───────────────────┼─────────────┤
│bNumConfigurations │ 0x1         │
└───────────────────┴─────────────┘
I (619) TinyUSB: TinyUSB Driver installed
I (619) USB_NCM: WiFi initialization
I (619) pp: pp rom version: e7ae62f
I (629) net80211: net80211 rom version: e7ae62f
I (689) wifi_init: rx ba win: 6
I (699) wifi_init: tcpip mbox: 32
I (699) wifi_init: udp mbox: 6
I (699) wifi_init: tcp mbox: 6
I (709) wifi_init: tcp tx win: 5744
I (709) wifi_init: tcp rx win: 5744
I (719) wifi_init: tcp mss: 1440
I (719) wifi_init: WiFi IRAM OP enabled
I (719) wifi_init: WiFi RX IRAM OP enabled
I (729) phy_init: phy_version 600,8dd0147,Mar 31 2023,16:34:12
I (779) USB_NCM: USB NCM and WiFi initialized and started
I (779) main_task: Returned from app_main()
I (849) USB_NCM: WiFi STA connected
```
