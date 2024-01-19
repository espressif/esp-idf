| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# TinyUSB Serial Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP chip to work as a USB Serial Device.

As a USB stack, a TinyUSB component is used.

## How to use example

This example can also be configured to act as double serial device.
Run `idf.py menuconfig` and in `Component config → TinyUSB Stack → Communication Device Class (CDC) → CDC Channel Count` select number of serial devices you want to implement.

### Hardware Required

Any ESP board that have USB-OTG supported.

#### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see this output (for single CDC channel):

```
I (285) example: USB initialization
I (285) tusb_desc:
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
I (455) TinyUSB: TinyUSB Driver installed
I (465) example: USB initialization DONE
```

Connect to the serial port (e.g. on Linux, it should be `/dev/ttyACM0`) by any terminal application (e.g. `picocom /dev/ttyACM0`).
Now you can send data strings to the device, the device will echo back the same data string.

The monitor tool will also print the communication process:

```
I (12438) example: Data from channel 0:
I (12438) example: 0x3ffbfea0   45 73 70 72 65 73 73 69  66 0d                    |Espressif.|
```
