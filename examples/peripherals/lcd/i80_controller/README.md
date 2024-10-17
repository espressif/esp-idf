| Supported Targets | ESP32 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# i80 LCD LVGL porting example

LVGL is an open-source graphics library for creating modern GUIs. It has plenty of built-in graphical elements with low memory footprint, which is friendly for embedded GUI applications.

This example can be taken as a skeleton of porting the LVGL library onto the `esp_lcd` driver layer. **Note** that, this example only focuses on the display interface, regardless of the input device driver.

The whole porting code is located in [i80_controller_example_main.c](main/i80_controller_example_main.c), and the UI demo code is located in [lvgl_demo_ui.c](main/lvgl_demo_ui.c).

The UI will display two images (one Espressif logo and another Espressif text), which have been converted into C arrays by the [online converting tool](https://lvgl.io/tools/imageconverter), and will be compiled directly into application binary.

This example is constructed by [IDF component manager](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html), all the external dependency will be handled by the CMake build system automatically. In this case, it will help download the lvgl from the [ESP Component Registry](https://components.espressif.com/component/lvgl/lvgl), with the version specified in the [manifest file](main/idf_component.yml).

This example uses the [esp_timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_timer.html) to generate the ticks needed by LVGL and uses a dedicated task to run the `lv_timer_handler()`. Since the LVGL APIs are not thread-safe, this example uses a mutex which be invoked before the call of `lv_timer_handler()` and released after it. The same mutex needs to be used in other tasks and threads around every LVGL (lv_...) related function call and code. For more porting guides, please refer to [LVGL porting doc](https://docs.lvgl.io/master/porting/index.html).

## How to use the example

### Hardware Required

* An ESP development board
* An Intel 8080 interfaced (so called MCU interface or parallel interface) LCD (this example can use ST7789, NT35510 or ILI9341)
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```text
   ESP Board                      LCD Screen
+-------------+              +----------------+
|             |              |                |
|         3V3 +------------->| VCC            |
|             |              |                |
|         GND +--------------+ GND            |
|             |              |                |
|  DATA[0..7] |<------------>| DATA[0..7]     |
|             |              |                |
|        PCLK +------------->| PCLK           |
|             |              |                |
|          CS +------------->| CS             |
|             |              |                |
|         D/C +------------->| D/C            |
|             |              |                |
|         RST +------------->| RST            |
|             |              |                |
|    BK_LIGHT +------------->| BCKL           |
|             |              |                |
+-------------+              +----------------+
```

The GPIO number used by this example can be changed in [i80_controller_example_main.c](main/i80_controller_example_main.c).
Especially, please pay attention to the binary signal level used to turn the LCD backlight on, some LCD modules need a low level to turn it on, while others require a high level. You can change the backlight level macro `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` in [i80_controller_example_main.c](main/i80_controller_example_main.c).

### Build and Flash

Run `idf.py set-target <target-name>` to select one supported target that can run this example. This step will also apply the default Kconfig configurations into the `sdkconfig` file.

Run `idf.py menuconfig` to open a terminal UI where you can tune specific configuration for this example in the `Example Configuration` menu.

* `i80 LCD controller model`: Choose the LCD model to use by the example. If you choose `NT35510`, there will be another relevant configuration `NT35510 Data Width`, to choose the data line width for your NT35510 LCD module.
* `Allocate color data from PSRAM`: Select this option if you want to allocate the LVGL draw buffers from PSRAM.
* `Pixel clock frequency (Hz)`: Set the pixel clock frequency for the LCD controller.
* `LCD image source from`: Select where to load the image resource. See [Image Resource](#image-resource) for more details.

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A fancy animation will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from the ESP Component Registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
I (0) cpu_start: Starting scheduler on APP CPU.
I (418) example: Turn off LCD backlight
I (418) gpio: GPIO[2]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (428) example: Initialize Intel 8080 bus
I (438) example: Install LCD driver of st7789
I (558) example: Turn on LCD backlight
I (558) example: Initialize LVGL library
I (558) example: Register display driver to LVGL
I (558) example: Install LVGL tick timer
I (558) example: Create LVGL task
I (558) example: Starting LVGL task
I (638) example: Display LVGL animation
```

## Image Resource

This example supports two ways of reading images

* from the [SPIFFS file system](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html). This is the suggested way we use in the example. It may take a little bit longer to load the image because of the bottleneck of the SPI flash read speed, but it will save the binary size.
* from the embedded binary (i.e., pre-decode the image into an array and pack it together with the application firmware). By this way, you can get faster image loading speed at the cost of bloating your application binary. What's worse, if you enabled the [XIP from PSRAM](https://github.com/espressif/esp-idf/tree/master/examples/system/xip_from_psram) feature, it will increase the PSRAM usage as well.

## Troubleshooting

* Can't get a stable UI when `EXAMPLE_LCD_I80_COLOR_IN_PSRAM` is enabled.

   This is because of the limited PSRAM bandwidth, compared to the internal SRAM. You can either decrease the PCLK clock `EXAMPLE_LCD_PIXEL_CLOCK_HZ` from the menuconfig or increase the PSRAM working frequency `SPIRAM_SPEED` from the KConfig (e.g. **ESP32S3-Specific** -> **Set RAM clock speed**) or decrease the FPS in LVGL configuration. For illustration, this example has set the refresh period to 100ms in the default sdkconfig file.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
