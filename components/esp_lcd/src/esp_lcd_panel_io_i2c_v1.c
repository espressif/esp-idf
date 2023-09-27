/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "lcd_panel.io.i2c";

#define CMD_HANDLER_BUFFER_SIZE I2C_LINK_RECOMMENDED_SIZE(2)    // only 2 operations will be queued in the handler ATTOW
#define BYTESHIFT(VAR, IDX) (((VAR) >> ((IDX) * 8)) & 0xFF)

static esp_err_t panel_io_i2c_del(esp_lcd_panel_io_t *io);
static esp_err_t panel_io_i2c_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size);
static esp_err_t panel_io_i2c_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_i2c_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_i2c_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx);

typedef struct {
    esp_lcd_panel_io_t base; // Base class of generic lcd panel io
    uint32_t i2c_bus_id;     // I2C bus id, indicating which I2C port
    uint32_t dev_addr;       // Device address
    int lcd_cmd_bits;        // Bit width of LCD command
    int lcd_param_bits;      // Bit width of LCD parameter
    bool control_phase_enabled; // Is control phase enabled
    uint32_t control_phase_cmd;  // control byte when transferring command
    uint32_t control_phase_data; // control byte when transferring data
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // User register's callback, invoked when color data trans done
    void *user_ctx;             // User's private data, passed directly to callback on_color_trans_done()
    uint8_t cmdlink_buffer[];   // pre-alloc I2C command link buffer, to be reused in all transactions
} lcd_panel_io_i2c_t;

esp_err_t esp_lcd_new_panel_io_i2c_v1(uint32_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = NULL;
    ESP_GOTO_ON_FALSE(io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(io_config->control_phase_bytes * 8 > io_config->dc_bit_offset, ESP_ERR_INVALID_ARG, err, TAG, "D/C bit exceeds control bytes");
    ESP_GOTO_ON_FALSE(io_config->scl_speed_hz == 0, ESP_ERR_INVALID_ARG, err, TAG, "scl_speed_hz is not need to set in legacy i2c_lcd driver");
    i2c_panel_io = calloc(1, sizeof(lcd_panel_io_i2c_t) + CMD_HANDLER_BUFFER_SIZE); // expand zero-length array cmdlink_buffer
    ESP_GOTO_ON_FALSE(i2c_panel_io, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c panel io");

    i2c_panel_io->i2c_bus_id = bus;
    i2c_panel_io->lcd_cmd_bits = io_config->lcd_cmd_bits;
    i2c_panel_io->lcd_param_bits = io_config->lcd_param_bits;
    i2c_panel_io->on_color_trans_done = io_config->on_color_trans_done;
    i2c_panel_io->user_ctx = io_config->user_ctx;
    i2c_panel_io->control_phase_enabled = (!io_config->flags.disable_control_phase);
    i2c_panel_io->control_phase_data = (!io_config->flags.dc_low_on_data) << (io_config->dc_bit_offset);
    i2c_panel_io->control_phase_cmd = (io_config->flags.dc_low_on_data) << (io_config->dc_bit_offset);
    i2c_panel_io->dev_addr = io_config->dev_addr;
    i2c_panel_io->base.del = panel_io_i2c_del;
    i2c_panel_io->base.rx_param = panel_io_i2c_rx_param;
    i2c_panel_io->base.tx_param = panel_io_i2c_tx_param;
    i2c_panel_io->base.tx_color = panel_io_i2c_tx_color;
    i2c_panel_io->base.register_event_callbacks = panel_io_i2c_register_event_callbacks;
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

    ESP_LOGD(TAG, "del lcd panel i2c @%p", i2c_panel_io);
    free(i2c_panel_io);
    return ret;
}

static esp_err_t panel_io_i2c_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx)
{
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);

    if (i2c_panel_io->on_color_trans_done != NULL) {
        ESP_LOGW(TAG, "Callback on_color_trans_done was already set and now it was owerwritten!");
    }

    i2c_panel_io->on_color_trans_done = cbs->on_color_trans_done;
    i2c_panel_io->user_ctx = user_ctx;

    return ESP_OK;
}

