| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# TinyUSB Composite Device (MSC + Serial) Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

A USB device can provide multiple functions that are active simultaneously. Such multi-function devices are also known as composite devices. This example shows how to set up ESP chip to work as a USB Serial Device as well as MSC device (Storage media as SPI-Flash).

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

Any ESP board that supports the USB-OTG peripheral.

### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

Next, for Self-Powered Devices with VBUS monitoring, user must set ``self_powered`` to ``true`` and ``vbus_monitor_io`` to GPIO number (``VBUS_MONITORING_GPIO_NUM``) that will be used for VBUS monitoring.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output at idf monitor:

```
I (344) main_task: Calling app_main()
I (344) example_main: USB Composite initialization
W (354) TinyUSB: The device's configuration descriptor is not provided by user, using default.
W (364) TinyUSB: The device's string descriptor is not provided by user, using default.
W (374) TinyUSB: The device's device descriptor is not provided by user, using default.
I (384) tusb_desc:
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
I (544) TinyUSB: TinyUSB Driver installed
I (554) example_main: Initializing storage...
I (554) example_main: Initializing wear levelling
I (584) example_main: USB Composite initialization DONE
I (584) main_task: Returned from app_main()
```
