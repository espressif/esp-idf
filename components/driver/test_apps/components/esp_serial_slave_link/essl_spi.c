/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_private/periph_ctrl.h"

#include "driver/spi_master.h"
#include "hal/spi_types.h"
#include "hal/spi_ll.h"

#include "essl_internal.h"
#include "essl_spi.h"

/**
 * Initialise device function list of SPI by this macro.
 */
#define ESSL_SPI_DEFAULT_DEV_FUNC() (essl_dev_t) {\
    .get_tx_buffer_num = essl_spi_get_tx_buffer_num,\
    .update_tx_buffer_num = essl_spi_update_tx_buffer_num,\
    .get_rx_data_size = essl_spi_get_rx_data_size,\
    .update_rx_data_size = essl_spi_update_rx_data_size,\
    .send_packet = essl_spi_send_packet,\
    .get_packet = essl_spi_get_packet,\
    .write_reg = essl_spi_write_reg,\
    .read_reg = essl_spi_read_reg,\
}

static const char TAG[] = "essl_spi";

typedef struct {
    spi_device_handle_t     spi;                    // Pointer to SPI device handle.
    /* Master TX, Slave RX */
    struct {
        size_t                  sent_buf_num;           // Number of TX buffers that has been sent out by the master.
        size_t                  slave_rx_buf_num;       // Number of RX buffers laoded by the slave.
        uint16_t                tx_buffer_size;         /* Buffer size for Master TX / Slave RX direction.
                                                         * Data with length within this size will still be regarded as one buffer.
                                                         * E.g. 10 bytes data costs 2 buffers if the size is 8 bytes per buffer. */
        uint8_t                 tx_sync_reg;            // The pre-negotiated register ID for Master-TX-SLAVE-RX synchronization. 1 word (4 Bytes) will be reserved for the synchronization.
    } master_out;
    /* Master RX, Slave TX */
    struct {
        size_t                  received_bytes;         // Number of the RX bytes that has been received by the Master.
        size_t                  slave_tx_bytes;         // Number of the TX bytes that has been loaded by the Slave
        uint8_t                 rx_sync_reg;            // The pre-negotiated register ID for Master-RX-SLAVE-TX synchronization. 1 word (4 Bytes) will be reserved for the synchronization.
    } master_in;
} essl_spi_context_t;

static uint16_t get_hd_command(spi_command_t cmd_t, uint32_t flags)
{
    spi_line_mode_t line_mode = {
        .cmd_lines = 1,
    };

    if (flags & SPI_TRANS_MODE_DIO) {
        line_mode.data_lines = 2;
        if (flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
            line_mode.addr_lines = 2;
        } else {
            line_mode.addr_lines = 1;
        }
    } else if (flags & SPI_TRANS_MODE_QIO) {
        line_mode.data_lines = 4;
        if (flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
            line_mode.addr_lines = 4;
        } else {
            line_mode.addr_lines = 1;
        }
    } else {
        line_mode.data_lines = 1;
        line_mode.addr_lines = 1;
    }

    return spi_ll_get_slave_hd_command(cmd_t, line_mode);
}

static int get_hd_dummy_bits(uint32_t flags)
{
    spi_line_mode_t line_mode = {};

    if (flags & SPI_TRANS_MODE_DIO) {
        line_mode.data_lines = 2;
    } else if (flags & SPI_TRANS_MODE_QIO) {
        line_mode.data_lines = 4;
    } else {
        line_mode.data_lines = 1;
    }

    return spi_ll_get_slave_hd_dummy_bits(line_mode);
}

esp_err_t essl_spi_rdbuf(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(SPI_CMD_HD_RDBUF, flags),
            .addr = addr % 72,
            .rxlength = len * 8,
            .rx_buffer = out_data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };

    return spi_device_transmit(spi, (spi_transaction_t *)&t);
}

