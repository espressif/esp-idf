| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# MIPI DSI LCD Panel Example

[esp_lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html) supports MIPI DSI interfaced LCD panel, with frame buffer(s) managed by the driver itself.

This example shows the general process of installing a MIPI DSI LCD driver, and displays a LVGL widget on the screen.

## How to use the example

### Hardware Required

* An ESP development board, which with MIPI DSI peripheral supported
* A general MIPI DSI LCD panel, with 2 data lanes and 1 clock lane, this example will use the [ILI9881C](https://components.espressif.com/components/espressif/esp_lcd_ili9881c) for demonstration
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```text
       ESP Board                         MIPI DSI LCD Panel
+-----------------------+              +-------------------+
|                   GND +--------------+ GND               |
|                       |              |                   |
|                   3V3 +--------------+ VCC               |
|                       |              |                   |
|             DSI_CLK_P +--------------+ DSI_CLK_P         |
|             DSI_CLK_N +              + DSI_CLK_N         |
|                       |              |                   |
|            DSI_DAT0_P +--------------+ DSI_DAT0_P        |
|            DAI_DAT0_N +              + DAI_DAT0_N        |
|                       |              |                   |
|            DSI_DAT1_P +--------------+ DSI_DAT1_P        |
|            DSI_DAT1_N +              + DSI_DAT1_N        |
|                       |              |                   |
|                       |              |                   |
|              BK_LIGHT +--------------+ BLK               |
|                       |              |                   |
|                 Reset +--------------+ Reset             |
|                       |              |                   |
+-----------------------+              +-------------------+
```

Before testing your LCD, you also need to read your LCD spec carefully, and then adjust the values like "resolution" and "blank time" in the [main](./main/mipi_dsi_lcd_example_main.c) file.

### Configure

Run `idf.py menuconfig` and go to `Example Configuration`:

* Choose whether to `Use DMA2D to copy draw buffer to frame buffer` asynchronously. If you choose `No`, the draw buffer will be copied to the frame buffer synchronously by CPU.
* Choose if you want to `Monitor FPS by GPIO`. If you choose `Yes`, then you can attach an oscilloscope or logic analyzer to the GPIO pin to monitor the FPS of the display.
  Please note, the actual FPS should be **double** the square wave frequency.

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A LVGL widget should show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
...
I (1629) example: MIPI DSI PHY Powered on
I (1629) example: Install MIPI DSI LCD control panel
I (1639) ili9881c: ID1: 0x98, ID2: 0x81, ID3: 0x5c
I (1779) example: Install MIPI DSI LCD data panel
I (1799) example: Initialize LVGL library
I (1799) example: Allocate separate LVGL draw buffers from PSRAM
I (1809) example: Use esp_timer as LVGL tick timer
I (1809) example: Create LVGL task
I (1809) example: Starting LVGL task
I (1919) example: Display LVGL Meter Widget
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
