| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PPA FreeType Example

## Overview

This example demonstrates how to render [Font Awesome](https://fontawesome.com/) icons with the FreeType library and composite them over a picture with the PPA blend engine, all without any display hardware.

The example uses the `espressif/freetype` component (declared in `main/idf_component.yml`) to rasterize two rows of three icons:

1. `star` (U+F005), `heart` (U+F004), `sun` (U+F185)
2. `face-smile` (U+F118), `bell` (U+F0F3), `envelope` (U+F0E0)

It then generates a vertical gradient background in software, and lets the PPA blend engine composite every glyph over the gradient with a fixed white text color, one icon per `ppa_do_blend` call. The final RGB565 overlay result is base64-encoded and printed to the serial console. The accompanying pytest script reconstructs the composite as a color PPM and compares it with the golden reference image.

The processing pipeline demonstrates:

- FreeType: initialize the library, load an OTF font from a LittleFS partition, and rasterize each glyph into a small per-glyph A8 alpha mask
- Blend: call `ppa_do_blend` once per glyph, moving the blend target region (block) to each icon's bounding box and compositing the A8 glyph mask over the gradient using `fg_fix_rgb_val` as the icon color

Blending one character at a time avoids building a full-frame A8 mask in software and allocating a large intermediate mask buffer, and it shows a multi-blend loop that moves the target region for every glyph.

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
LittleFS mounted
FreeType library initialized
Font loaded
Compositing text with PPA blend...
Rendering icon row at baseline Y=120: U+F005 U+F004 U+F185
Rendering icon row at baseline Y=180: U+F118 U+F0F3 U+F0E0
IMAGE_META effect=text_overlay width=320 height=240 format=RGB565 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
PPA FreeType demo done.
I (10085) main_task: Returned from app_main()
```

## Pytest Visual Check

The accompanying `pytest_ppa_freetype.py` script captures the `IMAGE_META` and `IMAGE_BASE64` output, reconstructs the composite image, and saves it as:

- `dut.logdir/ppa_freetype_text_overlay.ppm`

The test compares the reconstructed RGB888 image byte-for-byte with `golden_text_overlay.ppm`. This turns the example into a functional regression test and a visual artifact generator for CI logs.

### Getting The PPM Result Locally

If you want to inspect the processed image on your computer, first build the example for your target, then run pytest from the example directory with the matching target and serial port:

```bash
pytest pytest_ppa_freetype.py --target esp32p4 --port PORT
```

Replace `esp32p4` with another supported target such as `esp32s31`, and set `PORT` to your board's serial device.

`pytest-embedded` stores per-test logs under `$IDF_PATH/pytest-embedded/`. When the test finishes, pytest prints a log line similar to:

```text
Saved PPA artifact to .../pytest-embedded/<timestamp>/esp32p4.default.test_ppa_freetype/ppa_freetype_text_overlay.ppm
```

You can open the generated PPM file from that log directory with any image viewer to inspect the PPA composite output locally.

## Changing The Icons Or Font

The rendered icons, font size, baselines, and icon color are all plain `#define`s at the top of `main/ppa_freetype_example_main.c`:

- `EXAMPLE_ICON_ROW1` / `EXAMPLE_ICON_ROW2` - the two rows of Font Awesome code points (3 per row)
- `EXAMPLE_ROW1_BASELINE_Y` / `EXAMPLE_ROW2_BASELINE_Y` - the baseline row of each row in the 320x240 frame
- `EXAMPLE_FONT_PIXEL_SIZE` - the pixel size passed to `FT_Set_Pixel_Sizes()`
- `EXAMPLE_FG_R/G/B` - the fixed icon color used by the blend engine

### Font Source And Format

The example displays Font Awesome 7.3.1 Free regular icons. Font Awesome ships its Free fonts in several flavours: WOFF2 (`webfonts/fa-regular-400.woff2`) and OpenType (`otfs/Font Awesome 7 Free-Regular-400.otf`). The `espressif/freetype` component is built without WOFF2/brotli support, so it cannot decode a WOFF2 file directly. The OpenType (OTF) flavour, however, is fully supported by FreeType.

So instead of committing a font binary into the tree, `main/CMakeLists.txt` downloads the OTF variant from the Font Awesome 7.3.1 release at build time (`Font Awesome 7 Free-Regular-400.otf` from <https://github.com/FortAwesome/Font-Awesome/raw/refs/tags/7.3.1/otfs/Font%20Awesome%207%20Free-Regular-400.otf>) and packs it into the `fonts` LittleFS partition as `fa-regular-400.otf`.

The font is cached in the build directory after its first successful download. If the download fails, CMake stops with the upstream error instead of producing an invalid LittleFS image.
