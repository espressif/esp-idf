| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# WiFi station to "Wired" interface L2 forwarder

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example aims to demonstrate 1-1 bridge using WiFi station and one of these interfaces (so called *wired* in this example)
- Ethernet (supported for all targets)
- USB acting as NCM device (supported for ESP32-S2 and ESP32-S3)

It also allows for reconfiguring WiFi settings using a virtual network in the Ethernet. The reconfiguration mode is initialized if the WiFi settings are not available, connection fails or manually by long pressing the Boot button (GPIO0).
It is possible to configure WiFi settings (SSID and password) in a browser on a hostname `"http://wifi.settings"` or using unified provisioning.

## How to use example

This example could be used to *bring* wireless connectivity to devices that support only Ethernet (or USB Ethernet implemented as NCM device).
This example also supports runtime configuration of WiFi settings by means of a webpage or unified provisioning.


### Hardware Required

Any board with either Ethernet of USB-OTG supported.

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu choose the provisioning method:
* `EXAMPLE_WIFI_CONFIGURATION_MANUAL` for manual configuration using a webpage
* `EXAMPLE_WIFI_CONFIGURATION_PROVISIONING` for standard provisioning over the virtual USB network 

To provision the device using IDF provisioning tools (if `EXAMPLE_WIFI_CONFIGURATION_PROVISIONING` is selected) you can use idf provisioning utility with transport set to `softap`:
```bash
esp-idf/tools/esp_prov$ python esp_prov.py --transport httpd ...
```
Please refer to the provisioning documentation and `esp_prov` script [documentation](../../../tools/esp_prov/README.md) for more details.

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

(note that this is the output of USB configuration)
```
I (1740) example_sta2wired: Wi-Fi STA connected
I (1740) example_sta2wired: WiFi station connected successfully
W (1750) TinyUSB: The device's configuration descriptor is not provided by user, using default.
W (1760) TinyUSB: The device's string descriptor is not provided by user, using default.
W (1770) TinyUSB: The device's device descriptor is not provided by user, using default.
I (1770) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (1780) tusb_desc:
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
```
