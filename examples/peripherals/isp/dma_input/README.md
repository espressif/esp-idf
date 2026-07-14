| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# ISP DMA Input Visual Test Example

## Overview

This example generates a standard RAW8 Bayer color-bar pattern in software, writes it into a DMA-capable PSRAM input buffer, feeds it into the ISP through DW-GDMA, applies the ISP color adjustment module, and prints the RGB888 output as base64. The pytest script decodes the output into PPM images for inspection.

The data flow is:

1. A synthetic RAW8 Bayer **color-bar** pattern is generated in software into the ISP DMA input buffer.
2. The pattern is transferred into the ISP via `DW-GDMA → ISP DMA input`.
3. The ISP processes the data (demosaic, color adjustment) and outputs RGB888 (BGR24 byte layout).
4. The RGB888 frame is base64-encoded and printed with machine-parseable markers.
5. pytest decodes the payload, swaps BGR→RGB, and saves one PPM file per frame.

## Hardware Required

- An ESP32-P4 devkit with PSRAM (this example allocates the ISP DMA input/output buffers from PSRAM).

If you replace the software-generated pattern with a RAW image embedded in flash, copy it into a DMA-capable buffer before feeding it to the ISP DMA input path.

## How to Use

Run the test locally. It builds and flashes the example, captures the serial output, and saves one PPM artifact per frame:

```
cd examples/peripherals/isp/dma_input
pytest pytest_isp_dma_input.py --target esp32p4 --port PORT
```

The pytest log directory (`dut.logdir`) contains `reference.ppm` for the standard reference color-bar image and `isp_dma_input_frame00.ppm`, ... for the decoded ISP output frames. The pytest script checks that the decoded frame contains the expected standard color-bar structure and verifies that the ISP color brightness adjustment brightens the black bar.

ISP feature-specific setup is kept under `main/isp_features/<feature>/`, and pytest feature checks are kept under `pytest_features/<feature>.py`. The current color brightness check lives in `main/isp_features/color/` and `pytest_features/color_brightness.py`; new ISP feature checks can follow the same pattern while reusing the common DW-GDMA input/output and image parsing flow.

## Example Output

Each frame uses the same standard vertical color-bar input pattern with positive color brightness enabled to validate the ISP color adjustment module.

```text
Feeding 1 frames through ISP DMA input...
IMAGE_META frame=0 width=128 height=96 format=BGR24 encoding=base64 color_brightness=64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64_END
Frame 0 done
ISP DMA visual demo done.
```

## Reference

- [ESP-IDF: Image Signal Processor](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/api-reference/peripherals/isp.html)
