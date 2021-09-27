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
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_common.h"

static const char *TAG = "lcd_panel.io.spi";

static esp_err_t panel_io_spi_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_spi_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_spi_del(esp_lcd_panel_io_t *io);
static void lcd_spi_pre_trans_cb(spi_transaction_t *trans);
static void lcd_spi_post_trans_color_cb(spi_transaction_t *trans);

typedef struct {
    spi_transaction_t base;
    struct {
        unsigned int dc_gpio_level: 1;
        unsigned int trans_is_color: 1;
    } flags;
} lcd_spi_trans_descriptor_t;

typedef struct {
    esp_lcd_panel_io_t base;     // Base class of generic lcd panel io
    spi_device_handle_t spi_dev; // SPI device handle
    int dc_gpio_num;             // D/C line GPIO number
    bool (*on_color_trans_done)(esp_lcd_panel_io_handle_t panel_io, void *user_data, void *event_data); // User register's callback, invoked when color data trans done
    void *user_data;           // User's private data, passed directly to callback on_color_trans_done
    size_t queue_size;         // Size of transaction queue
    size_t num_trans_inflight;  // Number of transactions that are undergoing (the descriptor not recycled yet)
    int lcd_cmd_bits;          // Bit width of LCD command
    int lcd_param_bits;        // Bit width of LCD parameter
    struct {
        unsigned int dc_as_cmd_phase: 1; // D/C line value is encoded into SPI transaction command phase
        unsigned int dc_data_level: 1;   // Indicates the level of DC line when tranfering data
        unsigned int octal_mode: 1;      // Indicates whether the transmitting is enabled with octal mode (8 data lines)
    } flags;
    lcd_spi_trans_descriptor_t trans_pool[]; // Transaction pool
} esp_lcd_panel_io_spi_t;

esp_err_t esp_lcd_new_panel_io_spi(esp_lcd_spi_bus_handle_t bus, const esp_lcd_panel_io_spi_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_spi_t *spi_panel_io = NULL;
    ESP_GOTO_ON_FALSE(bus && io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(!(io_config->flags.dc_as_cmd_phase && io_config->dc_gpio_num >= 0),
                      ESP_ERR_INVALID_ARG, err, TAG, "invalid DC mode");
    spi_panel_io = calloc(1, sizeof(esp_lcd_panel_io_spi_t) + sizeof(lcd_spi_trans_descriptor_t) * io_config->trans_queue_depth);
    ESP_GOTO_ON_FALSE(spi_panel_io, ESP_ERR_NO_MEM, err, TAG, "no mem for spi panel io");

    spi_device_interface_config_t devcfg = {
        .flags = SPI_DEVICE_HALFDUPLEX,
        .clock_speed_hz = io_config->pclk_hz,
        .mode = io_config->spi_mode,
        .spics_io_num = io_config->cs_gpio_num,
        .queue_size = io_config->trans_queue_depth,
        .command_bits = io_config->flags.dc_as_cmd_phase ? 1 : 0, // whether to encode DC line into command transaction
        .pre_cb = lcd_spi_pre_trans_cb, // pre-transaction callback, mainly control DC gpio level
        .post_cb = io_config->on_color_trans_done ? lcd_spi_post_trans_color_cb : NULL, // post-transaction, where we invoke user registered "on_color_trans_done()"
    };
    ret = spi_bus_add_device((spi_host_device_t)bus, &devcfg, &spi_panel_io->spi_dev);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "adding spi device to bus failed");

    // if the DC line is not encoded into any spi transaction phase or it's not controlled by SPI peripheral
    if (io_config->dc_gpio_num >= 0) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << io_config->dc_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for D/C line failed");
    }

    spi_panel_io->flags.dc_as_cmd_phase = io_config->flags.dc_as_cmd_phase;
    spi_panel_io->flags.dc_data_level = !io_config->flags.dc_low_on_data;
    spi_panel_io->flags.octal_mode = io_config->flags.octal_mode;
    spi_panel_io->on_color_trans_done = io_config->on_color_trans_done;
    spi_panel_io->lcd_cmd_bits = io_config->lcd_cmd_bits;
    spi_panel_io->lcd_param_bits = io_config->lcd_param_bits;
    spi_panel_io->user_data = io_config->user_data;
    spi_panel_io->dc_gpio_num = io_config->dc_gpio_num;
    spi_panel_io->queue_size = io_config->trans_queue_depth;
    spi_panel_io->base.tx_param = panel_io_spi_tx_param;
    spi_panel_io->base.tx_color = panel_io_spi_tx_color;
    spi_panel_io->base.del = panel_io_spi_del;
    *ret_io = &(spi_panel_io->base);
    ESP_LOGD(TAG, "new spi lcd panel io @%p", spi_panel_io);

    return ESP_OK;

