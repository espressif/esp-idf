| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# TinyUSB Sample Descriptor

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP chip to work as a USB Serial Device.

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

Any ESP boards that have USB-OTG supported.

#### Pin Assignment

See common pin assignments for USB Device examples from [upper level](../README.md#common-pin-assignments).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output:

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

Connect to the serial port (e.g. on Linux, it should be `/dev/ttyACM0`) by any terminal application (e.g. `picocom /dev/ttyACM0`), typing a string "espressif" and you will get the exactly same string returned.

The monitor tool will also print the communication process:

```
I (146186) example: Line state changed! dtr:1, rst:1
I (147936) example: Got data (1 bytes): e
I (148136) example: Got data (1 bytes): s
I (148336) example: Got data (1 bytes): p
I (148416) example: Got data (1 bytes): r
I (148446) example: Got data (1 bytes): e
I (148676) example: Got data (1 bytes): s
I (148836) example: Got data (1 bytes): s
I (148956) example: Got data (1 bytes): i
I (149066) example: Got data (1 bytes): f
```
