| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# TinyUSB Sample Descriptor

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP32-S2 chip to get log output via Serial Device connection

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

Note: if you want to send data to the target see how to implement it via `tud_cdc_rx_cb` at the `tusb_serial_device` example.

## Example Output

After the flashing you should see the output at idf monitor:

```
I (340) example: USB initialization
I (340) TinyUSB: Driver installation...
I (340) TinyUSB - Descriptors Control: Setting of a descriptor:
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

I (373) TinyUSB: Driver installed
I (373) example: USB initialization DONE
I (383) example: log -> UART
example: print -> stdout
example: print -> stderr
...

```

Other log will be printed to USB:
```
I (5382) example: log -> USB
example: print -> stdout
example: print -> stderr
...
```
