| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PPA RGB LCD Example

## Overview

This example demonstrates how to use `esp_driver_ppa` to process an image and display the results on a generic RGB LCD panel.

The image is scaled, rotated, mirrored, blended, and filled by PPA, then pushed to the RGB panel with `esp_lcd_panel_draw_bitmap()`.

## Hardware Required

* An ESP development board, which supports the RGB LCD peripheral
* A general RGB panel, supports 16/24 bit-width, with HSYNC, VSYNC and DE signal
* An USB cable for power supply and programming

## Configure

Run `idf.py menuconfig` and open `Example Configuration`:

1. Set the RGB LCD timing parameters.
2. Set the RGB LCD GPIO assignments.
3. Choose whether the source image comes from the embedded JPEG or the pre-generated `RGB565` image.

## Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A picture with 'ESP32' word will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from the ESP Component Registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (1555) main_task: Calling app_main()
I (1565) ppa_rgb: Turn off LCD backlight
I (1575) ppa_rgb: Install RGB LCD panel driver
I (1605) ppa_rgb: Initialize RGB LCD panel
I (1615) ppa_rgb: Turn on LCD backlight
I (1795) ppa_rgb: JPEG image decoded! Size of the decoded image is: 320px x 240px
start srm operations
start blend operations
start fill operations
I (10085) main_task: Returned from app_main()
```

## Reference

- [ESP-IDF: Pixel-Processing Accelerator](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/api-reference/peripherals/ppa.html)
- [ESP-IDF: RGB LCD](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/api-reference/peripherals/lcd/rgb_lcd.html)