esp_err_t essl_spi_rdbuf_polling(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(SPI_CMD_HD_RDBUF, flags),
            .addr = addr % 72,
            .rxlength = len * 8,
            .rx_buffer = out_data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };

    return spi_device_polling_transmit(spi, (spi_transaction_t *)&t);
}

esp_err_t essl_spi_wrbuf(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(SPI_CMD_HD_WRBUF, flags),
            .addr = addr % 72,
            .length = len * 8,
            .tx_buffer = data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_transmit(spi, (spi_transaction_t *)&t);
}

esp_err_t essl_spi_wrbuf_polling(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(SPI_CMD_HD_WRBUF, flags),
            .addr = addr % 72,
            .length = len * 8,
            .tx_buffer = data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_polling_transmit(spi, (spi_transaction_t *)&t);
}

esp_err_t essl_spi_rddma_seg(spi_device_handle_t spi, uint8_t *out_data, int seg_len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(SPI_CMD_HD_RDDMA, flags),
            .rxlength = seg_len * 8,
            .rx_buffer = out_data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_transmit(spi, (spi_transaction_t *)&t);
}

esp_err_t essl_spi_rddma_done(spi_device_handle_t spi, uint32_t flags)
{
    spi_transaction_t end_t = {
        .cmd = get_hd_command(SPI_CMD_HD_INT0, flags),
        .flags = flags,
    };
    return spi_device_transmit(spi, &end_t);
}

esp_err_t essl_spi_rddma(spi_device_handle_t spi, uint8_t *out_data, int len, int seg_len, uint32_t flags)
{
    if (!esp_ptr_dma_capable(out_data) || ((intptr_t)out_data % 4) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    seg_len = (seg_len > 0) ? seg_len : len;

    uint8_t *read_ptr = out_data;
    esp_err_t ret = ESP_OK;
    while (len > 0) {
        int send_len = MIN(seg_len, len);

        ret = essl_spi_rddma_seg(spi, read_ptr, send_len, flags);
        if (ret != ESP_OK) {
            return ret;
        }

        len -= send_len;
        read_ptr += send_len;
    }
    return essl_spi_rddma_done(spi, flags);
}

esp_err_t essl_spi_wrdma_seg(spi_device_handle_t spi, const uint8_t *data, int seg_len, uint32_t flags)
{
    spi_transaction_ext_t t = {
        .base = {
            .cmd = get_hd_command(SPI_CMD_HD_WRDMA, flags),
            .length = seg_len * 8,
            .tx_buffer = data,
            .flags = flags | SPI_TRANS_VARIABLE_DUMMY,
        },
        .dummy_bits = get_hd_dummy_bits(flags),
    };
    return spi_device_transmit(spi, (spi_transaction_t *)&t);
}

esp_err_t essl_spi_wrdma_done(spi_device_handle_t spi, uint32_t flags)
{
    spi_transaction_t end_t = {
        .cmd = get_hd_command(SPI_CMD_HD_WR_END, flags),
        .flags = flags,
    };
    return spi_device_transmit(spi, &end_t);
}

esp_err_t essl_spi_wrdma(spi_device_handle_t spi, const uint8_t *data, int len, int seg_len, uint32_t flags)
{
    if (!esp_ptr_dma_capable(data)) {
        return ESP_ERR_INVALID_ARG;
    }
    seg_len = (seg_len > 0) ? seg_len : len;

    while (len > 0) {
        int send_len = MIN(seg_len, len);

        esp_err_t ret = essl_spi_wrdma_seg(spi, data, send_len, flags);
        if (ret != ESP_OK) {
            return ret;
        }

        len -= send_len;
        data += send_len;
    }

    return essl_spi_wrdma_done(spi, flags);
}

esp_err_t essl_spi_int(spi_device_handle_t spi, int int_n, uint32_t flags)
{
    spi_transaction_t end_t = {
        .cmd = get_hd_command(SPI_CMD_HD_INT0 + int_n, flags),
        .flags = flags,
    };
    return spi_device_transmit(spi, &end_t);
}

//------------------------------------ APPEND MODE ----------------------------------//
static uint32_t essl_spi_get_rx_data_size(void *arg);
static esp_err_t essl_spi_update_rx_data_size(void *arg, uint32_t wait_ms);
static uint32_t essl_spi_get_tx_buffer_num(void *arg);
static esp_err_t essl_spi_update_tx_buffer_num(void *arg, uint32_t wait_ms);

esp_err_t essl_spi_init_dev(essl_handle_t *out_handle, const essl_spi_config_t *init_config)
{
    ESP_RETURN_ON_FALSE(init_config->spi, ESP_ERR_INVALID_STATE, TAG, "Check SPI initialization first");
    ESP_RETURN_ON_FALSE(init_config->tx_sync_reg <= (SOC_SPI_MAXIMUM_BUFFER_SIZE - 1) * 4, ESP_ERR_INVALID_ARG, TAG, "GPSPI supports %d-byte-width internal registers", SOC_SPI_MAXIMUM_BUFFER_SIZE);
    ESP_RETURN_ON_FALSE(init_config->rx_sync_reg <= (SOC_SPI_MAXIMUM_BUFFER_SIZE - 1) * 4, ESP_ERR_INVALID_ARG, TAG, "GPSPI supports %d-byte-width internal registers", SOC_SPI_MAXIMUM_BUFFER_SIZE);
    ESP_RETURN_ON_FALSE(init_config->tx_sync_reg != init_config->rx_sync_reg, ESP_ERR_INVALID_ARG, TAG, "Should use different word of registers for synchronization");

    essl_spi_context_t *context = calloc(1, sizeof(essl_spi_context_t));
    essl_dev_t *dev = calloc(1, sizeof(essl_dev_t));
    if (!context || !dev) {
        free(context);
        free(dev);
        return ESP_ERR_NO_MEM;
    }

    *context = (essl_spi_context_t) {
        .spi = *init_config->spi,
        .master_out.tx_buffer_size = init_config->tx_buf_size,
        .master_out.tx_sync_reg = init_config->tx_sync_reg,
        .master_in.rx_sync_reg = init_config->rx_sync_reg
    };

    *dev = ESSL_SPI_DEFAULT_DEV_FUNC();
    dev->args = context;

    *out_handle = dev;

    return ESP_OK;
}

esp_err_t essl_spi_deinit_dev(essl_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_STATE, TAG, "ESSL SPI is not in use");
    free(handle->args);
    free(handle);
    return ESP_OK;
}

