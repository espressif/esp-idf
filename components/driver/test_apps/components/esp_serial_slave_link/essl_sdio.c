/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "esp_log.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_defs.h"

#include "essl_internal.h"
#include "essl_sdio.h"

static const char TAG[] = "essl_sdio";

#ifndef DR_REG_SLCHOST_BASE
#define DR_REG_SLCHOST_BASE             0 //The SDIO slave only check the least significant 10 bits, this doesn't matter
#endif

//This should be consistent with the macro in soc/host_reg.h
#define HOST_SLC0HOST_TOKEN_RDATA_REG   (DR_REG_SLCHOST_BASE + 0x44)
#define HOST_SLC0HOST_INT_RAW_REG       (DR_REG_SLCHOST_BASE + 0x50)
#define HOST_SLC0HOST_INT_ST_REG        (DR_REG_SLCHOST_BASE + 0x58)
#define HOST_SLCHOST_PKT_LEN_REG        (DR_REG_SLCHOST_BASE + 0x60)
#define HOST_SLCHOST_CONF_W0_REG        (DR_REG_SLCHOST_BASE + 0x6C)
#define HOST_SLCHOST_CONF_W7_REG        (DR_REG_SLCHOST_BASE + 0x8C)
#define HOST_SLC0HOST_INT_CLR_REG       (DR_REG_SLCHOST_BASE + 0xD4)
#define HOST_SLC0HOST_FUNC1_INT_ENA_REG (DR_REG_SLCHOST_BASE + 0xDC)


#define HOST_SLCHOST_CONF_W_REG(pos)    (HOST_SLCHOST_CONF_W0_REG+pos+(pos>23?4:0)+(pos>31?12:0))

#define ESSL_CMD53_END_ADDR    0x1f800

#define TX_BUFFER_MAX   0x1000
#define TX_BUFFER_MASK  0xFFF
#define RX_BYTE_MAX     0x100000
#define RX_BYTE_MASK    0xFFFFF

#define FUNC1_EN_MASK   (BIT(1))

/**
 * Initialize ``void`` over SDIO by this macro.
 */
#define ESSL_SDIO_DEFAULT_CONTEXT() (essl_dev_t){\
    .init = essl_sdio_init, \
    .wait_for_ready = essl_sdio_wait_for_ready, \
    .get_tx_buffer_num = essl_sdio_get_tx_buffer_num,\
    .update_tx_buffer_num = essl_sdio_update_tx_buffer_num,\
    .get_rx_data_size = essl_sdio_get_rx_data_size,\
    .update_rx_data_size = essl_sdio_update_rx_data_size,\
    .send_packet = essl_sdio_send_packet,\
    .get_packet = essl_sdio_get_packet,\
    .write_reg = essl_sdio_write_reg,\
    .read_reg = essl_sdio_read_reg,\
    .wait_int = essl_sdio_wait_int,\
    .send_slave_intr = essl_sdio_send_slave_intr, \
    .get_intr = essl_sdio_get_intr, \
    .clear_intr = essl_sdio_clear_intr, \
    .set_intr_ena = essl_sdio_set_intr_ena, \
    .reset_cnt = essl_sdio_reset_cnt, \
    }

typedef struct {
    //common part
    uint16_t        buffer_size;
    ///< All data that do not fully fill a buffer is still counted as one buffer. E.g. 10 bytes data costs 2 buffers if the size is 8 bytes per buffer.
    ///< Buffer size of the slave pre-defined between host and slave before communication.
    size_t          tx_sent_buffers;    ///< Counter holding the amount of buffers already sent to ESP32 slave. Should be set to 0 when initialization.
    size_t          tx_sent_buffers_latest;    ///< The latest reading (from the slave) of counter holding the amount of buffers loaded. Should be set to 0 when initialization.
    size_t          rx_got_bytes;       ///< Counter holding the amount of bytes already received from ESP32 slave. Should be set to 0 when initialization.
    size_t          rx_got_bytes_latest;       ///< The latest reading (from the slave) of counter holding the amount of bytes to send. Should be set to 0 when initialization.

    sdmmc_card_t   *card;               ///< Initialized sdmmc_cmd card
    uint16_t        block_size;
    ///< If this is too large, it takes time to send stuff bits; while if too small, intervals between blocks cost much.
    ///< Should be set according to length of data, and larger than ``TRANS_LEN_MAX/511``.
    ///< Block size of the SDIO function 1. After the initialization this will hold the value the slave really do. Valid value is 1-2048.
} essl_sdio_context_t;


