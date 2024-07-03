/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_common.h"

static const char *TAG = "lcd_panel.io.spi";

static esp_err_t panel_io_spi_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size);
static esp_err_t panel_io_spi_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_spi_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_spi_del(esp_lcd_panel_io_t *io);
static void lcd_spi_pre_trans_cb(spi_transaction_t *trans);
static void lcd_spi_post_trans_color_cb(spi_transaction_t *trans);
static esp_err_t panel_io_spi_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx);

typedef struct {
    spi_transaction_t base;
    struct {
        unsigned int dc_gpio_level: 1;
        unsigned int en_trans_done_cb: 1;
    } flags;
} lcd_spi_trans_descriptor_t;

typedef struct {
    esp_lcd_panel_io_t base;     // Base class of generic lcd panel io
    spi_device_handle_t spi_dev; // SPI device handle
    size_t spi_trans_max_bytes;  // Maximum bytes that can be transmitted in one spi transaction
    int dc_gpio_num;             // D/C line GPIO number
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // User register's callback, invoked when color data trans done
    void *user_ctx;           // User's private data, passed directly to callback on_color_trans_done
    size_t queue_size;         // Size of transaction queue
    size_t num_trans_inflight;  // Number of transactions that are undergoing (the descriptor not recycled yet)
    int lcd_cmd_bits;          // Bit width of LCD command
    int lcd_param_bits;        // Bit width of LCD parameter
    uint8_t cs_ena_pretrans;        // Amount of SPI bit-cycles the cs should be activated before the transmission (0-16)
    uint8_t cs_ena_posttrans;       // Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
    struct {
        unsigned int dc_cmd_level: 1;    // Indicates the level of DC line when transferring command
        unsigned int dc_data_level: 1;   // Indicates the level of DC line when transferring color data
        unsigned int dc_param_level: 1;  // Indicates the level of DC line when transferring parameters
        unsigned int octal_mode: 1;      // Indicates whether the transmitting is enabled with octal mode (8 data lines)
        unsigned int quad_mode: 1;       // Indicates whether the transmitting is enabled with quad mode (4 data lines)
    } flags;
    lcd_spi_trans_descriptor_t trans_pool[]; // Transaction pool
} esp_lcd_panel_io_spi_t;

