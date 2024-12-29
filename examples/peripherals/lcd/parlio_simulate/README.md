| Supported Targets | ESP32-C5 | ESP32-H2 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# Parallel IO simulation of SPI or I80 Interfaced LCD example

This example demonstrates how to use the Parallel IO lcd driver to simulate an I80 interface or SPI interface LCD.

The whole code is located in [parlio_simulate_example_main.c](main/parlio_simulate_example_main.c), and the UI demo code is located in [lvgl_demo_ui.c](main/lvgl_demo_ui.c).

The Parallel IO peripheral is designed for high-speed parallel communication, making it particularly well-suited for scenarios that demand rapid data transfers. A typical application is LCD Display Driving. The data width can be adjusted, allowing for flexibility and optimization of data transfer efficiency based on application requirements. Currently, the driver supports setting 1-bit data width to drive the SPI interfaced screen and setting 8-bit data width to drive the I80 interfaced screen.

## How to use the example

### Hardware Required

* An ESP development board
* An SPI or Intel 8080 interfaced (so called MCU interface or parallel interface) LCD (this example uses ST7789)
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the I80 LCD is as follows:

```text
   ESP Board                      LCD Screen
+---------------+              +----------------+
|               |              |                |
|           3V3 +------------->| VCC            |
|               |              |                |
|           GND +--------------+ GND            |
|               |              |                |
|    DATA[0..7] |<------------>| DATA[0..7]     |
|               |              |                |
|          PCLK +------------->| PCLK           |
|               |              |                |
|            CS +------------->| CS             |
|               |              |                |
|           D/C +------------->| D/C            |
|               |              |                |
|           RST +------------->| RST            |
|               |              |                |
|      BK_LIGHT +------------->| BCKL           |
|               |              |                |
+---------------+              +----------------+
```

The connection between ESP Board and the SPI LCD is as follows:

```text
   ESP Board                      LCD Screen
+---------------+              +----------------+
|               |              |                |
|           3V3 +------------->| VCC            |
|               |              |                |
|           GND +--------------+ GND            |
|               |              |                |
| MOSI(DATA[0]) +------------->| MOSI           |
|               |              |                |
|          PCLK +------------->| PCLK           |
|               |              |                |
|            CS +------------->| CS             |
|               |              |                |
|           D/C +------------->| D/C            |
|               |              |                |
|           RST +------------->| RST            |
|               |              |                |
|      BK_LIGHT +------------->| BCKL           |
|               |              |                |
+---------------+              +----------------+
```

Especially, please pay attention to the binary signal level used to turn the LCD backlight on, some LCD modules need a low level to turn it on, while others require a high level. You can change the backlight level macro `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` in [parlio_simulate_example_main.c](main/parlio_simulate_example_main.c).

### Build and Flash

Run `idf.py set-target <target-name>` to select one supported target that can run this example. This step will also apply the default Kconfig configurations into the `sdkconfig` file.

Run `idf.py menuconfig` to open a terminal UI where you can tune specific configuration for this example in the `Example Configuration` menu.

* `LCD interface`: Choose the interface to simulate in the example.
* `Allocate color data from PSRAM`: Select this option if you want to allocate the LVGL draw buffers from PSRAM.
* `Pixel clock frequency (Hz)`: Set the pixel clock frequency for the LCD controller.
* `LCD GPIO Configuration`: Select the GPIO number used by this example

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A fancy animation will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from the ESP Component Registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Example Output

```bash
I (1542) main_task: Calling app_main()
I (1542) example: Turn off LCD backlight
I (1542) gpio: GPIO[48]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (1552) example: Initialize Parlio device
I (1562) example: Install LCD driver of st7789
I (1562) gpio: GPIO[35]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (1732) example: Turn on LCD backlight
I (1732) example: Initialize LVGL library
I (1732) example: buf1@0x48001040, buf2@0x4800cc00
I (1732) example: Register display driver to LVGL
I (1732) example: Install LVGL tick timer
I (1732) example: Create LVGL task
I (1742) example: Starting LVGL task
I (1752) example: Display LVGL animation
I (1762) main_task: Returned from app_main()
```

## Troubleshooting

* Can't get a stable UI when `EXAMPLE_LCD_PARLIO_COLOR_IN_PSRAM` is enabled.

   This is because of the limited PSRAM bandwidth, compared to the internal SRAM. You can either decrease the PCLK clock `EXAMPLE_LCD_PIXEL_CLOCK_HZ` from the menuconfig or increase the PSRAM working frequency `SPIRAM_SPEED` from the KConfig (e.g. **Component-config** -> **ESP PSRAM** -> **PSRAM config** -> **Set PSRAM clock speed**) or decrease the FPS in LVGL configuration. In addition, enabling `SPIRAM_XIP_FROM_PSRAM` can help increase the PCLK frequency when the Frame Buffer is allocated from the PSRAM.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
