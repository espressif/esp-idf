| Supported Targets | ESP32-P4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- |

# RGB LCD Panel Example

[esp_lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/lcd/rgb_lcd.html) supports RGB interfaced LCD panels with multiple buffering modes. This example shows how to create an RGB panel driver, connect it to LVGL, and display a simple demo UI.

This example uses the [esp_timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html) to generate LVGL ticks, and a dedicated task to run `lv_timer_handler()`. Since LVGL APIs are not thread-safe, the example also uses a mutex around every LVGL call.

If you are new to this example, the easiest reading order is:

1. `main/rgb_lcd_example_main.c`: overall initialization flow
2. `main/example_rgb_lcd_panel.h`: LCD timing and GPIO definitions
3. `main/example_rgb_lcd_panel.c`: RGB panel creation and initialization helpers

This example uses 3 kinds of **buffering mode**:

| Driver Buffers  | LVGL draw buffers   | Pros and Cons |
|-----------------|---------------------|-----------------------------------------------------------------------------------|
| 1 Frame Buffer  | Two partial buffers | fewest memory footprint, performance affected by memory copy                      |
| 2 Frame Buffers | Direct refresh mode | no extra memory copy between draw buffer and frame buffer, large memory cost      |
| 1 Frame Buffer + </br> 2 Bounce Buffers | Two partial buffers | Fast DMA read, high CPU usage, more internal memory cost  |

## How to use the example

### Hardware Required

* An ESP development board, which supports the RGB LCD peripheral
* A general RGB panel, 16/24 bit-width, with HSYNC, VSYNC and DE signal
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```text
       ESP Board                           RGB  Panel
+-----------------------+              +-------------------+
|                   GND +--------------+GND                |
|                       |              |                   |
|                   3V3 +--------------+VCC                |
|                       |              |                   |
|                   PCLK+--------------+PCLK               |
|                       |              |                   |
|              DATA[N:0]+--------------+DATA[N:0]          |
|                       |              |                   |
|                  HSYNC+--------------+HSYNC              |
|                       |              |                   |
|                  VSYNC+--------------+VSYNC              |
|                       |              |                   |
|                     DE+--------------+DE                 |
|                       |              |                   |
|               BK_LIGHT+--------------+BLK                |
+-----------------------+              |                   |
                               3V3-----+DISP_EN            |
                                       |                   |
                                       +-------------------+
```

### Configure

Run `idf.py menuconfig` and go to `Example Configuration`:

1. `Use single frame buffer`: The driver allocates one full-screen frame buffer. LVGL renders into a smaller draw buffer, and the CPU copies the dirty area into the frame buffer.
2. `Use double frame buffer`: The driver allocates two full-screen frame buffers. LVGL renders directly into the offline frame buffer while the RGB LCD controller scans out the online frame buffer.
3. `Use bounce buffer`: The driver allocates one full-screen frame buffer plus two internal bounce buffers. This can help when PSRAM bandwidth is not enough, at the cost of extra CPU usage and internal RAM.
4. Choose the number of LCD data lines in `RGB LCD Data Lines`.
5. Set the RGB GPIOs in `RGB LCD GPIO assignment`, including `PCLK`, `HSYNC`, `VSYNC`, `DE`, and the data lines.

You will usually need to update the timing values in [example_rgb_lcd_panel.h](main/example_rgb_lcd_panel.h) to match your LCD datasheet.

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash, and monitor the project. If the timing and GPIO settings match your hardware, the LCD should show the LVGL demo UI.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from the ESP Component Registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
...
I (872) main_task: Started on CPU0
I (882) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (882) main_task: Calling app_main()
I (892) example: Initialize LCD backlight
I (892) example: Turn off LCD backlight
I (892) example: Create RGB LCD panel
I (922) example: Initialize RGB LCD panel
I (922) example: Turn on LCD backlight
I (922) example: Initialize LVGL library
I (932) example: Allocate LVGL draw buffers
I (932) example: Register event callbacks
I (932) example: Install LVGL tick timer
I (942) example: Create LVGL task
I (942) example: Starting LVGL task
I (992) example: Display LVGL UI
I (1102) main_task: Returned from app_main()
...
```

## Troubleshooting

* Why the LCD doesn't light up?
  * Please pay attention to the level used to turn on the LCD backlight, some LCD module needs a low level to turn it on, while others take a high level. You can change the backlight level macro `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` in [example_rgb_lcd_panel.h](main/example_rgb_lcd_panel.h).
  * Some LCD modules also require a separate initialization sequence over SPI or I2C before they can accept RGB data. This example only covers the RGB data path itself.
* Where to allocate the frame buffer?
  * The frame buffer of RGB panel is located in ESP side (unlike other controller based LCDs, where the frame buffer is located in external chip). As the frame buffer usually consumes much RAM (depends on the LCD resolution and color depth), we recommend to put the frame buffer into PSRAM (like what we do in this example). However, putting frame buffer in PSRAM will limit the maximum PCLK due to the bandwidth of **SPI0**.
* Why LCD screen drifts?
  * Slow down the PCLK frequency
  * Adjust other timing parameters like PCLK clock edge (by `pclk_active_neg`), sync porches like VBP (by `vsync_back_porch`) according to your LCD spec
  * Enable `CONFIG_SPIRAM_XIP_FROM_PSRAM`, which can saves some bandwidth of SPI0 from being consumed by ICache.
* How to further increase the PCLK frequency?
  * Enable `CONFIG_EXAMPLE_USE_BOUNCE_BUFFER`, which will make the LCD controller fetch data from internal SRAM (instead of the PSRAM), but at the cost of increasing CPU usage.
  * Enable `CONFIG_SPIRAM_XIP_FROM_PSRAM` can also help if the you're not using the bounce buffer mode. These two configurations can save some **SPI0** bandwidth from being consumed by ICache.
* Why the RGB timing is correct but the LCD doesn't show anything?
  * Please read the datasheet of the IC used by your LCD module, and check if it needs a special initialization sequence. The initialization is usually done by sending specific commands and parameters over SPI or I2C. After that sequence, the LCD will be ready to receive RGB data. For simplicity, this example only works out of the box for panels that do not need extra initialization.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
