| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# MIPI DSI to HDMI Bridge Example

This example demonstrates how to drive an HDMI display from the MIPI DSI peripheral through an external DSI-to-HDMI bridge.

The example uses the [LT9611](https://components.espressif.com/components/espressif/esp_lcd_lt9611) at 1920x1080@60 Hz in RGB565. It displays a full-screen LVGL dashboard using DMA2D and partial draw buffers in internal RAM.

## How to use the example

### Hardware Required

* An ESP32-P4 development board
* An LT9611 DSI-to-HDMI bridge
* An HDMI display and cable
* A USB cable for power supply and programming

### Hardware Connection

The connection between the ESP board, the LT9611 bridge, and the HDMI display is as follows:

```text
       ESP Board                      LT9611 Bridge                    HDMI Display
+-----------------------+      +-------------------+          +-------------------+
|                   3V3 +------+ VCC               |          |                   |
|                       |      |                   |          |                   |
|             DSI_CLK_P +------+ DSI_CLK_P         |          |                   |
|             DSI_CLK_N +------+ DSI_CLK_N         |          |                   |
|            DSI_DAT0_P +------+ DSI_DAT0_P        |          |                   |
|            DSI_DAT0_N +------+ DSI_DAT0_N        |          |                   |
|            DSI_DAT1_P +------+ DSI_DAT1_P        |          |                   |
|            DSI_DAT1_N +------+ DSI_DAT1_N        |          |                   |
|                       |      |           HDMI OUT+----------+HDMI IN            |
|                   SDA +------+ SDA               |          |                   |
|                   SCL +------+ SCL               |          |                   |
|                 Reset +------+ Reset (optional)  |          |                   |
|                       |      |                   |          |                   |
+-----------------------+      +-------------------+          +-------------------+
```

Before building, update `EXAMPLE_PIN_NUM_LCD_RST`, `EXAMPLE_I2C_NUM`, `EXAMPLE_PIN_NUM_SDA`, and `EXAMPLE_PIN_NUM_SCL` in [dsi_hdmi_bridge_example_main.c](main/dsi_hdmi_bridge_example_main.c) according to the board wiring.

The example powers `VDD_MIPI_DPHY` from LDO channel 3 at 2500 mV. Update `EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN` and `EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV` in the source according to the board power design.

### Build and Flash

Run the following command to build, flash, and monitor the example:

```text
idf.py -p PORT build flash monitor
```

The first build can take additional time while the Component Manager resolves the external bridge and LVGL dependencies.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for complete instructions.

### Example Output

```text
I (...) dsi_hdmi_bridge: MIPI D-PHY powered by LDO channel 3 at 2500 mV
I (...) dsi_hdmi_bridge: Initialize LVGL
I (...) dsi_hdmi_bridge: Starting LVGL task
I (...) dsi_hdmi_bridge: Displaying LVGL UI at 1920x1080
```

## Troubleshooting

If the log shows:

```text
E lcd.dsi: can't fetch data from external memory fast enough, underrun happens
```

the DSI bridge cannot fetch the framebuffer from PSRAM fast enough. Reduce PSRAM traffic, for example:

* Disable PSRAM XIP (this example already does so for 1080p60 RGB565)
* Keep LVGL draw buffers in internal RAM instead of PSRAM
* Avoid other large PSRAM allocations or DMA while the display is running

For technical questions, please open an [issue](https://github.com/espressif/esp-idf/issues).
