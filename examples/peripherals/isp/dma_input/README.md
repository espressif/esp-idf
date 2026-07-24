| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# ISP DMA Input Visual Test Example

## Overview

This example embeds a 240 x 280 RAW8 Bayer image of a real scene in flash, feeds it directly into the ISP through DW-GDMA, and prints the RGB888 output as base64. The ISP driver synchronizes the DMA buffers' cache automatically. The pytest script decodes the output into a PPM image and compares it with the checked-in golden image.

The data flow is:

1. The embedded 16-byte-aligned BGGR RAW8 image is transferred from mapped flash into the ISP via `DW-GDMA → ISP DMA input`.
2. The ISP processes the data (demosaic, color adjustment) and outputs RGB888 (BGR24 byte layout).
3. The RGB888 frame is base64-encoded and printed with machine-parseable markers.
4. pytest decodes the payload, swaps BGR→RGB, saves one PPM file per frame, and compares it with the golden image.

## Hardware Required

- An ESP32-P4 devkit with PSRAM (this example allocates the ISP output buffer from PSRAM).

## How to Use

Run the test locally. It builds and flashes the example, captures the serial output, and saves one PPM artifact per frame:

```
cd examples/peripherals/isp/dma_input
pytest pytest_isp_dma_input.py --target esp32p4 --port PORT
```

The pytest log directory (`dut.logdir`) contains `isp_dma_input_frame00.ppm`, the decoded RGB888 ISP output from the current hardware run. The repository includes [golden/golden.ppm](golden/golden.ppm), the checked-in reference image. The test compares the decoded RGB888 pixels with this reference image, making image-quality regressions visible in review and detectable in CI.

## Example Output

The example processes one embedded 240 x 280 BGGR RAW8 frame.

```text
Feeding 1 frames through ISP DMA input...
IMAGE_META frame=0 width=240 height=280 format=BGR24 encoding=base64
IMAGE_BASE64_BEGIN
IMAGE_BASE64 ...
IMAGE_BASE64_END
Frame 0 done
ISP DMA visual demo done.
```