esp_err_t essl_sdio_update_tx_buffer_num(void *arg, uint32_t wait_ms);
esp_err_t essl_sdio_update_rx_data_size(void *arg, uint32_t wait_ms);

static inline esp_err_t essl_sdio_write_byte(sdmmc_card_t *card, uint32_t addr, uint8_t val, uint8_t *val_o)
{
    return sdmmc_io_write_byte(card, 1, addr & 0x3FF, val, val_o);
}

static inline esp_err_t essl_sdio_write_bytes(sdmmc_card_t *card, uint32_t addr, uint8_t *val, int len)
{
    return sdmmc_io_write_bytes(card, 1, addr & 0x3FF, val, len);
}

static inline esp_err_t essl_sdio_read_byte(sdmmc_card_t *card, uint32_t addr, uint8_t *val_o)
{
    return sdmmc_io_read_byte(card, 1, addr & 0x3FF, val_o);
}

static inline esp_err_t essl_sdio_read_bytes(sdmmc_card_t *card, uint32_t addr, uint8_t *val_o, int len)
{
    return sdmmc_io_read_bytes(card, 1, addr & 0x3FF, val_o, len);
}

esp_err_t essl_sdio_init_dev(essl_handle_t *out_handle, const essl_sdio_config_t *config)
{
    esp_err_t ret = ESP_OK;
    essl_sdio_context_t *arg = NULL;
    essl_dev_t *dev = NULL;
    arg = (essl_sdio_context_t *)heap_caps_malloc(sizeof(essl_sdio_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    dev = (essl_dev_t *)heap_caps_malloc(sizeof(essl_dev_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (arg == NULL || dev == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    *dev = ESSL_SDIO_DEFAULT_CONTEXT();
    dev->args = arg;

    *arg = (essl_sdio_context_t) {
        .card = config->card,
        .block_size = 0x200,
        .buffer_size = config->recv_buffer_size,
        .tx_sent_buffers = 0,
        .rx_got_bytes = 0,
    };

    *out_handle = dev;
    return ESP_OK;

cleanup:
    free(arg);
    free(dev);
    return ret;
}

esp_err_t essl_sdio_deinit_dev(essl_handle_t handle)
{
    if (handle) {
        free (handle->args);
    }
    free(handle);
    return ESP_OK;
}

esp_err_t essl_sdio_init(void *arg, uint32_t wait_ms)
{
    essl_sdio_context_t *ctx = arg;
    esp_err_t err;
    uint8_t ioe = 0;
    sdmmc_card_t *card = ctx->card;

    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_ENABLE, &ioe);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "IOE: 0x%02"PRIx8, ioe);

    uint8_t ior = 0;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "IOR: 0x%02"PRIx8, ior);

    // enable function 1
    ioe |= FUNC1_EN_MASK;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_FN_ENABLE, ioe, &ioe);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "IOE: 0x%02"PRIx8, ioe);

    // wait for the card to become ready
    while ((ior & FUNC1_EN_MASK) == 0) {
        err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
        if (err != ESP_OK) {
            return err;
        }
        ESP_LOGD(TAG, "IOR: 0x%02"PRIx8, ior);
    }

    // get interrupt status
    uint8_t ie = 0;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_INT_ENABLE, &ie);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "IE: 0x%02"PRIx8, ie);

    // enable interrupts for function 1&2 and master enable
    ie |= BIT(0) | FUNC1_EN_MASK;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_INT_ENABLE, ie, &ie);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "IE: 0x%02"PRIx8, ie);

    // get bus width register
    uint8_t bus_width = 0;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BUS_WIDTH, &bus_width);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "BUS_WIDTH: 0x%02"PRIx8, bus_width);

    // enable continuous SPI interrupts
    bus_width |= CCCR_BUS_WIDTH_ECSI;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_BUS_WIDTH, bus_width, &bus_width);
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "BUS_WIDTH: 0x%02"PRIx8, bus_width);

    uint16_t bs = 512;
    const uint8_t *bs_u8 = (const uint8_t *) &bs;
    uint16_t bs_read = 0;
    uint8_t *bs_read_u8 = (uint8_t *) &bs_read;
    // Set block sizes for functions 0 to 512 bytes
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGD(TAG, "Function 0 BS: %d", (unsigned int) bs_read);

    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, SD_IO_CCCR_BLKSIZEL, bs_u8[0], NULL));
    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, SD_IO_CCCR_BLKSIZEH, bs_u8[1], NULL));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGD(TAG, "Function 0 BS: %d", (unsigned int) bs_read);

    // Set block sizes for functions 1 to given value (default value = 512).
    if (ctx->block_size > 0 && ctx->block_size <= 2048) {
        bs = ctx->block_size;
    } else {
        bs = 512;
    }
    size_t offset = SD_IO_FBR_START * 1;
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGD(TAG, "Function 1 BS: %d", (unsigned int) bs_read);

    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, bs_u8[0], NULL));
    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, bs_u8[1], NULL));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGD(TAG, "Function 1 BS: %d", (unsigned int) bs_read);

    if (bs_read != ctx->block_size) {
        ESP_LOGW(TAG, "Function1 block size %d different than set value %d", bs_read, ctx->block_size);
        ctx->block_size = bs_read;
    }
    return ESP_OK;
}

