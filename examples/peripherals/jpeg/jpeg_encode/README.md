| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# JPEG encode example

## Overview

This example demonstrates how to use the JPEG hardware [encoder](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/api-reference/peripherals/jpeg.html) to encode a 1080p picture:

This example makes use of the hardware-based JPEG encoder. If you have multiple pictures that need to be decoded, such as *.rgb -> *.jpg, you can use this example to accelerate encoding.

## How to use example

### Hardware Required

* An Espressif development board based on a chip listed in supported targets
* A USB cable for power supply and serial communication
* Computer with ESP-IDF installed and configured
* The raw picture is the only source that you need to prepare (We have an [esp1080p.rgb](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/jpeg/jpeg_encode/resources/esp1080.rgb) in resources folder, you can also get it from [jpeg_decode](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/jpeg/jpeg_decode) example).
* ffmpeg can also be used to produce rgb picture. For example `ffmpeg -i input.jpg -pix_fmt rgb24 output.rgb`

### Build and Flash

Before you start build and flash this example, please put the image `esp1080.rgb` in your sdcard.

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (1114) jpeg.example: Initializing SD card
I (1114) gpio: GPIO[43]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1124) gpio: GPIO[44]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1134) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1144) gpio: GPIO[40]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1154) gpio: GPIO[41]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1164) gpio: GPIO[42]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (1414) gpio: GPIO[42]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
Name: SD64G
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 60906MB
CSD: ver=2, sector_size=512, capacity=124735488 read_bl_len=9
SSR: bus_width=4
I (1434) jpeg.example: infile_1080p:/sdcard/esp1080.rgb
I (5174) jpeg.example: outfile:/sdcard/outjpg.jpg
I (5284) jpeg.example: Card unmounted
I (5284) main_task: Returned from app_main()
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
