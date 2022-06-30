/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

static const char *TAG = "lcd_panel.ili9341";

static esp_err_t panel_ili9341_del(esp_lcd_panel_t *panel);
static esp_err_t panel_ili9341_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_ili9341_init(esp_lcd_panel_t *panel);
static esp_err_t panel_ili9341_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_ili9341_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_ili9341_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_ili9341_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_ili9341_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_ili9341_disp_on_off(esp_lcd_panel_t *panel, bool off);

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    int x_gap;
    int y_gap;
    unsigned int bits_per_pixel;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    uint8_t colmod_cal; // save surrent value of LCD_CMD_COLMOD register
} ili9341_panel_t;

esp_err_t esp_lcd_new_panel_ili9341(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    ili9341_panel_t *ili9341 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ili9341 = calloc(1, sizeof(ili9341_panel_t));
    ESP_GOTO_ON_FALSE(ili9341, ESP_ERR_NO_MEM, err, TAG, "no mem for ili9341 panel");

    if (panel_dev_config->reset_gpio_num >= 0) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }

    switch (panel_dev_config->color_space) {
    case ESP_LCD_COLOR_SPACE_RGB:
        ili9341->madctl_val = 0;
        break;
    case ESP_LCD_COLOR_SPACE_BGR:
        ili9341->madctl_val |= LCD_CMD_BGR_BIT;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported color space");
        break;
    }

    switch (panel_dev_config->bits_per_pixel) {
    case 16:
        ili9341->colmod_cal = 0x55;
        break;
    case 18:
        ili9341->colmod_cal = 0x66;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported pixel width");
        break;
    }

    ili9341->io = io;
    ili9341->bits_per_pixel = panel_dev_config->bits_per_pixel;
    ili9341->reset_gpio_num = panel_dev_config->reset_gpio_num;
    ili9341->reset_level = panel_dev_config->flags.reset_active_high;
    ili9341->base.del = panel_ili9341_del;
    ili9341->base.reset = panel_ili9341_reset;
    ili9341->base.init = panel_ili9341_init;
    ili9341->base.draw_bitmap = panel_ili9341_draw_bitmap;
    ili9341->base.invert_color = panel_ili9341_invert_color;
    ili9341->base.set_gap = panel_ili9341_set_gap;
    ili9341->base.mirror = panel_ili9341_mirror;
    ili9341->base.swap_xy = panel_ili9341_swap_xy;
    ili9341->base.disp_off = panel_ili9341_disp_on_off;
    *ret_panel = &(ili9341->base);
    ESP_LOGD(TAG, "new ili9341 panel @%p", ili9341);

    return ESP_OK;

err:
    if (ili9341) {
        if (panel_dev_config->reset_gpio_num >= 0) {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(ili9341);
    }
    return ret;
}

