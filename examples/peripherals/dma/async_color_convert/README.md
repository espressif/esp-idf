| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# Async Color Convert Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to use the Async Color Convert driver (`esp_async_color_convert.h`) with the DMA2D backend.

The example performs:

- Loading an embedded UYVY422 raw image from flash
- Letting DMA2D read the source image directly from mapped flash
- Performing a blocking UYVY422 -> RGB888 conversion with the Async Color Convert driver
- Base64-encoding the converted BGR24 image and printing it with machine-parseable markers
- Letting pytest decode the payload, save a PPM artifact, and compare it against a golden reference image

## Hardware Required

Any board with a supported ESP target that mentioned in the above table can be used.

## Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
Loading embedded UYVY image from flash...
Embedded image size: 12288 bytes
Converting UYVY422 -> RGB888...
Converted image size: 18432 bytes
IMAGE_META width=96 height=64 format=BGR24 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64 ...
IMAGE_BASE64_END
Async color convert visual demo done.
```

## Visual Result In Pytest

The accompanying pytest script captures the `IMAGE_META` and `IMAGE_BASE64` output, reconstructs the converted image, and saves it as:

- `dut.logdir/async_color_convert_result.ppm`

It also compares the generated result with `golden_result.ppm` by hashing the decoded RGB pixel content. This turns the example into a regression test as well as a visual demo: the image must both render correctly for a human and match the stored golden output for CI.

## Replacing The Embedded UYVY Asset

The example embeds `main/assets/sample_96x64_uyvy.yuv`.

You can regenerate a compatible asset from any PNG with `ffmpeg`. One simple workflow is:

```bash
ffmpeg -y -i input.png -vf scale=96:64 -pix_fmt uyvy422 -f rawvideo sample_96x64_uyvy.yuv
```

After replacing the `.yuv` file, rebuild and flash the example. The firmware will emit the converted image as base64, and pytest will save the resulting PPM artifact automatically. If you intend the new image to become the expected output, update `golden_result.ppm` as well so the regression check stays in sync.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