void essl_spi_reset_cnt(void *arg)
{
    essl_spi_context_t *ctx = arg;
    if (ctx) {
        ctx->master_out.sent_buf_num = 0;
        ctx->master_in.received_bytes = 0;
    }
}

//------------------------------------ RX ----------------------------------//
esp_err_t essl_spi_read_reg(void *arg, uint8_t addr, uint8_t *out_value, uint32_t wait_ms)
{
    essl_spi_context_t *ctx = arg;
    ESP_RETURN_ON_FALSE(arg, ESP_ERR_INVALID_STATE, TAG, "Check ESSL SPI initialization first");
    uint8_t reserved_1_head = ctx->master_out.tx_sync_reg < ctx->master_in.rx_sync_reg ? ctx->master_out.tx_sync_reg : ctx->master_in.rx_sync_reg;
    uint8_t reserved_1_tail = reserved_1_head + 3;
    uint8_t reserved_2_head = ctx->master_out.tx_sync_reg < ctx->master_in.rx_sync_reg ? ctx->master_in.rx_sync_reg : ctx->master_out.tx_sync_reg;
    uint8_t reserved_2_tail = reserved_2_head + 3;
    ESP_RETURN_ON_FALSE(addr < reserved_1_head || (addr > reserved_1_tail && addr < reserved_2_head) || addr > reserved_2_tail, ESP_ERR_INVALID_ARG, TAG, "Invalid address");

    return essl_spi_rdbuf(ctx->spi, out_value, addr, sizeof(uint8_t), 0);
}

