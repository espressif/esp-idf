| Supported Targets | ESP32-P4 | ESP32-S3 |
| ----------------- | -------- | -------- |

# RGB LCD Panel Example

[esp_lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/lcd/rgb_lcd.html) supports RGB interfaced LCD panel, with multiple buffer modes. This example shows the general process of installing an RGB panel driver, and displays a scatter chart on the screen based on the LVGL library.

This example uses the [esp_timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html) to generate the ticks needed by LVGL and uses a dedicated task to run the `lv_timer_handler()`. Since the LVGL APIs are not thread-safe, this example uses a mutex which be invoked before the call of `lv_timer_handler()` and released after it. The same mutex needs to be used in other tasks and threads around every LVGL (lv_...) related function call and code. For more porting guides, please refer to [LVGL Display porting reference](https://docs.lvgl.io/master/porting/display.html).

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

1. `Use single frame buffer`: The RGB LCD driver allocates one frame buffer and mount it to the DMA. The example also allocates one draw buffer for the LVGL library. The draw buffer contents are copied to the frame buffer by the CPU.
2. `Use double frame buffer`: The RGB LCD driver allocates two frame buffers and mount them to the DMA. The LVGL library draws directly to the offline frame buffer while the online frame buffer is displayed by the RGB LCD controller.
3. `Use bounce buffer`: The RGB LCD driver allocates one frame buffer and two bounce buffers. The bounce buffers are mounted to the DMA. The frame buffer contents are copied to the bounce buffers by the CPU. The example also allocates one draw buffer for the LVGL library. The draw buffer contents are copied to the frame buffer by the CPU.
4. Choose the number of LCD data lines in `RGB LCD Data Lines`
5. Set the GPIOs used by RGB LCD peripheral in `GPIO assignment`, e.g. the synchronization signals (HSYNC, VSYNC, DE) and the data lines

### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A scatter chart will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from the ESP Component Registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
...
I (872) main_task: Started on CPU0
I (882) esp_psram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (882) main_task: Calling app_main()
I (892) example: Turn off LCD backlight
I (892) example: Install RGB LCD panel driver
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
  * Please pay attention to the level used to turn on the LCD backlight, some LCD module needs a low level to turn it on, while others take a high level. You can change the backlight level macro `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` in [lvgl_example_main.c](main/rgb_lcd_example_main.c).
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
  * Please read the datasheet of the IC used by your LCD module, and check if it needs a special initialization sequence. The initialization is usually done by sending some specific SPI commands and parameters to the IC. After the initialization, the LCD will be ready to receive RGB data. For simplicity, this example only works out of the box for those LCD modules which don't need extra initialization.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