esp_err_t essl_sdio_wait_for_ready(void *arg, uint32_t wait_ms)
{
    ESP_LOGV(TAG, "wait_for_ioready");
    esp_err_t err;
    sdmmc_card_t *card = ((essl_sdio_context_t *)arg)->card;
    // wait for the card to become ready
    uint8_t ior = 0;
    while ((ior & FUNC1_EN_MASK) == 0) {
        err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
        if (err != ESP_OK) {
            return err;
        }
        ESP_LOGD(TAG, "IOR: 0x%02x", ior);
    }
    return ESP_OK;
}

esp_err_t essl_sdio_send_packet(void *arg, const void *start, size_t length, uint32_t wait_ms)
{
    essl_sdio_context_t *ctx = arg;
    uint16_t buffer_size = ctx->buffer_size;
    int buffer_used = (length + buffer_size - 1) / buffer_size;
    esp_err_t err;

    if (essl_sdio_get_tx_buffer_num(arg) < buffer_used) {
        //slave has no enough buffer, try update for once
        esp_err_t err = essl_sdio_update_tx_buffer_num(arg, wait_ms);
        if (err != ESP_OK) {
            return err;
        }
        if (essl_sdio_get_tx_buffer_num(arg) < buffer_used) {
            ESP_LOGV(TAG, "buffer is not enough: %d, %d required.", ctx->tx_sent_buffers_latest, ctx->tx_sent_buffers + buffer_used);
            return ESP_ERR_NOT_FOUND;
        }
    }

    ESP_LOGV(TAG, "send_packet: len: %d", length);

    uint8_t *start_ptr = (uint8_t *)start;
    uint32_t len_remain = length;
    do {
        const int block_size = 512;
        /* Though the driver supports to split packet of unaligned size into
         * length of 4x and 1~3, we still send aligned size of data to get
         * higher effeciency. The length is determined by the SDIO address, and
         * the remainning will be discard by the slave hardware.
         */
        int block_n = len_remain / block_size;
        int len_to_send;
        if (block_n) {
            len_to_send = block_n * block_size;
            err = sdmmc_io_write_blocks(ctx->card, 1, ESSL_CMD53_END_ADDR - len_remain, start_ptr, len_to_send);
        } else {
            len_to_send = len_remain;
            err = sdmmc_io_write_bytes(ctx->card, 1, ESSL_CMD53_END_ADDR - len_remain, start_ptr, (len_to_send + 3) & (~3));
        }
        if (err != ESP_OK) {
            return err;
        }
        start_ptr += len_to_send;
        len_remain -= len_to_send;
    } while (len_remain);

    ctx->tx_sent_buffers += buffer_used;
    return ESP_OK;
}