esp_err_t esp_lcd_new_panel_io_spi(esp_lcd_spi_bus_handle_t bus, const esp_lcd_panel_io_spi_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_spi_t *spi_panel_io = NULL;
    ESP_GOTO_ON_FALSE(bus && io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    spi_panel_io = calloc(1, sizeof(esp_lcd_panel_io_spi_t) + sizeof(lcd_spi_trans_descriptor_t) * io_config->trans_queue_depth);
    ESP_GOTO_ON_FALSE(spi_panel_io, ESP_ERR_NO_MEM, err, TAG, "no mem for spi panel io");

    spi_device_interface_config_t devcfg = {
        .flags = SPI_DEVICE_HALFDUPLEX |
        (io_config->flags.lsb_first ? SPI_DEVICE_TXBIT_LSBFIRST : 0) |
        (io_config->flags.sio_mode ? SPI_DEVICE_3WIRE : 0) |
        (io_config->flags.cs_high_active ? SPI_DEVICE_POSITIVE_CS : 0),
        .clock_speed_hz = io_config->pclk_hz,
        .mode = io_config->spi_mode,
        .spics_io_num = io_config->cs_gpio_num,
        .queue_size = io_config->trans_queue_depth,
        .pre_cb = lcd_spi_pre_trans_cb, // pre-transaction callback, mainly control DC gpio level
        .post_cb = lcd_spi_post_trans_color_cb, // post-transaction, where we invoke user registered "on_color_trans_done()"
        .cs_ena_pretrans = io_config->cs_ena_pretrans,
        .cs_ena_posttrans = io_config->cs_ena_posttrans,
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

    spi_panel_io->flags.dc_cmd_level = io_config->flags.dc_high_on_cmd;
    spi_panel_io->flags.dc_data_level = !io_config->flags.dc_low_on_data;
    spi_panel_io->flags.dc_param_level = !io_config->flags.dc_low_on_param;
    spi_panel_io->flags.octal_mode = io_config->flags.octal_mode;
    spi_panel_io->flags.quad_mode = io_config->flags.quad_mode;
    spi_panel_io->on_color_trans_done = io_config->on_color_trans_done;
    spi_panel_io->user_ctx = io_config->user_ctx;
    spi_panel_io->lcd_cmd_bits = io_config->lcd_cmd_bits;
    spi_panel_io->lcd_param_bits = io_config->lcd_param_bits;
    spi_panel_io->dc_gpio_num = io_config->dc_gpio_num;
    spi_panel_io->queue_size = io_config->trans_queue_depth;
    spi_panel_io->base.rx_param = panel_io_spi_rx_param;
    spi_panel_io->base.tx_param = panel_io_spi_tx_param;
    spi_panel_io->base.tx_color = panel_io_spi_tx_color;
    spi_panel_io->base.del = panel_io_spi_del;
    spi_panel_io->base.register_event_callbacks = panel_io_spi_register_event_callbacks;

    size_t max_trans_bytes = 0;
    ESP_GOTO_ON_ERROR(spi_bus_get_max_transaction_len((spi_host_device_t)bus, &max_trans_bytes), err, TAG, "get spi max transaction len failed");
    spi_panel_io->spi_trans_max_bytes = max_trans_bytes;

    *ret_io = &(spi_panel_io->base);
    ESP_LOGD(TAG, "new spi lcd panel io @%p, max_trans_bytes: %d", spi_panel_io, (int)max_trans_bytes);

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
    size_t num_trans_inflight = spi_panel_io->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
        spi_panel_io->num_trans_inflight--;
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

static esp_err_t panel_io_spi_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx)
{
    esp_lcd_panel_io_spi_t *spi_panel_io = __containerof(io, esp_lcd_panel_io_spi_t, base);

    if (spi_panel_io->on_color_trans_done != NULL) {
        ESP_LOGW(TAG, "Callback on_color_trans_done was already set and now it was overwritten!");
    }

    spi_panel_io->on_color_trans_done = cbs->on_color_trans_done;
    spi_panel_io->user_ctx = user_ctx;

    return ESP_OK;
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
    bool send_cmd = (lcd_cmd >= 0);

    ESP_RETURN_ON_ERROR(spi_device_acquire_bus(spi_panel_io->spi_dev, portMAX_DELAY), TAG, "acquire spi bus failed");

    // before issue a polling transaction, need to wait queued transactions finished
    size_t num_trans_inflight = spi_panel_io->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
        spi_panel_io->num_trans_inflight--;
    }
    lcd_trans = &spi_panel_io->trans_pool[0];
    memset(lcd_trans, 0, sizeof(lcd_spi_trans_descriptor_t));

    lcd_trans->base.user = spi_panel_io;
    if (param && param_size) {
        lcd_trans->base.flags |= SPI_TRANS_CS_KEEP_ACTIVE;
    }
    if (spi_panel_io->flags.octal_mode) {
        // use 8 lines for transmitting command, address and data
        lcd_trans->base.flags |= (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR | SPI_TRANS_MODE_OCT);
    }

    if (send_cmd) {
        spi_lcd_prepare_cmd_buffer(spi_panel_io, &lcd_cmd);
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_cmd_level; // set D/C level in command phase
        lcd_trans->base.length = spi_panel_io->lcd_cmd_bits;
        lcd_trans->base.tx_buffer = &lcd_cmd;
        // command is short, using polling mode
        ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) command failed");
    }

    if (param && param_size) {
        spi_lcd_prepare_param_buffer(spi_panel_io, param, param_size);
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_param_level; // set D/C level in param phase
        lcd_trans->base.length = param_size * 8; // transaction length is in bits
        lcd_trans->base.tx_buffer = param;
        lcd_trans->base.flags &= ~SPI_TRANS_CS_KEEP_ACTIVE;
        // parameter is usually short, using polling mode
        ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) param failed");
    }

err:
    spi_device_release_bus(spi_panel_io->spi_dev);

    return ret;
}

static esp_err_t panel_io_spi_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t *spi_trans = NULL;
    lcd_spi_trans_descriptor_t *lcd_trans = NULL;
    esp_lcd_panel_io_spi_t *spi_panel_io = __containerof(io, esp_lcd_panel_io_spi_t, base);
    bool send_cmd = (lcd_cmd >= 0);

    ESP_RETURN_ON_ERROR(spi_device_acquire_bus(spi_panel_io->spi_dev, portMAX_DELAY), TAG, "acquire spi bus failed");

    // before issue a polling transaction, need to wait queued transactions finished
    size_t num_trans_inflight = spi_panel_io->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
        spi_panel_io->num_trans_inflight--;
    }
    lcd_trans = &spi_panel_io->trans_pool[0];
    memset(lcd_trans, 0, sizeof(lcd_spi_trans_descriptor_t));

    lcd_trans->base.user = spi_panel_io;
    lcd_trans->base.flags |= SPI_TRANS_CS_KEEP_ACTIVE;
    if (spi_panel_io->flags.octal_mode) {
        // use 8 lines for transmitting command, address and data
        lcd_trans->base.flags |= (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR | SPI_TRANS_MODE_OCT);
    }

    if (send_cmd) {
        spi_lcd_prepare_cmd_buffer(spi_panel_io, &lcd_cmd);
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_cmd_level; // set D/C level in command phase
        lcd_trans->base.length = spi_panel_io->lcd_cmd_bits;
        lcd_trans->base.tx_buffer = &lcd_cmd;
        // command is short, using polling mode
        ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) command failed");
    }

    if (param && param_size) {
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_param_level; // set D/C level in param phase
        lcd_trans->base.length = 0;
        lcd_trans->base.tx_buffer = NULL;
        lcd_trans->base.rxlength = param_size * 8; // Read length in bits
        lcd_trans->base.rx_buffer = param;
        lcd_trans->base.flags &= ~SPI_TRANS_CS_KEEP_ACTIVE;
        // parameter is usually short, using polling mode
        ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) param failed");
    }

