| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# TinyUSB Sample Descriptor

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is demonstrating how to set up ESP chip to work as a Generic USB Device with a user-defined descriptor. You can specify a manufacturer, device's name, ID and other USB-devices parameters responsible for identification by host.

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

Any ESP boards that have USB-OTG supported.

#### Pin Assignment

See common pin assignments for USB Device examples from [upper level](../README.md#common-pin-assignments).

### Configure the project

There are two ways to set up a descriptor - using Menuconfig tool and in-code

#### In-code setting up

For the manual descriptor configuration use the default example's settings and modify `my_descriptor` in [source code](main/tusb_sample_descriptor_main.c) according to your needs

#### Menuconfig

If you want to set up the descriptor using Menuconfig UI:

1. Execute in the terminal from the example's directory: `idf.py menuconfig`

2. Turn off `Set up a USB descriptor manually in code` parameter at `Example Configuration`

3. Follow `Component config -> TinyUSB -> Descriptor configuration` for all available configurations.

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
I (287) example: USB initialization
I (287) tusb_desc:
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
│idProduct          │ 0x3000      │
├───────────────────┼─────────────┤
│bcdDevice          │ 0x101       │
├───────────────────┼─────────────┤
│iManufacturer      │ 0x1         │
├───────────────────┼─────────────┤
│iProduct           │ 0x2         │
├───────────────────┼─────────────┤
│iSerialNumber      │ 0x3         │
├───────────────────┼─────────────┤
│bNumConfigurations │ 0x1         │
└───────────────────┴─────────────┘
I (457) TinyUSB: TinyUSB Driver installed
I (467) example: USB initialization DONE
```

From PC, running `lsusb -v`, you should find the device's descriptor like:
```
Bus 001 Device 007: ID 303a:3000 I My Custom Device
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0
  bDeviceSubClass         0
  bDeviceProtocol         0
  bMaxPacketSize0        64
  idVendor           0x303a
  idProduct          0x3000
  bcdDevice            1.01
  iManufacturer           1 I
  iProduct                2 My Custom Device
  iSerial                 3 012-345
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x0009
    bNumInterfaces          0
    bConfigurationValue     1
    iConfiguration          0
    bmAttributes         0xa0
      (Bus Powered)
      Remote Wakeup
    MaxPower              100mA
can't get device qualifier: Resource temporarily unavailable
can't get debug descriptor: Resource temporarily unavailable
Device Status:     0x0000
  (Bus Powered)
```
