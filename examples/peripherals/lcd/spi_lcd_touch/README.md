| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# SPI LCD and Touch Panel Example

This example demonstrates how to interface an ESP32 development board with an SPI LCD display and optional touch controller using the ESP-IDF framework. The example creates an animated user interface using the LVGL graphics library. The touch controller and the LCD display share the same SPI bus, allowing for efficient communication and resource usage.

## What This Example Does

This example:

-   Initializes an ST7789 LCD display connected via SPI interface
-   Sets up LVGL (Light and Versatile Graphics Library) for creating user interfaces
-   Displays an animated arc widget that continuously rotates
-   Includes a button that allows you to rotate the screen orientation
-   Optionally supports touch input through STMPE610 or XPT2046 touch controllers

## Key Components Used

### ESP-IDF Components

-   **esp_lcd**: Provides the abstracted LCD panel driver interface and ST7789 device driver
-   **esp_timer**: Generates periodic ticks required by LVGL for animations
-   **driver/spi_master**: Creates and manages the SPI bus for communication with the LCD and touch controller

### External Components (from Component Registry)

-   **LVGL**: Modern graphics library for creating user interfaces
-   **esp_lcd_touch_stmpe610**: Touch controller driver for STMPE610
-   **esp_lcd_touch_xpt2046**: Touch controller driver for XPT2046

## Technical Implementation

The example implements several important concepts:

-   **Thread Safety**: Uses mutex locks to protect LVGL API calls since LVGL is not thread-safe
-   **Double Buffering**: Implements efficient display updates using dual display buffers
-   **Memory Management**: Uses DMA-capable memory allocation for optimal performance
-   **Event Handling**: Demonstrates proper callback registration for display and touch events

## How to use the example

### Hardware Requirements

To run this example, you will need:

-   **ESP Development Board**: Any compatible board, the supported ESP chips are listed in the table above.
-   **ST7789 LCD Display**: 240x320 pixel TFT display with SPI interface
-   **USB Cable**: For power supply and programming
-   **Jumper Wires**: For connecting the display to your ESP board
-   **Touch Controller**: STMPE610 or XPT2046 touch controller (if you want touch functionality)

### Hardware Connections

Connect your ESP development board to the ST7789 LCD display as shown below. Make sure to power off your board before making any connections.

### Wiring Diagram

```text
ESP Development Board                    ST7789 LCD Display
+----------------------+              +--------------------+
|             GND      +------------->| GND                |
|                      |              |                    |
|             3V3      +------------->| VCC                |
|                      |              |                    |
|             PCLK     +------------->| SCL                |
|                      |              |                    |
|             MOSI     +------------->| MOSI               |
|                      |              |                    |
|             MISO     |<-------------+ MISO               |
|                      |              |                    |
|             RST      +------------->| RES                |
|                      |              |                    |
|             DC       +------------->| DC                 |
|                      |              |                    |
|             LCD CS   +------------->| LCD CS             |
|                      |              |                    |
|             TOUCH CS +------------->| TOUCH CS           |
|                      |              |                    |
|             BK_LIGHT +------------->| BLK                |
+----------------------+              +--------------------+
```

### Pin Configuration Notes

-   **GPIO Pin Numbers**: The pin assignments shown above are the default values defined in the example code. You can modify these in the [main.c](main/spi_lcd_touch_example_main.c) file if needed.
-   **SPI Interface**: This example uses SPI2 by default. The MISO pin (GPIO 21) is optional and only needed if you use the touch controller to read the touch coordinates.
-   **Backlight Control**: The backlight pin controls the display brightness. The example uses active-low logic by default (set `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` to 1 if your display requires active-high).
-   **Touch Controller**: If using a touch controller, it shares the same SPI bus but uses a separate chip select pin.

## Getting Started

### Configuration Options

This example provides several configuration options through the ESP-IDF configuration system:

```bash
idf.py menuconfig
```

Navigate to "Example Configuration" to find:

-   **Enable LCD touch**: Toggle touch controller support on/off
-   **LCD touch controller model**: Choose between STMPE610 and XPT2046
-   **Display mirroring options**: Adjust for different touch controller orientations

### Exiting the Monitor

To exit the serial monitor, press `Ctrl+]`.

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## What to Expect

When the example runs successfully, you should see:

### Visual Output

-   **LCD Display**: The screen will show an animated rotating arc (progress bar style)
-   **Rotate Button**: A button labeled "ROTATE" in the bottom-left corner
-   **Smooth Animation**: The arc continuously rotates, demonstrating LVGL's animation capabilities

### Touch Interaction (if enabled)

-   **Screen Rotation**: Tap the "ROTATE" button to cycle through different screen orientations (0°, 90°, 180°, 270°)
-   **Touch Feedback**: The display responds immediately to touch input

### Console Output

```text
I (307) main_task: Started on CPU0
I (307) main_task: Calling app_main()
I (307) example: Turn off LCD backlight
I (307) example: Initialize SPI bus
I (317) example: Install panel IO
I (317) example: Install ST7789 panel driver
I (447) example: Turn on LCD backlight
I (447) example: Initialize LVGL library
I (457) example: Install LVGL tick timer
I (457) example: Register io panel event callback for LVGL flush ready notification
I (457) example: Create LVGL task
I (467) example: Starting LVGL task
I (547) example: Display LVGL Meter Widget
I (557) main_task: Returned from app_main()
```

## Learning Resources

### Documentation Links

-   [ESP-IDF SPI LCD Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd/spi_lcd.html)
-   [LVGL Documentation](https://docs.lvgl.io/master/)
-   [ESP Component Registry](https://components.espressif.com/)

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