static esp_err_t panel_ili9341_del(esp_lcd_panel_t *panel)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);

    if (ili9341->reset_gpio_num >= 0) {
        gpio_reset_pin(ili9341->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del ili9341 panel @%p", ili9341);
    free(ili9341);
    return ESP_OK;
}

static esp_err_t panel_ili9341_reset(esp_lcd_panel_t *panel)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9341->io;

    // perform hardware reset
    if (ili9341->reset_gpio_num >= 0) {
        gpio_set_level(ili9341->reset_gpio_num, ili9341->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(ili9341->reset_gpio_num, !ili9341->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
    } else { // perform software reset
        esp_lcd_panel_io_tx_param(io, LCD_CMD_SWRESET, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(20)); // spec, wait at least 5m before sending new command
    }

    return ESP_OK;
}

#define ILI9341_GAMMASET 0x26 // Gamma Set
#define ILI9341_VSCRSADD 0x37 // Vertical Scrolling Start Address
#define ILI9341_FRMCTR1  0xB1 // Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_DFUNCTR  0xB6 // Display Function Control
#define ILI9341_PWCTR1   0xC0 // Power Control 1
#define ILI9341_PWCTR2   0xC1 // Power Control 2
#define ILI9341_VMCTR1   0xC5 // VCOM Control 1
#define ILI9341_VMCTR2   0xC7 // VCOM Control 2
#define ILI9341_GMCTRP1  0xE0 // Positive Gamma Correction
#define ILI9341_GMCTRN1  0xE1 // Negative Gamma Correction

static esp_err_t panel_ili9341_init(esp_lcd_panel_t *panel)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9341->io;
    esp_lcd_panel_io_tx_param(io, LCD_CMD_SLPOUT, NULL, 0);                               // Exit Sleep
    vTaskDelay(pdMS_TO_TICKS(150));
    esp_lcd_panel_io_tx_param(io, 0xEF, (uint8_t[]) { 0x03, 0x80, 0x02 }, 3);
    esp_lcd_panel_io_tx_param(io, 0xCF, (uint8_t[]) { 0x00, 0xC1, 0x30 }, 3);
    esp_lcd_panel_io_tx_param(io, 0xED, (uint8_t[]) { 0x64, 0x03, 0x12, 0x81 }, 4);
    esp_lcd_panel_io_tx_param(io, 0xE8, (uint8_t[]) { 0x85, 0x00, 0x78 }, 3);
    esp_lcd_panel_io_tx_param(io, 0xCB, (uint8_t[]) { 0x39, 0x2C, 0x00, 0x34, 0x02 }, 5);
    esp_lcd_panel_io_tx_param(io, 0xF7, (uint8_t[]) { 0x20 }, 1);
    esp_lcd_panel_io_tx_param(io, 0xEA, (uint8_t[]) { 0x00, 0x00 }, 2);
    esp_lcd_panel_io_tx_param(io, ILI9341_PWCTR1  , (uint8_t[]) { 0x23 }, 1);             // Power control VRH[5:0]
    esp_lcd_panel_io_tx_param(io, ILI9341_PWCTR2  , (uint8_t[]) { 0x10 }, 1);             // Power control SAP[2:0];BT[3:0]
    esp_lcd_panel_io_tx_param(io, ILI9341_VMCTR1  , (uint8_t[]) { 0x3e, 0x28 }, 2);       // VCM control
    esp_lcd_panel_io_tx_param(io, ILI9341_VMCTR2  , (uint8_t[]) { 0x86 }, 1);             // VCM control2
    esp_lcd_panel_io_tx_param(io, ILI9341_VSCRSADD, (uint8_t[]) { 0x00 }, 1);             // Vertical scroll zero
    esp_lcd_panel_io_tx_param(io, ILI9341_FRMCTR1 , (uint8_t[]) { 0x00, 0x18 }, 2);
    esp_lcd_panel_io_tx_param(io, ILI9341_DFUNCTR , (uint8_t[]) { 0x08, 0x82, 0x27 }, 3); // Display Function Control
    esp_lcd_panel_io_tx_param(io, 0xF2, (uint8_t[]) { 0x00 }, 1);                         // 3Gamma Function Disable
    esp_lcd_panel_io_tx_param(io, ILI9341_GAMMASET , (uint8_t[]) { 0x01 }, 1);            // Gamma curve selected
    esp_lcd_panel_io_tx_param(io, ILI9341_GMCTRP1 , (uint8_t[]) {                         // Set Gamma
      0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
      0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 }, 15);
    esp_lcd_panel_io_tx_param(io, ILI9341_GMCTRN1 , (uint8_t[]) {                         // Set Gamma
      0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
      0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F }, 15);
    esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL, (uint8_t[]) {
        ili9341->madctl_val,
    }, 1);
    esp_lcd_panel_io_tx_param(io, LCD_CMD_COLMOD, (uint8_t[]) {
        ili9341->colmod_cal,
    }, 1);
    esp_lcd_panel_io_tx_param(io, LCD_CMD_DISPON, NULL, 0);                               // Display on
    vTaskDelay(pdMS_TO_TICKS(150));

    return ESP_OK;
}

static esp_err_t panel_ili9341_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = ili9341->io;

    x_start += ili9341->x_gap;
    x_end += ili9341->x_gap;
    y_start += ili9341->y_gap;
    y_end += ili9341->y_gap;

    // define an area of frame memory where MCU can access
    esp_lcd_panel_io_tx_param(io, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    // transfer frame buffer
    size_t len = (x_end - x_start) * (y_end - y_start) * ili9341->bits_per_pixel / 8;
    esp_lcd_panel_io_tx_color(io, LCD_CMD_RAMWR, color_data, len);

    return ESP_OK;
}

static esp_err_t panel_ili9341_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9341->io;
    int command = 0;
    if (invert_color_data) {
        command = LCD_CMD_INVON;
    } else {
        command = LCD_CMD_INVOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_ili9341_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9341->io;
    if (mirror_x) {
        ili9341->madctl_val |= LCD_CMD_MX_BIT;
    } else {
        ili9341->madctl_val &= ~LCD_CMD_MX_BIT;
    }
    if (mirror_y) {
        ili9341->madctl_val |= LCD_CMD_MY_BIT;
    } else {
        ili9341->madctl_val &= ~LCD_CMD_MY_BIT;
    }
    esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL, (uint8_t[]) {
        ili9341->madctl_val
    }, 1);
    return ESP_OK;
}

static esp_err_t panel_ili9341_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9341->io;
    if (swap_axes) {
        ili9341->madctl_val |= LCD_CMD_MV_BIT;
    } else {
        ili9341->madctl_val &= ~LCD_CMD_MV_BIT;
    }
    esp_lcd_panel_io_tx_param(io, LCD_CMD_MADCTL, (uint8_t[]) {
        ili9341->madctl_val
    }, 1);
    return ESP_OK;
}

static esp_err_t panel_ili9341_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    ili9341->x_gap = x_gap;
    ili9341->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_ili9341_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    ili9341_panel_t *ili9341 = __containerof(panel, ili9341_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili9341->io;
    int command = 0;
    if (on_off) {
        command = LCD_CMD_DISPON;
    } else {
        command = LCD_CMD_DISPOFF;
    }
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}
