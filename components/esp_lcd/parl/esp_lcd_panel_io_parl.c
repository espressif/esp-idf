/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Although we're manipulating Parlio peripheral, it has nothing to do with the Parlio.
// In fact, we're simulating the Intel 8080 (8 data-width) or SPI(1 data-width) interface with Parlio peripheral.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "soc/lcd_periph.h"
#include "hal/gpio_hal.h"
#include "driver/gpio.h"
#include "driver/parlio_tx.h"
#include "driver/parlio_types.h"
#include "esp_private/gpio.h"
#include "esp_private/parlio_private.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_common.h"

static const char *TAG = "lcd_panel.io.parl";

typedef struct lcd_panel_io_parlio_t lcd_panel_io_parlio_t;
typedef struct parlio_trans_descriptor_t parlio_trans_descriptor_t;

static esp_err_t panel_io_parl_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_parl_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_parl_del(esp_lcd_panel_io_t *io);
static bool lcd_default_isr_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx);
static esp_err_t panel_io_parl_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx);

struct parlio_trans_descriptor_t {
    lcd_panel_io_parlio_t *parlio_device; // parlio device issuing this transaction
    const void *data;     // Data buffer
    uint32_t data_length; // Data buffer size
    struct {
        unsigned int en_trans_done_cb: 1;
    } flags;
};

struct lcd_panel_io_parlio_t {
    esp_lcd_panel_io_t base;   // Base class of generic lcd panel io
    parlio_tx_unit_handle_t tx_unit; // Parlio TX unit
    size_t data_width;      // Number of data lines
    int dc_gpio_num;       // GPIO used for DC line
    int cs_gpio_num;           // GPIO used for CS line
    int lcd_cmd_bits;          // Bit width of LCD command
    int lcd_param_bits;        // Bit width of LCD parameter
    void *user_ctx;            // private data used when transfer color data
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // color data trans done callback
    struct {
        unsigned int dc_cmd_level: 1;  // Level of DC line in CMD phase
        unsigned int dc_data_level: 1; // Level of DC line in DATA phase
    } dc_levels;
    parlio_trans_descriptor_t trans_desc; // Transaction description
};