static uint32_t essl_spi_get_rx_data_size(void *arg)
{
    essl_spi_context_t *ctx = arg;
    ESP_LOGV(TAG, "slave tx buffer: %d bytes, master has read: %d bytes", ctx->master_in.slave_tx_bytes, ctx->master_in.received_bytes);
    return ctx->master_in.slave_tx_bytes - ctx->master_in.received_bytes;
}

static esp_err_t essl_spi_update_rx_data_size(void *arg, uint32_t wait_ms)
{
    essl_spi_context_t *ctx = arg;
    uint32_t updated_size = 0;
    uint32_t previous_size = 0;
    esp_err_t ret;

    ret = essl_spi_rdbuf_polling(ctx->spi, (uint8_t *)&previous_size, ctx->master_in.rx_sync_reg, sizeof(uint32_t), 0);
    if (ret != ESP_OK) {
        return ret;
    }

    /**
     * Read until the last 2 reading result are same. Reason:
     * SPI transaction is carried on per 1 Byte. So when Master is reading the shared register, if the
     * register value is changed by Slave at this time, Master may get wrong data.
     */
    while (1) {
        ret = essl_spi_rdbuf_polling(ctx->spi, (uint8_t *)&updated_size, ctx->master_in.rx_sync_reg, sizeof(uint32_t), 0);
        if (ret != ESP_OK) {
            return ret;
        }
        if (updated_size == previous_size) {
            ctx->master_in.slave_tx_bytes = updated_size;
            ESP_LOGV(TAG, "updated: slave prepared tx buffer is: %d bytes", (unsigned int)updated_size);
            return ret;
        }
        previous_size = updated_size;
    }
}

