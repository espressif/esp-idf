| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# TinyUSB MIDI Device Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP chip to work as a USB MIDI Device.
It outputs a MIDI note sequence via the native USB port.

As a USB stack, a TinyUSB component is used.

## How to use example

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

## MIDI output

You can use several programs on your computer to listen to the ESP's MIDI output depending on your operating system, e.g.:

* Windows: `MIDI-OX`
* Linux: `qsynth` with `qjackctl`
* macOS: `SimpleSynth`

## Example Output

After the flashing you should see the output at idf monitor:

```
I (285) example: USB initialization
I (285) tusb_desc:
┌─────────────────────────────────┐
│  USB Device Descriptor Summary  │
├───────────────────┬─────────────┤
│bDeviceClass       │ 0           │
├───────────────────┼─────────────┤
│bDeviceSubClass    │ 0           │
├───────────────────┼─────────────┤
│bDeviceProtocol    │ 0           │
├───────────────────┼─────────────┤
│bMaxPacketSize0    │ 64          │
├───────────────────┼─────────────┤
│idVendor           │ 0x303a      │
├───────────────────┼─────────────┤
│idProduct          │ 0x4008      │
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

Disconnect UART-to-USB port and connect the native USB port to a computer then the device should show up as a USB MIDI Device while outputting notes.
