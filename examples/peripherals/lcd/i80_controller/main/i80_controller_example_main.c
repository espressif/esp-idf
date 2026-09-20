/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "lvgl_port.h"

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     CONFIG_EXAMPLE_LCD_PIXEL_CLOCK_HZ

#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_DATA0          CONFIG_EXAMPLE_PIN_NUM_DATA0
#define EXAMPLE_PIN_NUM_DATA1          CONFIG_EXAMPLE_PIN_NUM_DATA1
#define EXAMPLE_PIN_NUM_DATA2          CONFIG_EXAMPLE_PIN_NUM_DATA2
#define EXAMPLE_PIN_NUM_DATA3          CONFIG_EXAMPLE_PIN_NUM_DATA3
#define EXAMPLE_PIN_NUM_DATA4          CONFIG_EXAMPLE_PIN_NUM_DATA4
#define EXAMPLE_PIN_NUM_DATA5          CONFIG_EXAMPLE_PIN_NUM_DATA5
#define EXAMPLE_PIN_NUM_DATA6          CONFIG_EXAMPLE_PIN_NUM_DATA6
#define EXAMPLE_PIN_NUM_DATA7          CONFIG_EXAMPLE_PIN_NUM_DATA7
#define EXAMPLE_PIN_NUM_PCLK           CONFIG_EXAMPLE_PIN_NUM_PCLK
#define EXAMPLE_PIN_NUM_CS             CONFIG_EXAMPLE_PIN_NUM_CS
#define EXAMPLE_PIN_NUM_DC             CONFIG_EXAMPLE_PIN_NUM_DC
#define EXAMPLE_PIN_NUM_RST            CONFIG_EXAMPLE_PIN_NUM_RST
#define EXAMPLE_PIN_NUM_BK_LIGHT       CONFIG_EXAMPLE_PIN_NUM_BK_LIGHT

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              240
#define EXAMPLE_LCD_V_RES              280
#define EXAMPLE_LCD_BUS_WIDTH          8
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

#define EXAMPLE_DMA_BURST_SIZE         64 // 16, 32, 64. Higher burst size can improve the performance when the DMA buffer comes from PSRAM

static void example_init_i80_bus(esp_lcd_panel_io_handle_t *io_handle)
{
    ESP_LOGI(TAG, "Initialize Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .wr_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            EXAMPLE_PIN_NUM_DATA3,
            EXAMPLE_PIN_NUM_DATA4,
            EXAMPLE_PIN_NUM_DATA5,
            EXAMPLE_PIN_NUM_DATA6,
            EXAMPLE_PIN_NUM_DATA7,
        },
        .bus_width = EXAMPLE_LCD_BUS_WIDTH,
        .max_transfer_bytes = EXAMPLE_LCD_H_RES * 100 * sizeof(uint16_t),
        .dma_burst_size = EXAMPLE_DMA_BURST_SIZE,
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, io_handle));
}

static void example_init_lcd_panel(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t *panel)
{
    ESP_LOGI(TAG, "Install LCD driver of st7789");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    // Set inversion, x/y coordinate order, x/y mirror according to your LCD module spec
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_invert_color(panel_handle, true);
    esp_lcd_panel_set_gap(panel_handle, 0, 20);
    *panel = panel_handle;
}

void app_main(void)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL);
#endif // EXAMPLE_PIN_NUM_BK_LIGHT >= 0

    // The LVGL demo UI loads its images from a LittleFS partition, so make sure
    // the filesystem is mounted before the UI is started.
    example_init_filesystem();

    esp_lcd_panel_io_handle_t io_handle = NULL;
    example_init_i80_bus(&io_handle);

    esp_lcd_panel_handle_t panel_handle = NULL;
    example_init_lcd_panel(io_handle, &panel_handle);

    // Stub: user can flush pre-defined pattern to the screen before we turn on the screen or backlight

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif // EXAMPLE_PIN_NUM_BK_LIGHT >= 0

    // Initialize LVGL and start the demo UI (see lvgl_port.c / lvgl_demo_ui.c)
    example_init_lvgl(io_handle, panel_handle, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
}
