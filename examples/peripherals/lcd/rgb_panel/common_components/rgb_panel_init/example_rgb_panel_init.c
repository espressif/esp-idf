/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "example_rgb_panel_init.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_lcd_panel_rgb.h"

static const char *TAG = "rgb_panel_init";

static void s_fill_data_gpio_nums(gpio_num_t data_gpio_nums[ESP_LCD_RGB_BUS_WIDTH_MAX])
{
    data_gpio_nums[0] = EXAMPLE_PIN_NUM_DATA0;
    data_gpio_nums[1] = EXAMPLE_PIN_NUM_DATA1;
    data_gpio_nums[2] = EXAMPLE_PIN_NUM_DATA2;
    data_gpio_nums[3] = EXAMPLE_PIN_NUM_DATA3;
    data_gpio_nums[4] = EXAMPLE_PIN_NUM_DATA4;
    data_gpio_nums[5] = EXAMPLE_PIN_NUM_DATA5;
    data_gpio_nums[6] = EXAMPLE_PIN_NUM_DATA6;
    data_gpio_nums[7] = EXAMPLE_PIN_NUM_DATA7;
    data_gpio_nums[8] = EXAMPLE_PIN_NUM_DATA8;
    data_gpio_nums[9] = EXAMPLE_PIN_NUM_DATA9;
    data_gpio_nums[10] = EXAMPLE_PIN_NUM_DATA10;
    data_gpio_nums[11] = EXAMPLE_PIN_NUM_DATA11;
    data_gpio_nums[12] = EXAMPLE_PIN_NUM_DATA12;
    data_gpio_nums[13] = EXAMPLE_PIN_NUM_DATA13;
    data_gpio_nums[14] = EXAMPLE_PIN_NUM_DATA14;
    data_gpio_nums[15] = EXAMPLE_PIN_NUM_DATA15;
#if CONFIG_EXAMPLE_LCD_DATA_LINES > 16
    data_gpio_nums[16] = EXAMPLE_PIN_NUM_DATA16;
    data_gpio_nums[17] = EXAMPLE_PIN_NUM_DATA17;
    data_gpio_nums[18] = EXAMPLE_PIN_NUM_DATA18;
    data_gpio_nums[19] = EXAMPLE_PIN_NUM_DATA19;
    data_gpio_nums[20] = EXAMPLE_PIN_NUM_DATA20;
    data_gpio_nums[21] = EXAMPLE_PIN_NUM_DATA21;
    data_gpio_nums[22] = EXAMPLE_PIN_NUM_DATA22;
    data_gpio_nums[23] = EXAMPLE_PIN_NUM_DATA23;
#endif
}

esp_err_t example_rgb_panel_init_backlight(void)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&bk_gpio_config), TAG, "configure backlight gpio failed");
#endif

    return ESP_OK;
}

void example_rgb_panel_set_backlight(bool on)
{
    if (EXAMPLE_PIN_NUM_BK_LIGHT < 0) {
        return;
    }

    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, on ? EXAMPLE_LCD_BK_LIGHT_ON_LEVEL : EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL);
}

esp_err_t example_rgb_panel_new(esp_lcd_panel_handle_t *panel_handle)
{
    ESP_RETURN_ON_FALSE(panel_handle, ESP_ERR_INVALID_ARG, TAG, "panel handle is null");

    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = CONFIG_EXAMPLE_LCD_DATA_LINES,
        .dma_burst_size = 64,
        .num_fbs = EXAMPLE_RGB_PANEL_NUM_FBS,
#if CONFIG_EXAMPLE_USE_BOUNCE_BUFFER
        .bounce_buffer_size_px = 20 * EXAMPLE_LCD_H_RES,
#endif
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            .hsync_back_porch = EXAMPLE_LCD_HBP,
            .hsync_front_porch = EXAMPLE_LCD_HFP,
            .hsync_pulse_width = EXAMPLE_LCD_HSYNC,
            .vsync_back_porch = EXAMPLE_LCD_VBP,
            .vsync_front_porch = EXAMPLE_LCD_VFP,
            .vsync_pulse_width = EXAMPLE_LCD_VSYNC,
            .flags = {
                .pclk_active_neg = true,
            },
        },
        .flags.fb_in_psram = true,
    };
    s_fill_data_gpio_nums(panel_config.data_gpio_nums);
    return esp_lcd_new_rgb_panel(&panel_config, panel_handle);
}

esp_err_t example_rgb_panel_init(esp_lcd_panel_handle_t panel_handle)
{
    ESP_RETURN_ON_FALSE(panel_handle, ESP_ERR_INVALID_ARG, TAG, "panel handle is null");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel_handle), TAG, "reset RGB panel failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel_handle), TAG, "init RGB panel failed");
    return ESP_OK;
}

esp_err_t example_rgb_panel_deinit(esp_lcd_panel_handle_t panel_handle)
{
    ESP_RETURN_ON_FALSE(panel_handle, ESP_ERR_INVALID_ARG, TAG, "panel handle is null");
    return esp_lcd_panel_del(panel_handle);
}
