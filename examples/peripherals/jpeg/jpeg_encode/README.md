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
* ffmpeg can also be used to produce rgb picture. For example `ffmpeg -i input.jpg -pix_fmt rgb24 output.rgb`

### Build and Flash

Before you start build and flash this example, please put the image `esp1080.rgb` in your sdcard.

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
Loading embedded BGR24 image from flash...
Embedded raw image size: 2764800 bytes
Encoding BGR24(raw) -> JPEG...
Encoded JPEG size: 30795 bytes
JPEG_META width=1280 height=720 format=JPEG encoding=base64 size=30795
JPEG_BASE64_BEGIN
JPEG_BASE64 ...
JPEG_BASE64 ...
JPEG_BASE64_END
JPEG encode demo done.
```

## Pytest Visual Check

The accompanying `pytest_jpeg_encode.py` script captures the `JPEG_META` and `JPEG_BASE64` output, reconstructs the encoded JPEG, and saves it as:

- `dut.logdir/jpeg_encode_result.jpeg`

It also compares the generated JPEG with `golden_output.jpeg`. This turns the example into both a functional regression test and a host-side artifact generator that makes the encoded result easy to inspect.

## Running Pytest Locally And Viewing The Image

To run the pytest helper locally on hardware, build the example for your target first, then invoke the test script with the target and serial port:

```bash
pytest pytest_jpeg_encode.py --target esp32p4 --port PORT
```

Replace `esp32p4` with another supported target such as `esp32s31` when needed.

`pytest-embedded` stores per-test logs under `$IDF_PATH/pytest-embedded/`. The script writes the reconstructed image to `jpeg_encode_result.jpeg` inside that test log directory, so after the test finishes you can open the generated JPEG locally with any image viewer to inspect the encoded output.

## Replacing The Embedded RGB Asset

If you want to regenerate a compatible raw frame from another input image, one simple workflow is:

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
