/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"

#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "lcd_panel.nt35510";

static esp_err_t panel_nt35510_del(esp_lcd_panel_t *panel);
static esp_err_t panel_nt35510_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_nt35510_init(esp_lcd_panel_t *panel);
static esp_err_t panel_nt35510_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end,
                                           const void *color_data);
static esp_err_t panel_nt35510_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_nt35510_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_nt35510_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_nt35510_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_nt35510_disp_on_off(esp_lcd_panel_t *panel, bool off);
static esp_err_t panel_nt35510_sleep(esp_lcd_panel_t *panel, bool sleep);

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    int x_gap;
    int y_gap;
    uint8_t fb_bits_per_pixel;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    uint8_t colmod_val; // save current value of LCD_CMD_COLMOD register
} nt35510_panel_t;

esp_err_t
esp_lcd_new_panel_nt35510(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config,
                          esp_lcd_panel_handle_t *ret_panel)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    nt35510_panel_t *nt35510 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    nt35510 = calloc(1, sizeof(nt35510_panel_t));
    ESP_GOTO_ON_FALSE(nt35510, ESP_ERR_NO_MEM, err, TAG, "no mem for nt35510 panel");

    if (panel_dev_config->reset_gpio_num >= 0) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }

    switch (panel_dev_config->rgb_ele_order) {
    case LCD_RGB_ELEMENT_ORDER_RGB:
        nt35510->madctl_val = 0;
        break;
    case LCD_RGB_ELEMENT_ORDER_BGR:
        nt35510->madctl_val |= LCD_CMD_BGR_BIT;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported RGB element order");
        break;
    }

    uint8_t fb_bits_per_pixel = 0;
    switch (panel_dev_config->bits_per_pixel) {
    case 16: // RGB565
        nt35510->colmod_val = 0x55;
        fb_bits_per_pixel = 16;
        break;
    case 18: // RGB666
        nt35510->colmod_val = 0x66;
        // each color component (R/G/B) should occupy the 6 high bits of a byte, which means 3 full bytes are required for a pixel
        fb_bits_per_pixel = 24;
        break;
    case 24: // RGB888
        nt35510->colmod_val = 0x77;
        fb_bits_per_pixel = 24;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported pixel width");
        break;
    }

    nt35510->io = io;
    nt35510->fb_bits_per_pixel = fb_bits_per_pixel;
    nt35510->reset_gpio_num = panel_dev_config->reset_gpio_num;
    nt35510->reset_level = panel_dev_config->flags.reset_active_high;
    nt35510->base.del = panel_nt35510_del;
    nt35510->base.reset = panel_nt35510_reset;
    nt35510->base.init = panel_nt35510_init;
    nt35510->base.draw_bitmap = panel_nt35510_draw_bitmap;
    nt35510->base.invert_color = panel_nt35510_invert_color;
    nt35510->base.set_gap = panel_nt35510_set_gap;
    nt35510->base.mirror = panel_nt35510_mirror;
    nt35510->base.swap_xy = panel_nt35510_swap_xy;
    nt35510->base.disp_on_off = panel_nt35510_disp_on_off;
    nt35510->base.disp_sleep = panel_nt35510_sleep;
    *ret_panel = &(nt35510->base);
    ESP_LOGD(TAG, "new nt35510 panel @%p", nt35510);

    return ESP_OK;

err:
    if (nt35510) {
        if (panel_dev_config->reset_gpio_num >= 0) {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(nt35510);
    }
    return ret;
}

static esp_err_t panel_nt35510_del(esp_lcd_panel_t *panel)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);

    if (nt35510->reset_gpio_num >= 0) {
        gpio_reset_pin(nt35510->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del nt35510 panel @%p", nt35510);
    free(nt35510);
    return ESP_OK;
}

static esp_err_t panel_nt35510_reset(esp_lcd_panel_t *panel)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;

    // perform hardware reset
    if (nt35510->reset_gpio_num >= 0) {
        gpio_set_level(nt35510->reset_gpio_num, nt35510->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(nt35510->reset_gpio_num, !nt35510->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
    } else {
        // perform software reset
        ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, LCD_CMD_SWRESET << 8, NULL, 0), TAG,
                            "io tx param failed");
        vTaskDelay(pdMS_TO_TICKS(20)); // spec, wait at least 5m before sending new command
    }

    return ESP_OK;
}

