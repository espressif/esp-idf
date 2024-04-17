| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# JPEG decode example

## Overview

This example demonstrates how to use the JPEG hardware decoder to decode a 1080p and a 720p picture:

If you have a bunch of big JPEG picture need to be decoded, such as `*.jpg` -> `*.rgb`, and this example uses hardware JPEG decoder to accelerate the decoding.

## How to use example

### Prerequisites Required

This example demonstrates the flexibility of decoding pictures by decoding two different sizes: one in 1080p and another in 720p. It showcases how you can easily modify the code to meet your specific requirements, such as only decoding 1080p photos.

### Build and Flash

Before you start build and flash this example, please put the image `esp720.jpg` and `esp1080.jpg` in your sdcard.

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (1116) jpeg.example: Initializing SD card
I (1116) gpio: GPIO[43]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1126) gpio: GPIO[44]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1136) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1146) gpio: GPIO[40]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1156) gpio: GPIO[41]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (1166) gpio: GPIO[42]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (1416) gpio: GPIO[42]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
Name: SD64G
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 60906MB
CSD: ver=2, sector_size=512, capacity=124735488 read_bl_len=9
SSR: bus_width=4
I (1436) jpeg.example: jpg_file_1080:/sdcard/esp1080.jpg
I (1696) jpeg.example: jpg_file_1080:/sdcard/esp720.jpg
I (1796) jpeg.example: header parsed, width is 1920, height is 1080
I (1846) jpeg.example: raw_file_1080:/sdcard/out.rgb
I (11836) jpeg.example: raw_file_720:/sdcard/out2.rgb
I (13336) jpeg.example: Card unmounted
I (13336) main_task: Returned from app_main()
```

Also, the helper script [open_raw_picture.py](./open_raw_picture.py) simplifies the visualization of the output on your computer. For this to work, go to `examples/peripheral/jpeg/jpeg_decode` and install the requirements by running `pip install -r requirements.txt`.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)