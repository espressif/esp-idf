| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# TinyUSB Sample Descriptor

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP chip to get log output via Serial Device connection.

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

Any ESP boards that have USB-OTG supported.

#### Pin Assignment

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

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
I (288) example: USB initialization
I (288) tusb_desc:
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
I (458) TinyUSB: TinyUSB Driver installed
I (468) example: USB initialization DONE
I (468) example: log -> UART
example: print -> stdout
example: print -> stderr
...

```

Other log will be printed to USB:
```
I (3478) example: log -> USB
example: print -> stdout
example: print -> stderr
```