err:
    spi_device_release_bus(spi_panel_io->spi_dev);

    return ret;
}

static esp_err_t panel_io_spi_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t *spi_trans = NULL;
    lcd_spi_trans_descriptor_t *lcd_trans = NULL;
    esp_lcd_panel_io_spi_t *spi_panel_io = __containerof(io, esp_lcd_panel_io_spi_t, base);

    ESP_RETURN_ON_ERROR(spi_device_acquire_bus(spi_panel_io->spi_dev, portMAX_DELAY), TAG, "acquire spi bus failed");

    bool send_cmd = (lcd_cmd >= 0);
    if (send_cmd) {
        // before issue a polling transaction, need to wait queued transactions finished
        size_t num_trans_inflight = spi_panel_io->num_trans_inflight;
        for (size_t i = 0; i < num_trans_inflight; i++) {
            ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
            ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
            spi_panel_io->num_trans_inflight--;
        }
        lcd_trans = &spi_panel_io->trans_pool[0];
        memset(lcd_trans, 0, sizeof(lcd_spi_trans_descriptor_t));

        spi_lcd_prepare_cmd_buffer(spi_panel_io, &lcd_cmd);
        lcd_trans->base.user = spi_panel_io;
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_cmd_level; // set D/C level in command phase
        lcd_trans->base.length = spi_panel_io->lcd_cmd_bits;
        lcd_trans->base.tx_buffer = &lcd_cmd;
        if (color && color_size) {
            lcd_trans->base.flags |= SPI_TRANS_CS_KEEP_ACTIVE;
        }
        if (spi_panel_io->flags.octal_mode) {
            // use 8 lines for transmitting command, address and data
            lcd_trans->base.flags |= (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR | SPI_TRANS_MODE_OCT);
        }
        // command is short, using polling mode
        ret = spi_device_polling_transmit(spi_panel_io->spi_dev, &lcd_trans->base);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (polling) command failed");
    }

    // if the color buffer is big, we want to split it into chunks, and queue the chunks one by one
    do {
        size_t chunk_size = color_size;

        if (spi_panel_io->num_trans_inflight < spi_panel_io->queue_size) {
            // get the next available transaction
            lcd_trans = &spi_panel_io->trans_pool[spi_panel_io->num_trans_inflight];
        } else {
            // transaction pool has used up, recycle one transaction
            ret = spi_device_get_trans_result(spi_panel_io->spi_dev, &spi_trans, portMAX_DELAY);
            ESP_GOTO_ON_ERROR(ret, err, TAG, "recycle spi transactions failed");
            lcd_trans = __containerof(spi_trans, lcd_spi_trans_descriptor_t, base);
            spi_panel_io->num_trans_inflight--;
        }
        memset(lcd_trans, 0, sizeof(lcd_spi_trans_descriptor_t));

        // SPI per-transfer size has its limitation, if the color buffer is too big, we need to split it into multiple chunks
        if (chunk_size > spi_panel_io->spi_trans_max_bytes) {
            // cap the transfer size to the maximum supported by the bus
            chunk_size = spi_panel_io->spi_trans_max_bytes;
            lcd_trans->base.flags |= SPI_TRANS_CS_KEEP_ACTIVE;
        } else {
            // mark en_trans_done_cb only at the last round to avoid premature completion callback
            lcd_trans->flags.en_trans_done_cb = 1;
            lcd_trans->base.flags &= ~SPI_TRANS_CS_KEEP_ACTIVE;
        }

        lcd_trans->base.user = spi_panel_io;
        lcd_trans->flags.dc_gpio_level = spi_panel_io->flags.dc_data_level; // set D/C level in data phase
        lcd_trans->base.length = chunk_size * 8; // transaction length is in bits
        lcd_trans->base.tx_buffer = color;
        if (spi_panel_io->flags.octal_mode) {
            // use 8 lines for transmitting command, address and data
            lcd_trans->base.flags |= (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR | SPI_TRANS_MODE_OCT);
        } else if (spi_panel_io->flags.quad_mode) {
            // use 4 lines only for transmitting data
            lcd_trans->base.flags |= SPI_TRANS_MODE_QIO;
        }

        // color data is usually large, using queue+blocking mode
        ret = spi_device_queue_trans(spi_panel_io->spi_dev, &lcd_trans->base, portMAX_DELAY);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "spi transmit (queue) color failed");
        spi_panel_io->num_trans_inflight++;

        // move on to the next chunk
        color = (const uint8_t *)color + chunk_size;
        color_size -= chunk_size;
    } while (color_size > 0); // continue while we have remaining data to transmit

err:
    spi_device_release_bus(spi_panel_io->spi_dev);
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
    if (lcd_trans->flags.en_trans_done_cb) {
        if (spi_panel_io->on_color_trans_done) {
            spi_panel_io->on_color_trans_done(&spi_panel_io->base, NULL, spi_panel_io->user_ctx);
        }
    }
}