esp_err_t essl_sdio_get_packet(void *arg, void *out_data, size_t size, uint32_t wait_ms)
{
    essl_sdio_context_t *ctx = arg;
    esp_err_t err;

    ESP_LOGV(TAG, "get_packet: read size=%d", size);
    if (essl_sdio_get_rx_data_size(arg) < size) {
        err = essl_sdio_update_rx_data_size(arg, wait_ms);
        if (err != ESP_OK) {
            return err;
        }
        if (essl_sdio_get_rx_data_size(arg) < size) {
            return ESP_ERR_NOT_FOUND;
        }
    }

    uint8_t *start = out_data;
    uint32_t len_remain = size;
    do {
        const int block_size = 512; //currently our driver don't support block size other than 512
        int len_to_send;

        int block_n = len_remain / block_size;
        if (block_n != 0) {
            len_to_send = block_n * block_size;
            err = sdmmc_io_read_blocks(ctx->card, 1, ESSL_CMD53_END_ADDR - len_remain, start, len_to_send);
        } else {
            len_to_send = len_remain;
            /* though the driver supports to split packet of unaligned size into length
             * of 4x and 1~3, we still get aligned size of data to get higher
             * effeciency. The length is determined by the SDIO address, and the
             * remainning will be ignored by the slave hardware.
             */
            err = sdmmc_io_read_bytes(ctx->card, 1, ESSL_CMD53_END_ADDR - len_remain, start, (len_to_send + 3) & (~3));
        }
        if (err != ESP_OK) {
            return err;
        }
        start += len_to_send;
        len_remain -= len_to_send;
        ctx->rx_got_bytes += len_to_send;
    } while (len_remain != 0);

    return err;
}

uint32_t essl_sdio_get_tx_buffer_num(void *arg)
{
    essl_sdio_context_t *ctx = arg;
    ESP_LOGV(TAG, "tx latest: %d, sent: %d", ctx->tx_sent_buffers_latest, ctx->tx_sent_buffers);
    return (ctx->tx_sent_buffers_latest + TX_BUFFER_MAX - ctx->tx_sent_buffers) % TX_BUFFER_MAX;
}

esp_err_t essl_sdio_update_tx_buffer_num(void *arg, uint32_t wait_ms)
{
    essl_sdio_context_t *ctx = arg;
    uint32_t len;
    esp_err_t err;

    err = essl_sdio_read_bytes(ctx->card, HOST_SLC0HOST_TOKEN_RDATA_REG, (uint8_t *) &len, 4);
    if (err != ESP_OK) {
        return err;
    }
    len = (len >> 16)&TX_BUFFER_MASK;
    ctx->tx_sent_buffers_latest = len;
    ESP_LOGV(TAG, "update_tx_buffer_num: %d", (unsigned int)len);
    return ESP_OK;
}

uint32_t essl_sdio_get_rx_data_size(void *arg)
{
    essl_sdio_context_t *ctx = arg;
    ESP_LOGV(TAG, "rx latest: %d, read: %d", ctx->rx_got_bytes_latest, ctx->rx_got_bytes);
    return (ctx->rx_got_bytes_latest + RX_BYTE_MAX - ctx->rx_got_bytes) % RX_BYTE_MAX;
}

esp_err_t essl_sdio_update_rx_data_size(void *arg, uint32_t wait_ms)
{
    essl_sdio_context_t *ctx = arg;
    uint32_t len;
    esp_err_t err;

    ESP_LOGV(TAG, "get_rx_data_size: got_bytes: %d", ctx->rx_got_bytes);
    err = essl_sdio_read_bytes(ctx->card, HOST_SLCHOST_PKT_LEN_REG, (uint8_t *) &len, 4);
    if (err != ESP_OK) {
        return err;
    }
    len &= RX_BYTE_MASK;
    ctx->rx_got_bytes_latest = len;
    return ESP_OK;
}


