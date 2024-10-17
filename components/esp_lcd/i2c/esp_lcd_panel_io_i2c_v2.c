/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "esp_heap_caps.h"
#include "esp_compiler.h"

static const char *TAG = "lcd_panel.io.i2c";

#define BYTESHIFT(VAR, IDX) (((VAR) >> ((IDX) * 8)) & 0xFF)
#define CONTROL_PHASE_LENGTH  (1)
#define CMD_LENGTH            (4)

static esp_err_t panel_io_i2c_del(esp_lcd_panel_io_t *io);
static esp_err_t panel_io_i2c_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size);
static esp_err_t panel_io_i2c_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_i2c_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_i2c_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx);

typedef struct {
    esp_lcd_panel_io_t base; // Base class of generic lcd panel io
    i2c_master_dev_handle_t i2c_handle; // I2C master driver handle.
    uint32_t dev_addr;       // Device address
    int lcd_cmd_bits;        // Bit width of LCD command
    int lcd_param_bits;      // Bit width of LCD parameter
    bool control_phase_enabled; // Is control phase enabled
    uint32_t control_phase_cmd;  // control byte when transferring command
    uint32_t control_phase_data; // control byte when transferring data
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // User register's callback, invoked when color data trans done
    void *user_ctx;             // User's private data, passed directly to callback on_color_trans_done()
} lcd_panel_io_i2c_t;

esp_err_t esp_lcd_new_panel_io_i2c_v2(i2c_master_bus_handle_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = NULL;
    i2c_master_dev_handle_t i2c_handle = NULL;
    ESP_GOTO_ON_FALSE(io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(io_config->control_phase_bytes * 8 > io_config->dc_bit_offset, ESP_ERR_INVALID_ARG, err, TAG, "D/C bit exceeds control bytes");
    // leak detection of i2c_panel_io because saving i2c_panel_io->base address
    ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-malloc-leak")
    i2c_panel_io = calloc(1, sizeof(lcd_panel_io_i2c_t));
    ESP_GOTO_ON_FALSE(i2c_panel_io, ESP_ERR_NO_MEM, err, TAG, "no mem for i2c panel io");

    i2c_device_config_t i2c_lcd_cfg = {
        .device_address = io_config->dev_addr,
        .scl_speed_hz = io_config->scl_speed_hz,
    };
    ESP_GOTO_ON_ERROR(i2c_master_bus_add_device(bus, &i2c_lcd_cfg, &i2c_handle), err, TAG, "i2c add device fail");

    i2c_panel_io->i2c_handle = i2c_handle;
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
    if (i2c_panel_io) {
        free(i2c_panel_io);
    }
    return ret;
    ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-malloc-leak")
}

static esp_err_t panel_io_i2c_del(esp_lcd_panel_io_t *io)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);

    ESP_LOGD(TAG, "del lcd panel io i2c @%p", i2c_panel_io);
    ESP_ERROR_CHECK(i2c_master_bus_rm_device(i2c_panel_io->i2c_handle));
    free(i2c_panel_io);
    return ret;
}

static esp_err_t panel_io_i2c_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx)
{
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);

    if (i2c_panel_io->on_color_trans_done != NULL) {
        ESP_LOGW(TAG, "Callback on_color_trans_done was already set and now it was overwritten!");
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

    if (send_param) {
        int write_size = 0;
        uint8_t write_buffer[CONTROL_PHASE_LENGTH + CMD_LENGTH] = {0};

        if (i2c_panel_io->control_phase_enabled) {
            write_buffer[0] = i2c_panel_io->control_phase_cmd;
            write_size += 1;

        }
        uint8_t cmds[4] = {BYTESHIFT(lcd_cmd, 3), BYTESHIFT(lcd_cmd, 2), BYTESHIFT(lcd_cmd, 1), BYTESHIFT(lcd_cmd, 0)};
        size_t cmds_size = i2c_panel_io->lcd_cmd_bits / 8;
        if (cmds_size > 0 && cmds_size <= sizeof(cmds)) {
            memcpy(write_buffer + write_size, cmds + (sizeof(cmds) - cmds_size), cmds_size);
            write_size += cmds_size;
        }

        ESP_GOTO_ON_ERROR(i2c_master_transmit_receive(i2c_panel_io->i2c_handle, write_buffer, write_size, buffer, buffer_size, -1), err, TAG, "i2c transaction failed");
    } else {
        ESP_GOTO_ON_ERROR(i2c_master_receive(i2c_panel_io->i2c_handle, buffer, buffer_size, -1), err, TAG, "i2c transaction failed");
    }

    return ESP_OK;
err:
    return ret;
}

static esp_err_t panel_io_i2c_tx_buffer(esp_lcd_panel_io_t *io, int lcd_cmd, const void *buffer, size_t buffer_size, bool is_param)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i2c_t *i2c_panel_io = __containerof(io, lcd_panel_io_i2c_t, base);
    bool send_param = (lcd_cmd >= 0);

    uint8_t control_phase_byte = 0;
    size_t control_phase_size = 0;
    if (i2c_panel_io->control_phase_enabled) {
        control_phase_byte = is_param ? i2c_panel_io->control_phase_cmd : i2c_panel_io->control_phase_data;
        control_phase_size = 1;
    }

    uint8_t *cmd_buffer = NULL;
    size_t cmd_buffer_size = 0;
    // some displays don't want any additional commands on data transfers
    uint8_t cmds[4] = {BYTESHIFT(lcd_cmd, 3), BYTESHIFT(lcd_cmd, 2), BYTESHIFT(lcd_cmd, 1), BYTESHIFT(lcd_cmd, 0)};
    if (send_param) {
        size_t cmds_size = i2c_panel_io->lcd_cmd_bits / 8;
        if (cmds_size > 0 && cmds_size <= sizeof(cmds)) {
            cmd_buffer = cmds + (sizeof(cmds) - cmds_size);
            cmd_buffer_size = cmds_size;
        }
    }

    uint8_t *lcd_buffer = NULL;
    size_t lcd_buffer_size = 0;
    if (buffer) {
        lcd_buffer = (uint8_t*)buffer;
        lcd_buffer_size = buffer_size;
    }

    i2c_master_transmit_multi_buffer_info_t lcd_i2c_buffer[3] = {
        {.write_buffer = &control_phase_byte, .buffer_size = control_phase_size},
        {.write_buffer = cmd_buffer, .buffer_size = cmd_buffer_size},
        {.write_buffer = lcd_buffer, .buffer_size = lcd_buffer_size},
    };

    ESP_GOTO_ON_ERROR(i2c_master_multi_buffer_transmit(i2c_panel_io->i2c_handle, lcd_i2c_buffer, sizeof(lcd_i2c_buffer) / sizeof(i2c_master_transmit_multi_buffer_info_t), -1), err, TAG, "i2c transaction failed");
    if (!is_param) {
        // trans done callback
        if (i2c_panel_io->on_color_trans_done) {
            i2c_panel_io->on_color_trans_done(&(i2c_panel_io->base), NULL, i2c_panel_io->user_ctx);
        }
    }

    return ESP_OK;
err:
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
