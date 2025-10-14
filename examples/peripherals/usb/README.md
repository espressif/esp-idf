# USB-OTG Examples

See the [README.md](../../README.md) file in the upper level [examples](../../) directory for more information about examples.

## Common Pin Assignments

Pin assignment is only needed for ESP chips that have a USB-OTG peripheral.
If your board doesn't have a USB connector connected to the USB-OTG dedicated GPIOs, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

```
ESP BOARD       USB CONNECTOR (type A)
                   --
                  | || VBUS (5V)
[USB_DM]  ------> | || D-
[USB_DP]  ------> | || D+
                  | || GND
                   --
```

### USB PHY pin mapping

|              | USB_DP | USB_DM |
| ------------ | ------ | ------ |
| ESP32-S2/S3  | GPIO20 | GPIO19 |
| ESP32-P4 2.0 | pin 50 | pin 49 |
| ESP32-P4 1.1 | GPIO27 | GPIO26 |

> Note: On the ESP32-P4, the USB 2.0 PHY pins are dedicated to USB-OTG functionality and cannot be used as general-purpose GPIOs.
