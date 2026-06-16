| Supported Targets | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# JPEG decode example

## Overview

This example demonstrates how to use the JPEG hardware decoder to decode one embedded JPEG image into `RGB888` raw bytes in default `BGR24` order.

The example performs:

- Embedding `main/assets/image.jpg` into the application image
- Letting the JPEG decoder read the embedded JPEG bitstream directly from flash
- Parsing the JPEG header with `jpeg_decoder_get_info()`
- Decoding the image into an `RGB888` output buffer with default `BGR24` byte order
- Allocating the output buffer for padded dimensions when the JPEG block layout rounds width or height up to 16-pixel boundaries
- Base64-encoding the decoded raw pixels and printing them with machine-parseable UART markers
- Letting pytest rebuild `jpeg_decode_result.ppm` for inspection and compare it against `golden_output.ppm`

## Hardware Required

Any board based on a supported target can be used. No SD card or external storage setup is required.

## Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
Loading embedded JPEG from flash...
Embedded JPEG size: 43700 bytes
JPEG header parsed: width=320 height=240
Decoding JPEG -> RGB888...
Decoded RGB888 size: 245760 bytes
JPEG_DECODE_INFO width=320 height=240 padded_width=320 padded_height=256 format=RGB888 encoding=base64 size=245760
JPEG_DECODE_BASE64_BEGIN
JPEG_DECODE_BASE64 ...
JPEG_DECODE_BASE64 ...
JPEG_DECODE_BASE64_END
JPEG decode demo done.
```

`padded_width` and `padded_height` report the actual decoded buffer dimensions. For JPEGs whose block layout pads the output to 16-pixel boundaries, these values can be larger than the visible `width` and `height`, so the output buffer must be sized for the padded image.

## Pytest Regression Check

The accompanying `pytest_jpeg_decode.py` script waits for the `JPEG_DECODE_INFO` and `JPEG_DECODE_BASE64` markers, reconstructs the decoded raw pixel output, crops away padded rows, and saves the visible image as:

- `dut.logdir/jpeg_decode_result.ppm`

The test writes the `PPM` file and compares it with `golden_output.ppm`. This makes the example both a functional regression test and a host-side artifact generator for inspection.

## Running Pytest Locally And Viewing The Image

To run the pytest helper locally on hardware, build the example for your target first, then invoke the test script with the target and serial port:

```bash
idf.py set-target esp32p4 build
pytest --target esp32p4 --port PORT pytest_jpeg_decode.py
```

Replace `esp32p4` with another supported target such as `esp32s31` when needed.

`pytest-embedded` stores per-test logs under `$IDF_PATH/pytest-embedded/`. The script writes the reconstructed image to `jpeg_decode_result.ppm` inside that test log directory, so after the test finishes you can open the generated `PPM` file locally with an image viewer that supports `PPM` to inspect the decoded output.

## Replacing The Embedded JPEG Asset

If you want to try another input image, replace:

- `main/assets/image.jpg`

Keep the replacement as a baseline JPEG with the same general scale if you want UART log volume and test runtime to stay small.

After replacing the asset, rerun the example and update `golden_output.ppm` from the generated `dut.logdir/jpeg_decode_result.ppm` artifact if the new output should become the expected result.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