static esp_err_t panel_nt35510_init(esp_lcd_panel_t *panel)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;
    // LCD goes into sleep mode and display will be turned off after power on reset, exit sleep mode first
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, LCD_CMD_SLPOUT << 8, NULL, 0), TAG,
                        "io tx param failed");;
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL << 8, (uint16_t[]) {
        nt35510->madctl_val,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, LCD_CMD_COLMOD << 8, (uint16_t[]) {
        nt35510->colmod_val,
    }, 2), TAG, "io tx param failed");

    return ESP_OK;
}

static esp_err_t panel_nt35510_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end,
                                           const void *color_data)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = nt35510->io;

    x_start += nt35510->x_gap;
    x_end += nt35510->x_gap;
    y_start += nt35510->y_gap;
    y_end += nt35510->y_gap;

    // define an area of frame memory where MCU can access
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_CASET << 8) + 0, (uint16_t[]) {
        (x_start >> 8) & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_CASET << 8) + 1, (uint16_t[]) {
        x_start & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_CASET << 8) + 2, (uint16_t[]) {
        ((x_end - 1) >> 8) & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_CASET << 8) + 3, (uint16_t[]) {
        (x_end - 1) & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_RASET << 8) + 0, (uint16_t[]) {
        (y_start >> 8) & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_RASET << 8) + 1, (uint16_t[]) {
        y_start & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_RASET << 8) + 2, (uint16_t[]) {
        ((y_end - 1) >> 8) & 0xFF,
    }, 2), TAG, "io tx param failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, (LCD_CMD_RASET << 8) + 3, (uint16_t[]) {
        (y_end - 1) & 0xFF,
    }, 2), TAG, "io tx param failed");
    // transfer frame buffer
    size_t len = (x_end - x_start) * (y_end - y_start) * nt35510->fb_bits_per_pixel / 8;
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_color(io, LCD_CMD_RAMWR << 8, color_data, len), TAG, "io tx color failed");

    return ESP_OK;
}

static esp_err_t panel_nt35510_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;
    int command = 0;
    if (invert_color_data) {
        command = LCD_CMD_INVON;
    } else {
        command = LCD_CMD_INVOFF;
    }
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, command << 8, NULL, 0), TAG,
                        "io tx param failed");
    return ESP_OK;
}

static esp_err_t panel_nt35510_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;
    if (mirror_x) {
        nt35510->madctl_val |= LCD_CMD_MX_BIT;
    } else {
        nt35510->madctl_val &= ~LCD_CMD_MX_BIT;
    }
    if (mirror_y) {
        nt35510->madctl_val |= LCD_CMD_MY_BIT;
    } else {
        nt35510->madctl_val &= ~LCD_CMD_MY_BIT;
    }
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL << 8, (uint16_t[]) {
        nt35510->madctl_val
    }, 2), TAG, "io tx param failed");
    return ESP_OK;
}

static esp_err_t panel_nt35510_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;
    if (swap_axes) {
        nt35510->madctl_val |= LCD_CMD_MV_BIT;
    } else {
        nt35510->madctl_val &= ~LCD_CMD_MV_BIT;
    }
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL << 8, (uint16_t[]) {
        nt35510->madctl_val
    }, 2), TAG, "io tx param failed");
    return ESP_OK;
}

static esp_err_t panel_nt35510_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    nt35510->x_gap = x_gap;
    nt35510->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_nt35510_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;
    int command = 0;
    if (on_off) {
        command = LCD_CMD_DISPON;
    } else {
        command = LCD_CMD_DISPOFF;
    }
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, command << 8, NULL, 0), TAG,
                        "io tx param failed");
    return ESP_OK;
}

static esp_err_t panel_nt35510_sleep(esp_lcd_panel_t *panel, bool sleep)
{
    nt35510_panel_t *nt35510 = __containerof(panel, nt35510_panel_t, base);
    esp_lcd_panel_io_handle_t io = nt35510->io;
    int command = 0;
    if (sleep) {
        command = LCD_CMD_SLPIN;
    } else {
        command = LCD_CMD_SLPOUT;
    }
    ESP_RETURN_ON_ERROR(esp_lcd_panel_io_tx_param(io, command, NULL, 0), TAG,
                        "io tx param failed");
    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
}
