| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PPA Transform Example

## Overview

This example demonstrates how to use `esp_driver_ppa` to process an embedded RGB565 image without any display hardware.

The example embeds a pre-generated raw RGB565 image, processes it with three PPA engines, then base64-encodes the final RGB565 buffer and prints it to the serial console. The accompanying pytest script reconstructs the image as a PPM file and compares it with a golden reference image.

The processing pipeline demonstrates:

- SRM: rotate the full image by 180 degrees
- Blend: overlay a cyan A8-mask highlight using a fixed foreground color
- Fill: draw an amber frame and blue corner markers

## Hardware Required

* An ESP development board with PPA support
* An USB cable for power supply and programming

## Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (1555) main_task: Calling app_main()
Loading embedded RGB565 image from flash...
Embedded raw image size: 153600 bytes
Transforming image with PPA SRM...
Blending highlight with PPA blend...
Drawing border with PPA fill...
IMAGE_META width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
PPA image processing demo done.
I (10085) main_task: Returned from app_main()
```

## Pytest Visual Check

The accompanying `pytest_ppa_transform.py` script captures the `IMAGE_META` and `IMAGE_BASE64` output, reconstructs the processed image, and saves it as:

- `dut.logdir/ppa_transform_result.ppm`

It also compares the generated image with `golden_result.ppm` by hashing the decoded RGB pixel content. This turns the example into a functional regression test and a visual artifact generator for CI logs.

### Getting The PPM Result Locally

If you want to inspect the processed image on your computer, first build the example for your target, then run pytest from the ESP-IDF root directory with the matching target and serial port:

```bash
pytest -k test_ppa_transform --target esp32p4 --port PORT
```

Replace `esp32p4` with another supported target such as `esp32s31`, and set `PORT` to your board's serial device.

`pytest-embedded` stores per-test logs under `$IDF_PATH/pytest-embedded/`. When the test finishes, pytest prints a log line similar to:

```text
Saved PPA artifact to .../pytest-embedded/<timestamp>/esp32p4.default.test_ppa_transform/ppa_transform_result.ppm
```

You can open the generated `ppa_transform_result.ppm` from that log directory with any image viewer to inspect the PPA output locally.

## Replacing The Embedded RGB Asset

The example embeds `main/assets/image.rgb`, a 320x240 RGB565 raw image.

You can regenerate a compatible raw asset with `ffmpeg`:

```bash
ffmpeg -y -i path/to/your/image.jpg -vf scale=320:240 -pix_fmt rgb565le -f rawvideo main/assets/image.rgb
```

After replacing the raw asset, rebuild and flash the example. The firmware will emit the processed image as base64, and pytest will save the reconstructed PPM artifact automatically. If the new image is intended to become the expected output, update `golden_result.ppm` as well.