esp_err_t esp_lcd_new_panel_io_parl(const esp_lcd_panel_io_parl_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_parlio_t *parlio_device = NULL;

    ESP_GOTO_ON_FALSE(io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    // Only support 1-bit(SPI) or 8-bit(I80) data width
#if SOC_PARLIO_SUPPORT_I80_LCD
    ESP_GOTO_ON_FALSE(io_config->data_width == 1 || io_config->data_width == 8, ESP_ERR_INVALID_ARG, err,
                      TAG, "invalid bus width:%d", io_config->data_width);
#else
    ESP_GOTO_ON_FALSE(io_config->data_width == 1, ESP_ERR_INVALID_ARG, err,
                      TAG, "invalid bus width:%d", io_config->data_width);
#endif

    // allocate parlio device memory
    parlio_device = heap_caps_calloc(1, sizeof(lcd_panel_io_parlio_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(parlio_device, ESP_ERR_NO_MEM, err, TAG, "no mem for parlio device");

    // initialize the parlio unit
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t pio_config = {
        .clk_src = io_config->clk_src,
        .data_width = io_config->data_width,
        .clk_in_gpio_num = -1, // use internal clock source
        .valid_gpio_num =  io_config->cs_gpio_num,
        .clk_out_gpio_num = io_config->clk_gpio_num,
        .data_gpio_nums = {
            io_config->data_gpio_nums[0],
            io_config->data_gpio_nums[1],
            io_config->data_gpio_nums[2],
            io_config->data_gpio_nums[3],
            io_config->data_gpio_nums[4],
            io_config->data_gpio_nums[5],
            io_config->data_gpio_nums[6],
            io_config->data_gpio_nums[7],
        },
        .output_clk_freq_hz = io_config->pclk_hz,
        .trans_queue_depth = io_config->trans_queue_depth ? io_config->trans_queue_depth : 4,
        .max_transfer_size = io_config->max_transfer_bytes,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .dma_burst_size = io_config->dma_burst_size,
        .flags.invert_valid_out = !io_config->flags.cs_active_high,
    };
    ESP_GOTO_ON_ERROR(parlio_new_tx_unit(&pio_config, &tx_unit), err, TAG, "config parlio tx unit failed");

    parlio_tx_event_callbacks_t parlio_cbs = {
        .on_trans_done = lcd_default_isr_callback,
    };
    ESP_GOTO_ON_ERROR(parlio_tx_unit_register_event_callbacks(tx_unit, &parlio_cbs, parlio_device), err, TAG, "register parlio tx callback failed");

    // DC signal is controlled by software, set as general purpose IO
    gpio_func_sel(io_config->dc_gpio_num, PIN_FUNC_GPIO);
    gpio_output_enable(io_config->dc_gpio_num);

    parlio_device->dc_gpio_num = io_config->dc_gpio_num;
    parlio_device->tx_unit = tx_unit;
    parlio_device->data_width = io_config->data_width;
    parlio_device->lcd_cmd_bits = io_config->lcd_cmd_bits;
    parlio_device->lcd_param_bits = io_config->lcd_param_bits;
    parlio_device->dc_levels.dc_cmd_level = io_config->dc_levels.dc_cmd_level;
    parlio_device->dc_levels.dc_data_level = io_config->dc_levels.dc_data_level;
    parlio_device->cs_gpio_num = io_config->cs_gpio_num;
    parlio_device->trans_desc.parlio_device = parlio_device;

    // fill panel io function table
    parlio_device->base.del = panel_io_parl_del;
    parlio_device->base.tx_param = panel_io_parl_tx_param;
    parlio_device->base.tx_color = panel_io_parl_tx_color;
    parlio_device->base.register_event_callbacks = panel_io_parl_register_event_callbacks;

    // enable parlio tx unit finally
    ESP_GOTO_ON_ERROR(parlio_tx_unit_enable(tx_unit), err, TAG, "enable parlio tx unit failed");

    *ret_io = &(parlio_device->base);
    ESP_LOGD(TAG, "new parlio lcd panel io @%p", parlio_device);
    return ESP_OK;

err:
    if (parlio_device) {
        if (parlio_device->tx_unit) {
            parlio_del_tx_unit(parlio_device->tx_unit);
        }
        free(parlio_device);
    }
    return ret;
}

void *esp_lcd_parlio_alloc_draw_buffer(esp_lcd_panel_io_handle_t io, size_t size, uint32_t caps)
{
    ESP_RETURN_ON_FALSE(io, NULL, TAG, "invalid argument");
    lcd_panel_io_parlio_t *parlio_device = __containerof(io, lcd_panel_io_parlio_t, base);
    size_t int_mem_align = 0;
    size_t ext_mem_align = 0;
    void *buf = NULL;
    parlio_tx_get_alignment_constraints(parlio_device->tx_unit, &int_mem_align, &ext_mem_align);
    // alloc from external memory
    if (caps & MALLOC_CAP_SPIRAM) {
        buf = heap_caps_aligned_calloc(ext_mem_align, 1, size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    } else {
        buf = heap_caps_aligned_calloc(int_mem_align, 1, size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    }
    return buf;
}

static esp_err_t panel_io_parl_del(esp_lcd_panel_io_t *io)
{
    lcd_panel_io_parlio_t *parlio_device = __containerof(io, lcd_panel_io_parlio_t, base);
    // wait all pending transaction to finish
    ESP_RETURN_ON_ERROR(parlio_tx_unit_wait_all_done(parlio_device->tx_unit, -1), TAG, "error waiting transmit done");
    ESP_LOGD(TAG, "del parlio lcd panel io @%p", parlio_device);

    ESP_RETURN_ON_ERROR(parlio_tx_unit_disable(parlio_device->tx_unit), TAG, "disable parlio tx unit failed");
    ESP_RETURN_ON_ERROR(parlio_del_tx_unit(parlio_device->tx_unit), TAG, "del parlio tx unit failed");

    free(parlio_device);
    return ESP_OK;
}

static esp_err_t panel_io_parl_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx)
{
    lcd_panel_io_parlio_t *parlio_device = __containerof(io, lcd_panel_io_parlio_t, base);
    if (parlio_device->on_color_trans_done != NULL) {
        ESP_LOGW(TAG, "Callback on_color_trans_done was already set and now it was overwritten!");
    }
    parlio_device->on_color_trans_done = cbs->on_color_trans_done;
    parlio_device->user_ctx = user_ctx;
    return ESP_OK;
}

static void parlio_lcd_prepare_cmd_buffer(parlio_trans_descriptor_t *trans_desc, const void *cmd)
{
    lcd_panel_io_parlio_t *parlio_device = trans_desc->parlio_device;
    uint8_t *from = (uint8_t *)cmd;
    // LCD is big-endian, e.g. to send command 0x1234, byte 0x12 should appear on the data bus first
    // However, the Parlio peripheral will send 0x34 first, so we reversed the order below
    if (parlio_device->data_width < parlio_device->lcd_cmd_bits) {
        int start = 0;
        int end = parlio_device->lcd_cmd_bits / 8 - 1;
        lcd_com_reverse_buffer_bytes(from, start, end);
    }
    trans_desc->data = cmd;
    trans_desc->data_length = MAX(parlio_device->lcd_cmd_bits, parlio_device->data_width) / 8;
}

static void parlio_lcd_prepare_param_buffer(parlio_trans_descriptor_t *trans_desc, const void *param, size_t param_num)
{
    lcd_panel_io_parlio_t *parlio_device = trans_desc->parlio_device;
    uint8_t *from = (uint8_t *)param;
    int param_size = parlio_device->lcd_param_bits / 8;
    // LCD is big-endian, e.g. to send param 0x1234, byte 0x12 should appear on the data bus first
    // However, the Parlio peripheral will send 0x34 first, so we reversed the order below
    if (parlio_device->data_width < parlio_device->lcd_param_bits) {
        for (size_t i = 0; i < param_num; i++) {
            int start = i * param_size;
            int end = start + param_size - 1;
            lcd_com_reverse_buffer_bytes(from, start, end);
        }
    }
    trans_desc->data = param;
    trans_desc->data_length = param_num;
}

static void parlio_lcd_prepare_color_buffer(parlio_trans_descriptor_t *trans_desc, const void *color, size_t color_size)
{
    trans_desc->data = color;
    trans_desc->data_length = color_size;
}

static esp_err_t panel_io_parl_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size)
{
    lcd_panel_io_parlio_t *parlio_device = __containerof(io, lcd_panel_io_parlio_t, base);
    parlio_trans_descriptor_t *trans_desc = NULL;

    // before issue a polling transaction, need to wait queued transactions finished
    ESP_RETURN_ON_ERROR(parlio_tx_unit_wait_all_done(parlio_device->tx_unit, -1), TAG, "error waiting transmit done");
    trans_desc = &parlio_device->trans_desc;
    trans_desc->flags.en_trans_done_cb = false;
    parlio_lcd_prepare_cmd_buffer(trans_desc, &lcd_cmd);

    gpio_set_level(parlio_device->dc_gpio_num, parlio_device->dc_levels.dc_cmd_level);
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };

    ESP_RETURN_ON_ERROR(parlio_tx_unit_transmit(parlio_device->tx_unit, trans_desc->data, trans_desc->data_length * 8, &transmit_config), TAG, "error transmit");
    if (param && param_size) {
        parlio_lcd_prepare_param_buffer(trans_desc, param, param_size * 8 / parlio_device->lcd_param_bits);
        // wait transmit done before changing DC level
        ESP_RETURN_ON_ERROR(parlio_tx_unit_wait_all_done(parlio_device->tx_unit, -1), TAG, "error waiting transmit done");
        gpio_set_level(parlio_device->dc_gpio_num, parlio_device->dc_levels.dc_data_level);
        ESP_RETURN_ON_ERROR(parlio_tx_unit_transmit(parlio_device->tx_unit, trans_desc->data, trans_desc->data_length * 8, &transmit_config), TAG, "error transmit");
    }
    // In case the lcd_cmd/param data is on the stack, wait transmit done to prevent it from being recycled
    ESP_RETURN_ON_ERROR(parlio_tx_unit_wait_all_done(parlio_device->tx_unit, -1), TAG, "error waiting transmit done");
    return ESP_OK;
}

static esp_err_t panel_io_parl_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size)
{
    lcd_panel_io_parlio_t *parlio_device = __containerof(io, lcd_panel_io_parlio_t, base);
    parlio_trans_descriptor_t *trans_desc = NULL;
    trans_desc = &parlio_device->trans_desc;

    if (lcd_cmd != -1) {
        // wait transmit done to prevent skipping previous color transfer callback
        ESP_RETURN_ON_ERROR(parlio_tx_unit_wait_all_done(parlio_device->tx_unit, -1), TAG, "error waiting transmit done");
        trans_desc->flags.en_trans_done_cb = false; // no callback for command transfer
        parlio_lcd_prepare_cmd_buffer(trans_desc, &lcd_cmd);
        gpio_set_level(parlio_device->dc_gpio_num, parlio_device->dc_levels.dc_cmd_level);
        parlio_transmit_config_t transmit_config = {
            .idle_value = 0x00,
        };
        ESP_RETURN_ON_ERROR(parlio_tx_unit_transmit(parlio_device->tx_unit, trans_desc->data, trans_desc->data_length * 8, &transmit_config), TAG, "error transmit");
        // wait transmit done before changing DC level
        ESP_RETURN_ON_ERROR(parlio_tx_unit_wait_all_done(parlio_device->tx_unit, -1), TAG, "error waiting transmit done");
    }

    parlio_lcd_prepare_color_buffer(trans_desc, color, color_size);
    trans_desc->flags.en_trans_done_cb = true;

    gpio_set_level(parlio_device->dc_gpio_num, parlio_device->dc_levels.dc_data_level);
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    ESP_RETURN_ON_ERROR(parlio_tx_unit_transmit(parlio_device->tx_unit, trans_desc->data, trans_desc->data_length * 8, &transmit_config), TAG, "error transmit");
    return ESP_OK;
}

IRAM_ATTR static bool lcd_default_isr_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx)
{
    lcd_panel_io_parlio_t *parlio_device = (lcd_panel_io_parlio_t *)user_ctx;
    parlio_trans_descriptor_t *trans_desc = &parlio_device->trans_desc;
    bool need_yield = false;

    // device callback
    if (trans_desc->flags.en_trans_done_cb) {
        if (parlio_device->on_color_trans_done) {
            if (parlio_device->on_color_trans_done(&parlio_device->base, NULL, parlio_device->user_ctx)) {
                need_yield = true;
            }
        }
    }

    return need_yield;
}
