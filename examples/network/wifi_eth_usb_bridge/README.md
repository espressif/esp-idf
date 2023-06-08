| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# TinyUSB Network Control Model Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Network Control Model (NCM) is a sub-class of Communication Device Class (CDC) USB Device for Ethernet-over-USB applications.

In this example, we implemented the ESP development board to transmit WiFi data to the Linux host via USB, so that the Linux host could access the Internet.

As a USB stack, a TinyUSB component is used.

## How to use example

This example demonstrate usage of USB NCM device as USB-WiFi bridge. It also allows for reconfiguring WiFi settings using a virtual network in NCM device. The reconfiguration mode is initialized if the WiFi settings are not available, connection fails or manually by long pressing the Boot button (GPIO0).
It is possible to configure WiFi settings (SSID and password) in a browser on an address `"wifi.settings"` or using unified provisioning.

### Hardware Required

Any ESP board that have USB-OTG supported.

#### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu choose the provisioning method:
* `EXAMPLE_WIFI_CONFIGURATION_MANUAL` for manual configuration using a webpage
* `EXAMPLE_WIFI_CONFIGURATION_PROVISIONING` for standard provisioning over the virtual USB network 

To provision the device using IDF provisioning tools (if `EXAMPLE_WIFI_CONFIGURATION_PROVISIONING` is selected) you can use idf provisioning utility with transport set to `softap`:
```bash
esp-idf/tools/esp_prov$ python esp_prov.py --transport softap ...
```
Please refer to the provisioning documentation and `esp_prov` script [documentation](../../../../../tools/esp_prov/README.md) for more details.

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
I (725) usb_net: Wi-Fi STA connected
I (735) usb_net: CONNECTED_BIT

I (735) usb_net: connect success

I (735) wifi:BcnInt:102400, DTIM:1
I (745) usb_net: USB net initialization
I (745) tusb_desc: 
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
│idProduct          │ 0x4002      │
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
I (915) TinyUSB: TinyUSB Driver installed
I (925) usb_net: USB NCM initialization DONE
```