err:
    if (spi_panel_io) {
        if (io_config->dc_gpio_num >= 0) {
            gpio_reset_pin(io_config->dc_gpio_num);
        }
        free(spi_panel_io);
    }
    return ret;
}

static esp_err_t panel_io_spi_del(esp_lcd_panel_io_t *io)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t *spi_trans = NULL;
    esp_lcd_panel_io_spi_t *spi_panel_io = __containerof(io, esp_lcd_panel_io_spi_t, base);

    // wait all pending transaction to finish
    for (size_t i = 0; i < spi_panel_io->num_trans_inflight; i++) {
        ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
    }
    spi_bus_remove_device(spi_panel_io->spi_dev);
    if (spi_panel_io->dc_gpio_num >= 0) {
        gpio_reset_pin(spi_panel_io->dc_gpio_num);
    }
    ESP_LOGD(TAG, "del lcd panel io spi @%p", spi_panel_io);
    free(spi_panel_io);

err:
    return ret;
}

static void spi_lcd_prepare_cmd_buffer(esp_lcd_panel_io_spi_t *panel_io, const void *cmd)
{
    uint8_t *from = (uint8_t *)cmd;
    // LCD is big-endian, e.g. to send command 0x1234, byte 0x12 should appear on the bus first
    // However, the SPI peripheral will send 0x34 first, so we reversed the order below
    if (panel_io->lcd_cmd_bits > 8) {
        int start = 0;
        int end = panel_io->lcd_cmd_bits / 8 - 1;
        lcd_com_reverse_buffer_bytes(from, start, end);
    }
}

static void spi_lcd_prepare_param_buffer(esp_lcd_panel_io_spi_t *panel_io, const void *param, size_t param_size)
{
    uint8_t *from = (uint8_t *)param;
    int param_width = panel_io->lcd_param_bits / 8;
    size_t param_num = param_size / param_width;
    // LCD is big-endian, e.g. to send command 0x1234, byte 0x12 should appear on the bus first
    // However, the SPI peripheral will send 0x34 first, so we reversed the order below
    if (panel_io->lcd_param_bits > 8) {
        for (size_t i = 0; i < param_num; i++) {
            int start = i * param_width;
            int end = start + param_width - 1;
            lcd_com_reverse_buffer_bytes(from, start, end);
        }
    }
}

static esp_err_t panel_io_spi_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t *spi_trans = NULL;
    lcd_spi_trans_descriptor_t *lcd_trans = NULL;
    esp_lcd_panel_io_spi_t *spi_panel_io = __containerof(io, esp_lcd_panel_io_spi_t, base);

    // before issue a polling transaction, need to wait queued transactions finished
    for (size_t i = 0; i < spi_panel_io->num_trans_inflight; i++) {
        ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
    }
    spi_panel_io->num_trans_inflight = 0;
    lcd_trans = &spi_panel_io->trans_pool[0];
    memset(lcd_trans, 0, sizeof(lcd_spi_trans_descriptor_t));
    spi_lcd_prepare_cmd_buffer(spi_panel_io, &lcd_cmd);
    lcd_trans->base.user = spi_panel_io;
    lcd_trans->flags.dc_gpio_level = !spi_panel_io->flags.dc_data_level; // set D/C line to command mode
    lcd_trans->base.length = spi_panel_io->lcd_cmd_bits;
    lcd_trans->base.tx_buffer = &lcd_cmd;
    if (spi_panel_io->flags.octal_mode) {
        // use 8 lines for transmitting command, address and data
        lcd_trans->base.flags |= (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR | SPI_TRANS_MODE_OCT);
    }
    if (spi_panel_io->flags.dc_as_cmd_phase) { // encoding DC value to SPI command phase when necessary
        lcd_trans->base.cmd = !spi_panel_io->flags.dc_data_level;
    }
    // command is short, using polling mode
    ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) command failed");

    if (param && param_size) {
        spi_lcd_prepare_param_buffer(spi_panel_io, param, param_size);
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_data_level; // set D/C line to data mode
        lcd_trans->base.length = param_size * 8; // transaction length is in bits
        lcd_trans->base.tx_buffer = param;
        if (spi_panel_io->flags.dc_as_cmd_phase) { // encoding DC value to SPI command phase when necessary
            lcd_trans->base.cmd = spi_panel_io->flags.dc_data_level;
        }
        // parameter is usually short, using polling mode
        ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) param failed");
    }

