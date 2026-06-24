| Supported Targets | ESP32-P4 | ESP32-S3 |
| ----------------- | -------- | -------- |

# H.264 Encoder-Decoder Example

## Overview

This example demonstrates how to use H.264 hardware/software encoder and decoder with visual pattern generation:

- Generate colorful test patterns for video processing
- Encode video frames using H.264 codec (hardware on ESP32-P4, software on ESP32-S3)
- Decode the encoded frames back to original format using software decoder
- Display visual comparison between source and decoded images

The example supports multiple YUV formats and provides side-by-side colorized display in the console. All encoding parameters are configurable through the ESP-IDF menuconfig system.

## Configuration

This example provides comprehensive configuration options through `idf.py menuconfig`:

### H.264 Encoder Type Selection
- **Hardware Encoder**: Available only on ESP32-P4, provides better performance and lower power consumption
- **Software Encoder**: Available on all targets (ESP32-S3, ESP32-P4), uses more CPU resources

### Configurable Parameters
All parameters can be adjusted in "H.264 Example Configuration" menu:

- **Video Width**: 64-1920 pixels (default: 320)
- **Video Height**: 64-1080 pixels (default: 240) 
- **Frame Rate**: 1-60 fps (default: 30 for hardware, 15 for software)
- **Bitrate**: 64K-10M bps (default: 512K for hardware, 256K for software)
- **GOP Size**: 1-255 frames (default: 30)
- **QP Value**: 10-51 (default: 26 for hardware, 28 for software)

### Target-Specific Defaults
- **ESP32-P4**: Optimized for hardware encoding with higher performance settings
- **ESP32-S3**: Optimized for software encoding with conservative settings

## How to use example

### Prerequisites Required

This example requires:
- ESP32-P4 development board (for hardware encoding) or ESP32-S3 development board (for software encoding)
- USB cable for programming and power supply
- Terminal that supports ANSI color codes for proper visual output

### Configure the Example

Before building, configure the example parameters:

```bash
idf.py menuconfig
```

Navigate to: `Component config` → `H.264 Example Configuration`

1. **Select Encoder Type**: Choose between Hardware (ESP32-P4 only) or Software encoder
2. **Adjust Parameters**: Configure video resolution, frame rate, bitrate, etc.
3. **Save and Exit**: Press 'S' to save configuration

### Build and Flash

For ESP32-P4 (with hardware encoding support):
```bash
idf.py set-target esp32p4
idf.py menuconfig  # Configure as needed
idf.py build
idf.py -p PORT flash monitor
```

For ESP32-S3 (software encoding only):
```bash
idf.py set-target esp32s3
idf.py menuconfig  # Software encoder will be automatically selected
idf.py build
idf.py -p PORT flash monitor
```

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
- **ESP_H264_RAW_FMT_O_UYY_E_VYY**: Interlaced YUV format (hardware encoder input on ESP32-P4)

## Performance Recommendations

### For ESP32-P4 (Hardware Encoding):
- Resolution: Up to 1920x1080 supported
- Frame Rate: 30-60 fps achievable
- Bitrate: 512K-5M bps recommended
- QP: 20-30 for optimal quality/performance balance

### For ESP32-S3 (Software Encoding):
- Resolution: 320x240 or smaller recommended
- Frame Rate: 10-15 fps for stable performance  
- Bitrate: 256K-1M bps recommended
- QP: 28-35 for better performance

## Troubleshooting

**Configuration Issues:**
- Use `idf.py menuconfig` to verify H.264 settings before building
- Ensure hardware encoder is only selected for ESP32-P4 target

**Memory allocation failures:**
- Reduce resolution or frame rate in menuconfig
- Ensure sufficient SPIRAM is available
- Check ESP-IDF memory configuration

**Encoding/decoding errors:**
- Verify the correct target is selected (ESP32-P4 for hardware)
- Check that H.264 component is properly configured in menuconfig
- Adjust bitrate settings for your resolution/frame rate combination

**Performance Issues:**
- Lower resolution, frame rate, or bitrate for software encoding
- Use hardware encoder on ESP32-P4 for better performance
- Increase QP value to reduce computational load

**Visual output issues:**
- Ensure your terminal supports ANSI color codes
- Try different terminal applications if colors don't display properly

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
