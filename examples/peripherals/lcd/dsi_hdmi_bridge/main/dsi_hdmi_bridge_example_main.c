/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lcd_lt9611.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_ldo_regulator.h"
#include "esp_log.h"
#include "lvgl_port.h"

static const char *TAG = "dsi_hdmi_bridge";

// The "VDD_MIPI_DPHY" should be supplied with 2.5V, it can source from the internal LDO regulator or from external LDO chip
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN       3  // LDO_VO3 is connected to VDD_MIPI_DPHY
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV 2500
#define EXAMPLE_MIPI_DSI_LANE_NUM               2
#define EXAMPLE_PIN_NUM_LCD_RST                 -1

// Update these values according to the board I2C wiring.
#define EXAMPLE_I2C_PORT_NUM                    0
#define EXAMPLE_PIN_NUM_SDA                     7
#define EXAMPLE_PIN_NUM_SCL                     8

static void example_enable_mipi_dphy_power(void)
{
    const esp_ldo_channel_config_t ldo_config = {
        .chan_id = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    esp_ldo_channel_handle_t ldo = NULL;
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_config, &ldo));
    ESP_LOGI(TAG, "MIPI D-PHY powered by LDO channel %d at %d mV",
             EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN,
             EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV);
}

static i2c_master_bus_handle_t create_i2c_bus(void)
{
    const i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = EXAMPLE_I2C_PORT_NUM,
        .sda_io_num = EXAMPLE_PIN_NUM_SDA,
        .scl_io_num = EXAMPLE_PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus));
    return bus;
}

static esp_lcd_dsi_bus_handle_t create_dsi_bus(void)
{
    const esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = EXAMPLE_MIPI_DSI_LANE_NUM,
        .lane_bit_rate_mbps = LT9611_1920x1080_60HZ_RGB16_LANE_BITRATE_MBPS,
    };
    esp_lcd_dsi_bus_handle_t bus = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &bus));
    return bus;
}

static esp_lcd_panel_handle_t create_lt9611_panel(i2c_master_bus_handle_t i2c_bus,
                                                  esp_lcd_dsi_bus_handle_t dsi_bus)
{
    const esp_lcd_dpi_panel_config_t dpi_config = LT9611_1920x1080_60HZ_RGB16_DPI_PANEL_CONFIG_WITH_FBS(1);
    const lt9611_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = dsi_bus,
            .dpi_config = &dpi_config,
            .lane_num = EXAMPLE_MIPI_DSI_LANE_NUM,
        },
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .flags.reset_active_high = true,
        .vendor_config = (void *) &vendor_config,
    };
    esp_lcd_panel_handle_t panel = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_lt9611(i2c_bus, &panel_config, &panel));
    return panel;
}

void app_main(void)
{
    example_enable_mipi_dphy_power();
    i2c_master_bus_handle_t i2c_bus = create_i2c_bus();
    esp_lcd_dsi_bus_handle_t dsi_bus = create_dsi_bus();
    esp_lcd_panel_handle_t panel = create_lt9611_panel(i2c_bus, dsi_bus);

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_enable_dma2d(panel));
    // Draw the first LVGL frame into the framebuffer before starting DPI scanout.
    example_lvgl_init(panel);
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
}
