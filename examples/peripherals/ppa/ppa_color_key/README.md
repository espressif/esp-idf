| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PPA Color Key Example

## Overview

This example demonstrates two common color-keying effects of the PPA blend engine without any display hardware.

The example is intentionally tailored to its embedded demo asset. It embeds a pre-generated raw RGB565 image, generates a centered RGB888 glow-style foreground image in software, then runs two PPA blend color-keying passes. The first pass replaces the red `ESP32` text with the generated glow. The second pass keeps the red keyed text untouched while the remaining area blends with the same generated effect. The glow geometry is fixed because the demo asset keeps the text near the center of the frame. Both final RGB565 buffers are base64-encoded and printed to the serial console. The accompanying pytest script reconstructs the images as PPM files and compares them with two golden reference images.

The processing pipeline demonstrates:

- Software preprocessing: generate a fixed centered RGB888 glow foreground
- Blend color key effect 1: replace the red keyed text with the generated RGB888 glow
- Blend color key effect 2: preserve the red keyed text while blending the generated RGB888 glow into the non-key area

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
Generating shared glow foreground...
Replacing keyed red text with glow foreground...
IMAGE_META effect=replace_keyed_text_with_glow width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
Blending glow around keyed red text...
IMAGE_META effect=blend_glow_around_keyed_text width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
PPA color key demo done.
I (10085) main_task: Returned from app_main()
```

## Pytest Visual Check

The accompanying `pytest_ppa_color_key.py` script captures each `IMAGE_META` and `IMAGE_BASE64` payload, reconstructs both processed images, and saves them as:

- `dut.logdir/ppa_color_key_replace_keyed_text_with_glow.ppm`
- `dut.logdir/ppa_color_key_blend_glow_around_keyed_text.ppm`

It also compares the generated images with `golden_replace_keyed_text_with_glow.ppm` and `golden_blend_glow_around_keyed_text.ppm` by hashing the decoded RGB pixel content. This turns the example into a functional regression test and a visual artifact generator for CI logs.

### Getting The PPM Result Locally

If you want to inspect the processed image on your computer, first build the example for your target, then run pytest from the ESP-IDF root directory with the matching target and serial port:

```bash
pytest -k test_ppa_color_key --target esp32p4 --port PORT
```

Replace `esp32p4` with another supported target such as `esp32s31`, and set `PORT` to your board's serial device.

`pytest-embedded` stores per-test logs under `$IDF_PATH/pytest-embedded/`. When the test finishes, pytest prints a log line similar to:

```text
Saved PPA artifact to .../pytest-embedded/<timestamp>/esp32p4.default.test_ppa_color_key/ppa_color_key_replace_keyed_text_with_glow.ppm
Saved PPA artifact to .../pytest-embedded/<timestamp>/esp32p4.default.test_ppa_color_key/ppa_color_key_blend_glow_around_keyed_text.ppm
```

You can open both generated PPM files from that log directory with any image viewer to inspect the two PPA color-key effects locally.