esp_err_t essl_spi_get_packet(void *arg, void *out_data, size_t size, uint32_t wait_ms)
{
    ESP_RETURN_ON_FALSE(arg, ESP_ERR_INVALID_STATE, TAG, "Check ESSL SPI initialization first");
    if (!esp_ptr_dma_capable(out_data) || ((intptr_t)out_data % 4) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    essl_spi_context_t *ctx = arg;
    esp_err_t ret;

    if (essl_spi_get_rx_data_size(arg) < size) {
        /**
         * For realistic situation, usually there will be a large overhead (Slave will load large amount of data),
         * so here we only update the Slave's TX size when the last-updated size is smaller than what Master requires.
         */
        ret = essl_spi_update_rx_data_size(arg, wait_ms);
        if (ret != ESP_OK) {
            return ret;
        }

        //Slave still did not load enough size of buffer
        if (essl_spi_get_rx_data_size(arg) < size) {
            ESP_LOGV(TAG, "slave buffer: %d is not enough, %d is required", ctx->master_in.slave_tx_bytes, ctx->master_in.received_bytes + size);
            return ESP_ERR_NOT_FOUND;
        }
    }

    ESP_LOGV(TAG, "get_packet: size to read is: %d", size);
    ret = essl_spi_rddma_seg(ctx->spi, out_data, size, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    ctx->master_in.received_bytes += size;

    return ESP_OK;
}

//------------------------------------ TX ----------------------------------//
esp_err_t essl_spi_write_reg(void *arg, uint8_t addr, uint8_t value, uint8_t *out_value, uint32_t wait_ms)
{
    essl_spi_context_t *ctx = arg;
    ESP_RETURN_ON_FALSE(arg, ESP_ERR_INVALID_STATE, TAG, "Check ESSL SPI initialization first");
    uint8_t reserved_1_head = ctx->master_out.tx_sync_reg < ctx->master_in.rx_sync_reg ? ctx->master_out.tx_sync_reg : ctx->master_in.rx_sync_reg;
    uint8_t reserved_1_tail = reserved_1_head + 3;
    uint8_t reserved_2_head = ctx->master_out.tx_sync_reg < ctx->master_in.rx_sync_reg ? ctx->master_in.rx_sync_reg : ctx->master_out.tx_sync_reg;
    uint8_t reserved_2_tail = reserved_2_head + 3;
    ESP_RETURN_ON_FALSE(addr < reserved_1_head || (addr > reserved_1_tail && addr < reserved_2_head) || addr > reserved_2_tail, ESP_ERR_INVALID_ARG, TAG, "Invalid address");
    ESP_RETURN_ON_FALSE(out_value == NULL, ESP_ERR_NOT_SUPPORTED, TAG, "This feature is not supported");

    return essl_spi_wrbuf(ctx->spi, &value, addr, sizeof(uint8_t), 0);
}

static uint32_t essl_spi_get_tx_buffer_num(void *arg)
{
    essl_spi_context_t *ctx = arg;
    ESP_LOGV(TAG, "slave rx buffer: %d, master has sent: %d", ctx->master_out.slave_rx_buf_num, ctx->master_out.sent_buf_num);
    return ctx->master_out.slave_rx_buf_num - ctx->master_out.sent_buf_num;
}

static esp_err_t essl_spi_update_tx_buffer_num(void *arg, uint32_t wait_ms)
{
    essl_spi_context_t *ctx = arg;
    uint32_t updated_num = 0;
    uint32_t previous_size = 0;
    esp_err_t ret;

    ret = essl_spi_rdbuf_polling(ctx->spi, (uint8_t *)&previous_size, ctx->master_out.tx_sync_reg, sizeof(uint32_t), 0);
    if (ret != ESP_OK) {
        return ret;
    }

    /**
     * Read until the last 2 reading result are same. Reason:
     * SPI transaction is carried on per 1 Byte. So when Master is reading the shared register, if the
     * register value is changed by Slave at this time, Master may get wrong data.
     */
    while (1) {
        ret = essl_spi_rdbuf_polling(ctx->spi, (uint8_t *)&updated_num, ctx->master_out.tx_sync_reg, sizeof(uint32_t), 0);
        if (ret != ESP_OK) {
            return ret;
        }
        if (updated_num == previous_size) {
            ctx->master_out.slave_rx_buf_num = updated_num;
            ESP_LOGV(TAG, "updated: slave prepared rx buffer: %d", (unsigned int)updated_num);
            return ret;
        }
        previous_size = updated_num;
    }
}

esp_err_t essl_spi_send_packet(void *arg, const void *data, size_t size, uint32_t wait_ms)
{
    ESP_RETURN_ON_FALSE(arg, ESP_ERR_INVALID_STATE, TAG, "Check ESSL SPI initialization first");
    if (!esp_ptr_dma_capable(data)) {
        return ESP_ERR_INVALID_ARG;
    }

    essl_spi_context_t *ctx = arg;
    esp_err_t ret;
    uint32_t buf_num_to_use = (size + ctx->master_out.tx_buffer_size - 1) / ctx->master_out.tx_buffer_size;

    if (essl_spi_get_tx_buffer_num(arg) < buf_num_to_use) {
        /**
         * For realistic situation, usually there will be a large overhead (Slave will load enough number of RX buffers),
         * so here we only update the Slave's RX buffer number when the last-updated number is smaller than what Master requires.
         */
        ret = essl_spi_update_tx_buffer_num(arg, wait_ms);
        if (ret != ESP_OK) {
            return ret;
        }
        //Slave still did not load a sufficient amount of buffers
        if (essl_spi_get_tx_buffer_num(arg) < buf_num_to_use) {
            ESP_LOGV(TAG, "slave buffer: %"PRIu32" is not enough, %"PRIu32" is required", (uint32_t)ctx->master_out.slave_rx_buf_num, (uint32_t)ctx->master_out.sent_buf_num + buf_num_to_use);
            return ESP_ERR_NOT_FOUND;
        }
    }

    ESP_LOGV(TAG, "send_packet: size to write is: %zu", size);
    ret = essl_spi_wrdma_seg(ctx->spi, data, size, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    ctx->master_out.sent_buf_num += buf_num_to_use;

    return essl_spi_wrdma_done(ctx->spi, 0);
}
