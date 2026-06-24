| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# JPEG encode example

## Overview

This example demonstrates how to use the JPEG hardware [encoder](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/api-reference/peripherals/jpeg.html) to encode a 720p raw image.

The example performs:

- Embedding `main/assets/esp720p.rgb` into the final firmware image
- Letting the JPEG encoder read one 1280x720 `bgr24` frame directly from flash
- Encoding the frame into JPEG with the hardware encoder
- Base64-encoding the resulting JPEG bitstream and printing it with machine-parseable markers
- Letting pytest rebuild `jpeg_encode_result.jpeg` and compare it against `golden_output.jpeg`

## Hardware Required

Any board based on a supported target can be used, provided it has enough flash to hold the embedded 720p raw asset and the application image. The example defaults are configured for a 4 MB flash layout and PSRAM-enabled builds.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

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

## Replacing The Embedded RGB Asset

If you want to regenerate a compatible raw frame from another input image, one simple workflow is:

```bash
ffmpeg -y -i input.jpg -vf scale=1280:720 -pix_fmt bgr24 -f rawvideo main/assets/esp720p.rgb
```

After replacing the raw asset, rebuild and flash the example. The firmware will emit the encoded JPEG as base64, and pytest will save the reconstructed JPEG artifact automatically. If the new image is intended to become the expected output, update `golden_output.jpeg` as well.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