err:
    return ret;
}

static esp_err_t panel_io_spi_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t *spi_trans = NULL;
    lcd_spi_trans_descriptor_t *lcd_trans = NULL;
    esp_lcd_panel_io_spi_t *spi_panel_io = __containerof(io, esp_lcd_panel_io_spi_t, base);

    // before issue a polling transaction, need to wait queued transactions finished
    for (size_t i = 0; i < spi_panel_io->num_trans_inflight; i++) {
        ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
    }
    spi_panel_io->num_trans_inflight = 0;
    lcd_trans = &spi_panel_io->trans_pool[0];
    memset(lcd_trans, 0, sizeof(lcd_spi_trans_descriptor_t));
    spi_lcd_prepare_cmd_buffer(spi_panel_io, &lcd_cmd);
    lcd_trans->base.user = spi_panel_io;
    lcd_trans->flags.dc_gpio_level = !spi_panel_io->flags.dc_data_level; // set D/C line to command mode
    lcd_trans->base.length = spi_panel_io->lcd_cmd_bits;
    lcd_trans->base.tx_buffer = &lcd_cmd;
    if (spi_panel_io->flags.dc_as_cmd_phase) { // encoding DC value to SPI command phase when necessary
        lcd_trans->base.cmd = !spi_panel_io->flags.dc_data_level;
    }
    if (spi_panel_io->flags.octal_mode) {
        // use 8 lines for transmitting command, address and data
        lcd_trans->base.flags |= (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR | SPI_TRANS_MODE_OCT);
    }
    // command is short, using polling mode
    ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) command failed");

    // sending LCD color data
    lcd_trans->flags.trans_is_color = 1;
    lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_data_level; // set D/C line to data mode
    lcd_trans->base.length = color_size * 8; // transaction length is in bits
    lcd_trans->base.tx_buffer = color;
    if (spi_panel_io->flags.dc_as_cmd_phase) { // encoding DC value to SPI command phase when necessary
        lcd_trans->base.cmd = spi_panel_io->flags.dc_data_level;
    }
    // color data is usually large, using queue+blocking mode
    ret = spi_device_queue_trans(spi_panel_io->spi_dev, &lcd_trans->base, portMAX_DELAY);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (queue) color failed");
    spi_panel_io->num_trans_inflight++;

err:
    return ret;
}

static void lcd_spi_pre_trans_cb(spi_transaction_t *trans)
{
    esp_lcd_panel_io_spi_t *spi_panel_io = trans->user;
    lcd_spi_trans_descriptor_t *lcd_trans = __containerof(trans, lcd_spi_trans_descriptor_t, base);
    if (spi_panel_io->dc_gpio_num >= 0) { // set D/C line level if necessary
        gpio_set_level(spi_panel_io->dc_gpio_num, lcd_trans->flags.dc_gpio_level);
    }
}

static void lcd_spi_post_trans_color_cb(spi_transaction_t *trans)
{
    esp_lcd_panel_io_spi_t *spi_panel_io = trans->user;
    lcd_spi_trans_descriptor_t *lcd_trans = __containerof(trans, lcd_spi_trans_descriptor_t, base);
    if (lcd_trans->flags.trans_is_color) {
        if (spi_panel_io->on_color_trans_done) {
            spi_panel_io->on_color_trans_done(&spi_panel_io->base, spi_panel_io->user_data, NULL);
        }
    }
}