esp_err_t essl_sdio_write_reg(void *arg, uint8_t addr, uint8_t value, uint8_t *value_o, uint32_t wait_ms)
{
    ESP_LOGV(TAG, "write_reg: 0x%02"PRIX8, value);
    // addrress over range
    if (addr >= 60) {
        return ESP_ERR_INVALID_ARG;
    }
    //W7 is reserved for interrupts
    if (addr >= 28) {
        addr += 4;
    }
    return essl_sdio_write_byte(((essl_sdio_context_t *)arg)->card, HOST_SLCHOST_CONF_W_REG(addr), value, value_o);
}

esp_err_t essl_sdio_read_reg(void *arg, uint8_t add, uint8_t *value_o, uint32_t wait_ms)
{
    ESP_LOGV(TAG, "read_reg");
    // address over range
    if (add >= 60) {
        return ESP_ERR_INVALID_ARG;
    }
    //W7 is reserved for interrupts
    if (add >= 28) {
        add += 4;
    }
    esp_err_t ret = essl_sdio_read_byte(((essl_sdio_context_t *)arg)->card, HOST_SLCHOST_CONF_W_REG(add), value_o);
    ESP_LOGV(TAG, "reg: %02"PRIX8, *value_o);
    return ret;
}

esp_err_t essl_sdio_clear_intr(void *arg, uint32_t intr_mask, uint32_t wait_ms)
{
    ESP_LOGV(TAG, "clear_intr: %08"PRIX32, intr_mask);
    return essl_sdio_write_bytes(((essl_sdio_context_t *) arg)->card, HOST_SLC0HOST_INT_CLR_REG, (uint8_t *) &intr_mask, 4);
}

esp_err_t essl_sdio_get_intr(void *arg, uint32_t *intr_raw, uint32_t *intr_st, uint32_t wait_ms)
{
    essl_sdio_context_t *ctx = arg;
    esp_err_t r;
    ESP_LOGV(TAG, "get_intr");
    if (intr_raw == NULL && intr_st == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (intr_raw != NULL) {
        r = essl_sdio_read_bytes(ctx->card, HOST_SLC0HOST_INT_RAW_REG, (uint8_t *) intr_raw, 4);
        if (r != ESP_OK) {
            return r;
        }
    }
    if (intr_st != NULL) {
        r = essl_sdio_read_bytes(ctx->card, HOST_SLC0HOST_INT_ST_REG, (uint8_t *) intr_st, 4);
        if (r != ESP_OK) {
            return r;
        }
    }
    return ESP_OK;
}

esp_err_t essl_sdio_set_intr_ena(void *arg, uint32_t ena_mask, uint32_t wait_ms)
{
    ESP_LOGV(TAG, "set_intr_ena: %08"PRIX32, ena_mask);
    return essl_sdio_write_bytes(((essl_sdio_context_t *)arg)->card, HOST_SLC0HOST_FUNC1_INT_ENA_REG,
                                 (uint8_t *) &ena_mask, 4);
}

esp_err_t essl_sdio_get_intr_ena(void *arg, uint32_t *ena_mask_o, uint32_t wait_ms)
{
    ESP_LOGV(TAG, "get_intr_ena");
    esp_err_t ret = essl_sdio_read_bytes(((essl_sdio_context_t *)arg)->card, HOST_SLC0HOST_FUNC1_INT_ENA_REG,
                                         (uint8_t *) ena_mask_o, 4);
    ESP_LOGV(TAG, "ena: %08"PRIX32, *ena_mask_o);
    return ret;
}

esp_err_t essl_sdio_send_slave_intr(void *arg, uint32_t intr_mask, uint32_t wait_ms)
{
    //Only 8 bits available
    ESP_LOGV(TAG, "send_slave_intr: %02"PRIx8, (uint8_t)intr_mask);
    return essl_sdio_write_byte(((essl_sdio_context_t *)arg)->card, HOST_SLCHOST_CONF_W7_REG + 0, (uint8_t)intr_mask, NULL);
}

esp_err_t essl_sdio_wait_int(void *arg, uint32_t wait_ms)
{
    return sdmmc_io_wait_int(((essl_sdio_context_t *)arg)->card, wait_ms);
}

void essl_sdio_reset_cnt(void *arg)
{
    essl_sdio_context_t *ctx = arg;
    ctx->rx_got_bytes = 0;
    ctx->tx_sent_buffers = 0;
}
