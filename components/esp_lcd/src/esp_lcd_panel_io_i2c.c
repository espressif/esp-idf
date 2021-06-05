/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "lcd_panel.io.i2c";

static esp_err_t panel_io_i2c_del(esp_lcd_panel_io_t *io);
static esp_err_t panel_io_i2c_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *param, size_t param_size);
static esp_err_t panel_io_i2c_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *color, size_t color_size);

typedef struct {
    esp_lcd_panel_io_t base; // Base class of generic lcd panel io
    uint32_t i2c_bus_id;     // I2C bus id, indicating which I2C port
    uint32_t dev_addr;       // Device address
    uint32_t control_phase_cmd;  // control byte when transferring command
    uint32_t control_phase_data; // control byte when transferring data
    bool (*on_color_trans_done)(esp_lcd_panel_io_handle_t panel_io, void *user_data, void *event_data); // User register's callback, invoked when color data trans done
    void *user_data;             // User's private data, passed directly to callback on_color_trans_done()
} lcd_panel_io_i2c_t;

esp_err_t esp_lcd_new_panel_io_i2c(esp_lcd_i2c_bus_handle_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = NULL;
    ESP_GOTO_ON_FALSE(io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(io_config->control_phase_bytes * 8 > io_config->dc_bit_offset, ESP_ERR_INVALID_ARG, err, TAG, "D/C bit exceeds control bytes");
    i2c_panel_io = calloc(1, sizeof(lcd_panel_io_i2c_t));
    ESP_GOTO_ON_FALSE(i2c_panel_io, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c panel io");

    i2c_panel_io->i2c_bus_id = (uint32_t)bus;
    i2c_panel_io->on_color_trans_done = io_config->on_color_trans_done;
    i2c_panel_io->control_phase_data = (!io_config->flags.dc_low_on_data) << (io_config->dc_bit_offset);
    i2c_panel_io->control_phase_cmd = (io_config->flags.dc_low_on_data) << (io_config->dc_bit_offset);
    i2c_panel_io->user_data = io_config->user_data;
    i2c_panel_io->dev_addr = io_config->dev_addr;
    i2c_panel_io->base.del = panel_io_i2c_del;
    i2c_panel_io->base.tx_param = panel_io_i2c_tx_param;
    i2c_panel_io->base.tx_color = panel_io_i2c_tx_color;
    *ret_io = &(i2c_panel_io->base);
    ESP_LOGD(TAG, "new i2c lcd panel io @%p", i2c_panel_io);

    return ESP_OK;
err:
    return ret;
}

static esp_err_t panel_io_i2c_del(esp_lcd_panel_io_t *io)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);

    ESP_LOGD(TAG, "del lcd panel io spi @%p", i2c_panel_io);
    free(i2c_panel_io);
    return ret;
}

static esp_err_t panel_io_i2c_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *param, size_t param_size)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);

    i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
    ESP_GOTO_ON_FALSE(cmd_link, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c cmd link");
    ESP_GOTO_ON_ERROR(i2c_master_start(cmd_link), err, TAG, "issue start failed"); // start phase
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (i2c_panel_io->dev_addr << 1) | I2C_MASTER_WRITE, true), err, TAG, "write address failed"); // address phase
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, i2c_panel_io->control_phase_cmd, true), err, TAG, "write control command failed"); // control phase
    switch (lcd_cmd_bits / 8) { // LCD command
    case 4:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 24) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    case 3:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 16) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    case 2:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 8) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    case 1:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 0) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    default:
        break;
    }

    if (param) {
        uint8_t *data = (uint8_t *) param; // parameters for that command
        ESP_GOTO_ON_ERROR(i2c_master_write(cmd_link, data, param_size, true), err, TAG, "write param failed");
    }
    ESP_GOTO_ON_ERROR(i2c_master_stop(cmd_link), err, TAG, "issue stop failed"); // stop phase

    ESP_GOTO_ON_ERROR(i2c_master_cmd_begin(i2c_panel_io->i2c_bus_id, cmd_link, portMAX_DELAY), err, TAG, "i2c transaction failed");
    i2c_cmd_link_delete(cmd_link);

    return ESP_OK;
err:
    if (cmd_link) {
        i2c_cmd_link_delete(cmd_link);
    }
    return ret;
}

static esp_err_t panel_io_i2c_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *color, size_t color_size)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);

    i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
    ESP_GOTO_ON_FALSE(cmd_link, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c cmd link");
    ESP_GOTO_ON_ERROR(i2c_master_start(cmd_link), err, TAG, "issue start failed"); // start phase
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (i2c_panel_io->dev_addr << 1) | I2C_MASTER_WRITE, true), err, TAG, "write address failed"); // address phase
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, i2c_panel_io->control_phase_data, true), err, TAG, "write control data failed"); // control phase
    switch (lcd_cmd_bits / 8) { // LCD command
    case 4:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 24) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    case 3:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 16) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    case 2:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 8) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    case 1:
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (lcd_cmd >> 0) & 0xFF, true), err, TAG, "write LCD cmd failed"); // fall-through
    default:
        break;
    }
    ESP_GOTO_ON_ERROR(i2c_master_write(cmd_link, color, color_size, true), err, TAG, "write color failed"); // LCD gram data
    ESP_GOTO_ON_ERROR(i2c_master_stop(cmd_link), err, TAG, "issue stop failed"); // stop phase

    ESP_GOTO_ON_ERROR(i2c_master_cmd_begin(i2c_panel_io->i2c_bus_id, cmd_link, portMAX_DELAY), err, TAG, "i2c transaction failed");
    i2c_cmd_link_delete(cmd_link);
    // trans done callback
    if (i2c_panel_io->on_color_trans_done) {
        i2c_panel_io->on_color_trans_done(&(i2c_panel_io->base), i2c_panel_io->user_data, NULL);
    }

    return ESP_OK;
err:
    if (cmd_link) {
        i2c_cmd_link_delete(cmd_link);
    }
    return ret;
}
