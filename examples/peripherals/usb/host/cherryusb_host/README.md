| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# CherryUSB Host Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the CherryUSB host driver. Currently, this example supports communication with HID devices (such as Keyboard and Mouse), serial port devices (such as CDC_ACM, CH34x, CP210x, PL2303, FTDI FT23x/FT423x devices) and MSC (Mass Storage Class).

## How to use example

By default, all drivers supported by the routine are enabled. If you need to trim or disable unnecessary drivers, please disable the corresponding driver configuration.
Run `idf.py menuconfig` and in `Component config → CherryUSB Configuration → Enable usb host mode`, Uncheck the drivers you don’t need.

### Hardware Required
* Development board with USB-OTG support
* A USB cable for Power supply and programming
* USB OTG Cable

#### Pin Assignment

Follow instruction in [examples/usb/README.md](../../../README.md) for specific hardware setup.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

When running, the example will print the following output:

```
I (264) main_task: Started on CPU0
I (304) main_task: Calling app_main()
[I/USB] cherryusb, version: v1.5.0
[I/USB] ========== dwc2 hcd params ==========
[I/USB] CID:00000000
[I/USB] GSNPSID:4f54400a
[I/USB] GHWCFG1:00000000
[I/USB] GHWCFG2:215fffd0
[I/USB] GHWCFG3:03805eb5
[I/USB] GHWCFG4:dff1a030
[I/USB] dwc2 has 16 channels and dfifo depth(32-bit words) is 896
I (334) HOST: Init usb
I (334) main_task: Returned from app_main()
[I/usbh_hub] New low-speed device on Bus 0, Hub 1, Port 1 connected
[I/usbh_core] New device found,idVendor:413c,idProduct:2113,bcdDevice:0110
[I/usbh_core] The device has 1 bNumConfigurations
[I/usbh_core] The device has 2 interfaces
[I/usbh_core] Enumeration success, start loading class driver
[I/usbh_core] Loading hid class driver
[I/usbh_hid] Ep=81 Attr=03 Mps=8 Interval=10 Mult=00
[I/usbh_hid] Register HID Class:/dev/input0
I (1054) HID: intf 0, SubClass 1, Protocol 1
[I/usbh_core] Loading hid class driver
[W/usbh_hid] Do not support set idle
[I/usbh_hid] Ep=82 Attr=03 Mps=3 Interval=10 Mult=00
[I/usbh_hid] Register HID Class:/dev/input1
I (1074) HID: intf 1, SubClass 0, Protocol 0

Keyboard
xiaodou
```
