| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PPA Palette Example

## Overview

This example demonstrates the Color Look-Up Table (CLUT) of the PPA blend engine without any display hardware.

The example draws a small dashboard interface into an indexed (L8) picture, where every pixel stores which part of the interface it belongs to rather than a color. A color is only assigned later, by the CLUT. The interface is drawn once, and three themes are then applied to it by filling the CLUT with a different set of colors before each blend operation, so the whole screen is restyled without a single pixel being redrawn. Some theme colors are partly transparent, which lets the wallpaper behind the interface show through the card surface. All three RGB565 results are base64-encoded and printed to the serial console. The accompanying pytest script reconstructs the images as PPM files and compares them with three golden reference images.

The processing pipeline demonstrates:

- Software preprocessing: generate an RGB565 wallpaper, then draw the interface as an L8 index picture
- Fill the foreground CLUT with the colors of one theme through `ppa_set_color_lookup_table()`
- Blend the indexed interface over the wallpaper, once per theme
- Release the CLUT once no indexed picture is used anymore

Only as many CLUT entries are written as the interface actually uses, ten in this example. Since ten roles also fit in four bits, the same interface could be stored in the L4 color mode at half the size. An L4 picture packs two pixels into every byte, the pixel with the even x coordinate in the low nibble and the pixel with the odd x coordinate in the high nibble, so only the code that draws the interface would have to change.

## Hardware Required

* An ESP development board with PPA support
* An USB cable for power supply and programming

## Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (1497) main_task: Calling app_main()
Generating wallpaper...
Drawing the L8 interface...
Interface size: 76800 bytes, the same interface in RGB565 would take 153600 bytes
Applying the day theme...
IMAGE_META effect=day width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
Applying the night theme...
IMAGE_META effect=night width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
Applying the high_contrast theme...
IMAGE_META effect=high_contrast width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
Releasing the CLUT...
PPA palette demo done.
I (63537) main_task: Returned from app_main()
```

## Pytest Visual Check

The accompanying `pytest_ppa_palette.py` script captures each `IMAGE_META` and `IMAGE_BASE64` payload, reconstructs all three themed screens, and saves them as:

- `dut.logdir/ppa_palette_day.ppm`
- `dut.logdir/ppa_palette_night.ppm`
- `dut.logdir/ppa_palette_high_contrast.ppm`

It also compares the generated images with `golden_day.ppm`, `golden_night.ppm` and `golden_high_contrast.ppm` by hashing the decoded RGB pixel content, and checks that the three results differ from each other, which is what proves that the CLUT alone restyled the interface. This turns the example into a functional regression test and a visual artifact generator for CI logs.

### Getting The PPM Result Locally

If you want to inspect the processed images on your computer, first build the example for your target, then run pytest from the ESP-IDF root directory with the matching target and serial port:

```bash
pytest pytest_ppa_palette.py --target esp32p4 --port PORT
```

Replace `esp32p4` with another supported target such as `esp32s31`, and set `PORT` to your board's serial device.

`pytest-embedded` stores per-test logs under `$IDF_PATH/pytest-embedded/`. When the test finishes, pytest prints a log line similar to:

```text
Saved PPA artifact to .../pytest-embedded/<timestamp>/esp32p4.default.test_ppa_palette/ppa_palette_day.ppm
```

You can open the generated PPM files from that log directory with any image viewer to compare the three themes locally.

### Changing Or Adding A Theme

A theme is one row of the `s_themes` table in `main/ppa_palette_example_main.c`, which gives a color and an alpha to every role of `example_role_t`. Adding a theme or changing a color needs no change to the drawing code, but the matching golden `.ppm` files have to be regenerated, since the comparison is an exact pixel match.
