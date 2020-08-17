| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# TinyUSB Sample CDC

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is demonstrating how to set up ESP32-S2 chip to work as a CDC USB Device. You can specify a manufacturer, device's name, ID and other USB-devices parameters responsible for identification by host.


As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

- Any board with the ESP32-S2 chip

#### Menuconfig

If you want to set up the desctiptor using Menuconfig UI:

1. Execute in the terminal from the example's directory: `idf.py menuconfig`

2. Folow to `Component config -> TinyUSB -> Descriptor configuration` for all available configurations.

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
I (328) example: USB initialization
I (327) TUSB:descriptors_control: Setting of a descriptor: 
.bDeviceClass       = 239
.bDeviceSubClass    = 2,
.bDeviceProtocol    = 1,
.bMaxPacketSize0    = 64,
.idVendor           = 0x0000303a,
.idProduct          = 0x00003000,
.bcdDevice          = 0x00000100,
.iManufacturer      = 0x01,
.iProduct           = 0x02,
.iSerialNumber      = 0x03,
.bNumConfigurations = 0x01

I (357) example: USB initialization DONE
I (367) example: USB task started
```

Now open new terminal (ie putty). This will let to communicate with esp32 S2 over native USB.