static esp_err_t panel_io_i2c_rx_buffer(esp_lcd_panel_io_t *io, int lcd_cmd, void *buffer, size_t buffer_size)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);
    bool send_param = (lcd_cmd >= 0);

    i2c_cmd_handle_t cmd_link = i2c_cmd_link_create_static(i2c_panel_io->cmdlink_buffer, CMD_HANDLER_BUFFER_SIZE);
    ESP_GOTO_ON_FALSE(cmd_link, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c cmd link");
    ESP_GOTO_ON_ERROR(i2c_master_start(cmd_link), err, TAG, "issue start failed"); // start phase
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (i2c_panel_io->dev_addr << 1) | I2C_MASTER_WRITE, true), err, TAG, "write address failed"); // address phase
    if (send_param) {
        if (i2c_panel_io->control_phase_enabled) {
            ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, i2c_panel_io->control_phase_cmd, true),
                              err, TAG, "write control phase failed"); // control phase
        }
        uint8_t cmds[4] = {BYTESHIFT(lcd_cmd, 3), BYTESHIFT(lcd_cmd, 2), BYTESHIFT(lcd_cmd, 1), BYTESHIFT(lcd_cmd, 0)};
        size_t cmds_size = i2c_panel_io->lcd_cmd_bits / 8;
        if (cmds_size > 0 && cmds_size <= sizeof(cmds)) {
            ESP_GOTO_ON_ERROR(i2c_master_write(cmd_link, cmds + (sizeof(cmds) - cmds_size), cmds_size, true), err, TAG, "write LCD cmd failed");
        }
    }

    if (buffer) {
        ESP_GOTO_ON_ERROR(i2c_master_start(cmd_link), err, TAG, "issue start failed"); // start phase
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (i2c_panel_io->dev_addr << 1) | I2C_MASTER_READ, true), err, TAG, "write address failed"); // address phase
        ESP_GOTO_ON_ERROR(i2c_master_read(cmd_link, buffer, buffer_size, I2C_MASTER_LAST_NACK), err, TAG, "read data failed");
    }

    ESP_GOTO_ON_ERROR(i2c_master_stop(cmd_link), err, TAG, "issue stop failed"); // stop phase
    ESP_GOTO_ON_ERROR(i2c_master_cmd_begin(i2c_panel_io->i2c_bus_id, cmd_link, portMAX_DELAY), err, TAG, "i2c transaction failed");

    i2c_cmd_link_delete_static(cmd_link);

    return ESP_OK;
err:
    if (cmd_link) {
        i2c_cmd_link_delete_static(cmd_link);
    }
    return ret;
}

static esp_err_t panel_io_i2c_tx_buffer(esp_lcd_panel_io_t *io, int lcd_cmd, const void *buffer, size_t buffer_size, bool is_param)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);
    bool send_param = (lcd_cmd >= 0);

    i2c_cmd_handle_t cmd_link = i2c_cmd_link_create_static(i2c_panel_io->cmdlink_buffer, CMD_HANDLER_BUFFER_SIZE);
    ESP_GOTO_ON_FALSE(cmd_link, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c cmd link");
    ESP_GOTO_ON_ERROR(i2c_master_start(cmd_link), err, TAG, "issue start failed"); // start phase
    ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, (i2c_panel_io->dev_addr << 1) | I2C_MASTER_WRITE, true), err, TAG, "write address failed"); // address phase
    if (i2c_panel_io->control_phase_enabled) {
        ESP_GOTO_ON_ERROR(i2c_master_write_byte(cmd_link, is_param ? i2c_panel_io->control_phase_cmd : i2c_panel_io->control_phase_data, true),
                          err, TAG, "write control phase failed"); // control phase
    }

    // some displays don't want any additional commands on data transfers
    if (send_param) {
        uint8_t cmds[4] = {BYTESHIFT(lcd_cmd, 3), BYTESHIFT(lcd_cmd, 2), BYTESHIFT(lcd_cmd, 1), BYTESHIFT(lcd_cmd, 0)};
        size_t cmds_size = i2c_panel_io->lcd_cmd_bits / 8;
        if (cmds_size > 0 && cmds_size <= sizeof(cmds)) {
            ESP_GOTO_ON_ERROR(i2c_master_write(cmd_link, cmds + (sizeof(cmds) - cmds_size), cmds_size, true), err, TAG, "write LCD cmd failed");
        }
    }

    if (buffer) {
        ESP_GOTO_ON_ERROR(i2c_master_write(cmd_link, buffer, buffer_size, true), err, TAG, "write data failed");
    }

    ESP_GOTO_ON_ERROR(i2c_master_stop(cmd_link), err, TAG, "issue stop failed"); // stop phase
    ESP_GOTO_ON_ERROR(i2c_master_cmd_begin(i2c_panel_io->i2c_bus_id, cmd_link, portMAX_DELAY), err, TAG, "i2c transaction failed");
    i2c_cmd_link_delete_static(cmd_link);

    if (!is_param) {
        // trans done callback
        if (i2c_panel_io->on_color_trans_done) {
            i2c_panel_io->on_color_trans_done(&(i2c_panel_io->base), NULL, i2c_panel_io->user_ctx);
        }
    }

    return ESP_OK;
err:
    if (cmd_link) {
        i2c_cmd_link_delete_static(cmd_link);
    }
    return ret;
}

static esp_err_t panel_io_i2c_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size)
{
    return panel_io_i2c_rx_buffer(io, lcd_cmd, param, param_size);
}

static esp_err_t panel_io_i2c_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size)
{
    return panel_io_i2c_tx_buffer(io, lcd_cmd, param, param_size, true);
}

static esp_err_t panel_io_i2c_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size)
{
    return panel_io_i2c_tx_buffer(io, lcd_cmd, color, color_size, false);
}
