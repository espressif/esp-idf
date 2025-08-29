| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# I2C U8G2 Display Demo

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to use the U8G2 graphics library with SSD1306 display over I2C interface using the new ESP-IDF I2C master driver. The demo showcases various U8G2 features in a continuous loop, including:

- **Text Display**: Multiple font sizes and styles
- **Geometric Shapes**: Rectangles, circles, triangles, and lines  
- **Pixel Manipulation**: Individual pixel control and dot patterns
- **Animated Progress Bar**: Dynamic progress from 0% to 100%
- **Bouncing Ball Animation**: Physics simulation with collision detection
- **Bitmap Display**: Custom icons and images

The project features a modular design with demo functions separated into dedicated source files for better maintainability and code organization. You could always add or remove various showcases to test out the graphic features in u8g2 library.

## How to use example

### Hardware Required

To run this example, you should have:

- An ESP32 development board (or other supported targets)
- SSD1306 display with I2C interface (128x64 recommended)
- Jumper wires for connections

If you are using a display other than SSD1306 that is supported by the u8g2 library, you can modify the `u8g2_Setup_ssd1306_i2c_128x64_noname_f` function in [`main/i2c_u8g2_main.c`](main/i2c_u8g2_main.c) to match your specific display controller. For details, please refer to: https://github.com/olikraus/u8g2/wiki/u8g2setupc.

### Configure the Project

Run `menuconfig` to configure the project settings:

```bash
idf.py menuconfig
```

Navigate to `Example Configuration` where you can configure:

- **I2C Configuration Items:**
  - I2C Display Address
  - SCL GPIO Number
  - SDA GPIO Number
  - Master Frequency

All parameters can be adjusted without code modification to match your hardware setup.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

```bash
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The display will show a continuous demo loop with various graphics demonstrations. And the terminal will print log meanwhile:

```bash
I (303) U8G2: Starting U8G2 display demo program (menuconfig based configuration)
I (303) U8G2: I2C Configuration: SDA=GPIO5, SCL=GPIO4, Freq=400000Hz, Timeout=1000ms
I (313) U8G2: Display Configuration: Address=0x3C
I (313) U8G2: Initializing display...
I (313) U8G2: GPIO and delay initialization completed
I (323) U8G2: I2C master driver initialized successfully
I (623) U8G2: Setting power mode...
I (623) U8G2: Display initialization completed
I (623) U8G2: Starting U8G2 demo sequence
I (2663) U8G2: Demo cycle: 1
I (2663) DEMO: Text Display
I (5693) DEMO: Geometric Shapes
I (8723) DEMO: Pixel Operations
I (11753) DEMO: Progress Bar
I (15283) DEMO: Animation Effects
I (24083) DEMO: Bitmap Display
```

## Troubleshooting

### Display Not Working
- **Check connections**: Verify SDA/SCL wiring and power supply (3.3V)
- **I2C Address**: Try different addresses in menuconfig (common values: 0x3C, 0x3D)
- **Pull-up resistors**: Ensure adequate pull-up resistors (2-10kΩ recommended)
- **Display compatibility**: Verify your display uses SSD1306 controller

### Communication Issues  
- **Lower I2C frequency**: Reduce frequency in menuconfig if experiencing errors
- **Increase timeout**: Adjust I2C timeout value for slow devices
- **Check grounding**: Ensure proper ground connections between ESP32 and display

## Technical Notes

- Uses the new ESP-IDF I2C master driver (`esp_driver_i2c`) instead of the legacy driver
- U8G2 library dependency is managed automatically via ESP Component Manager
- All configuration parameters are accessible through `menuconfig` without code changes
- Display reset pin support is optional (set to -1 to disable)
- Compatible with various SSD1306 display sizes and configurations

## References

- [ESP-IDF I2C API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)
- [U8G2 Library Documentation](https://github.com/olikraus/u8g2/wiki)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)