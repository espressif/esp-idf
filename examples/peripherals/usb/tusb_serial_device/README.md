| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# TinyUSB Sample Descriptor

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP32-S2 chip to work as a USB Serial Device.

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

- Any board with the ESP32-S2 chip with USB connectors or with exposed USB's D+ and D- (DATA+/DATA-) pins.

If the board has no USB connector, but has the pins connect pins directly to the host (e.g. with DIY cable from any USB connection cable)

```
ESP32-S2 BOARD          USB CONNECTOR (type A)
                          --
                         | || VCC
    [GPIO 19]  --------> | || D-
    [GPIO 20]  --------> | || D+
                         | || GND
                          --
```

You can also use power from the USB connector.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Serial Connection

After program's start and getting of the message of readiness (`Serial device is ready to connect`) you can connect to the board using any serial port terminal application (e.g. CoolTerm).

## Example Output

After the flashing you should see the output:

```
I (346) example: USB initialization
I (346) TinyUSB: Driver installation...
I (346) TinyUSB - Descriptors Control: Setting of a descriptor:
.bDeviceClass       = 239
.bDeviceSubClass    = 2,
.bDeviceProtocol    = 1,
.bMaxPacketSize0    = 64,
.idVendor           = 0x0000303a,
.idProduct          = 0x00004001,
.bcdDevice          = 0x00000100,
.iManufacturer      = 0x01,
.iProduct           = 0x02,
.iSerialNumber      = 0x03,
.bNumConfigurations = 0x01

I (362) TinyUSB: Driver installed
I (362) example: USB initialization DONE
I (922) example: Line state changed! dtr:0, rst:0
```

Let's try to send a string "espressif" and get the return string in your console on PC:

```
I (18346) example: Got data (9 bytes): espressif
```
