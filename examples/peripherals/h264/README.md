| Supported Targets | ESP32-P4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- |

# H.264 Encoder-Decoder Example

## Overview

This example demonstrates how to use H.264 hardware/software encoder and decoder with visual pattern generation:

- Generate colorful test patterns for video processing
- Encode video frames using the H.264 hardware or software encoder
- Decode the encoded frames back to original format using software decoder
- Display visual comparison between source and decoded images

The example supports multiple YUV formats and provides side-by-side colorized display in the console. All encoding parameters are configurable through the ESP-IDF menuconfig system.

## Configuration

This example provides comprehensive configuration options through `idf.py menuconfig`:

### H.264 Encoder Type Selection
- **Hardware Encoder**: Selected by default on targets with H.264 encoding hardware; provides better performance and lower power consumption.
- **Software Encoder**: Available on all supported targets; uses more CPU resources.

### Configurable Parameters
All parameters can be adjusted in "H.264 Example Configuration" menu:

- **Video Width**: 64-1920 pixels (default: 320)
- **Video Height**: 64-1080 pixels (default: 240) 
- **Frame Rate**: 1-60 fps (default: 30 for hardware, 15 for software)
- **Bitrate**: 64K-10M bps (default: 512K for hardware, 256K for software)
- **GOP Size**: 1-255 frames (default: 30)
- **QP Value**: 10-51 (default: 26 for hardware, 28 for software)

### Defaults
The hardware encoder defaults to 30 fps, 512 Kbps, and QP 26. The software encoder uses conservative defaults of 15 fps, 256 Kbps, and QP 28.

## How to use example

### Prerequisites Required

This example requires:
- A development board for one of the supported targets
- USB cable for programming and power supply
- Terminal that supports ANSI color codes for proper visual output

### Configure the Example

Before building, configure the example parameters:

```bash
idf.py menuconfig
```

Navigate to: `Component config` → `H.264 Example Configuration`.

1. **Select Encoder Type**: Choose between Hardware (when supported by the target) or Software encoder
2. **Adjust Parameters**: Configure video resolution, frame rate, bitrate, etc.
3. **Save and Exit**: Press 'S' to save configuration

### Build and Flash

```bash
idf.py set-target TARGET
idf.py menuconfig  # Configure as needed
idf.py build
idf.py -p PORT flash monitor
```

Replace `TARGET` with a supported target from the table. The hardware encoder is selected automatically when the target provides the H.264 encoder capability; otherwise the software encoder is selected.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (1555) H264_ENC_DEC: H264 Example starting: 320x240 @ 30fps
I (1565) H264_ENC_DEC: Encoder: Hardware, Decoder: Software
I (1575) H264_ENC_DEC: Config: GOP=30, Bitrate=512000 bps, QP=26
I (1585) H264_DEC.SW: tinyh264 version: 1c7f584
I (1585) H264_ENC_DEC: H264 encode-decode loop started (320x240 @ 30fps)

Frame 0: source image             |            decoded image
[Colorized bar patterns displayed side by side using ANSI colors]

Frame 1: source image             |            decoded image
[Colorized bar patterns displayed side by side using ANSI colors]

Frame 2: source image             |            decoded image
[Colorized bar patterns displayed side by side using ANSI colors]
...

I (21465) H264_ENC_DEC: H264 process Completed successfully
I (21475) main_task: Returned from app_main()
```

*Note: The exact values shown will depend on your menuconfig settings.*

## Video Format Support

- **ESP_H264_RAW_FMT_I420**: Planar YUV 4:2:0 format (decoder output, software encoder input)
- **ESP_H264_RAW_FMT_O_UYY_E_VYY**: Interlaced YUV format (hardware encoder input)

## Performance Recommendations

### Hardware Encoder
- Resolution: 80x80 to 1920x1080 in this example
- Frame Rate: 30-60 fps is achievable, depending on resolution
- Bitrate: 512K-5M bps recommended
- QP: 20-30 for optimal quality/performance balance

### Software Encoder
- Resolution: 320x240 or smaller recommended
- Frame Rate: 10-15 fps for stable performance  
- Bitrate: 256K-1M bps recommended
- QP: 28-35 for better performance

## Troubleshooting

**Configuration Issues:**
- Use `idf.py menuconfig` to verify H.264 settings before building
- Select the hardware encoder only when it is offered by menuconfig for the current target

**Memory allocation failures:**
- Reduce resolution or frame rate in menuconfig
- Ensure sufficient SPIRAM is available
- Check ESP-IDF memory configuration

**Encoding/decoding errors:**
- Verify the selected encoder is supported by the target
- Check that H.264 component is properly configured in menuconfig
- Adjust bitrate settings for your resolution/frame rate combination

**Performance Issues:**
- Lower resolution, frame rate, or bitrate for software encoding
- Use the hardware encoder when it is available for better performance
- Increase QP value to reduce computational load

**Visual output issues:**
- Ensure your terminal supports ANSI color codes
- Try different terminal applications if colors don't display properly

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
