# USB-OTG Examples

See the [README.md](../../README.md) file in the upper level [examples](../../) directory for more information about examples.

## Common Pin Assignments

Pin assignment is only needed for ESP chips that have a USB-OTG peripheral.
If your board doesn't have a USB connector connected to the USB-OTG dedicated GPIOs, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

```
ESP BOARD          USB CONNECTOR (type A)
                          --
                         | || VCC
[USBPHY_DM_NUM]  ------> | || D-
[USBPHY_DP_NUM]  ------> | || D+
                         | || GND
                          --
```
Refer to `soc/usb_pins.h` to find the real GPIO number of **USBPHY_DP_NUM** and **USBPHY_DM_NUM**.

|             | USB_DP | USB_DM |
| ----------- | ------ | ------ |
| ESP32-S2/S3 | GPIO20 | GPIO19 |
| ESP32-P4    | pin 51 | pin 50 |

> Note: On the ESP32-P4, the USB 2.0 PHY pins are dedicated to USB-OTG functionality and cannot be used as general-purpose GPIOs.
