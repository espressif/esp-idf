/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* -------------------------------------------------- */
/* BLE Log - Peripheral Interface - SPI Master HD DMA */
/* -------------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph_spi_master_hd.h"
#include "ble_log_prph_spi_common.h"
#include "ble_log_lbm.h"

#include "hal/spi_ll.h"
#include "hal/spi_types.h"

/* MACRO */
#define BLE_LOG_SPI_HD_DATA_DONE            BIT(0)
#define BLE_LOG_SPI_HD_END_QUEUE_FAILED     BIT(1)

#if CONFIG_SPI_MASTER_ISR_IN_IRAM
#define BLE_LOG_SPI_MASTER_HD_CB_ATTR       BLE_LOG_IRAM_ATTR
#else
#define BLE_LOG_SPI_MASTER_HD_CB_ATTR
#endif

/* VARIABLE */
BLE_LOG_STATIC bool prph_inited = false;
BLE_LOG_STATIC bool bus_inited = false;
BLE_LOG_STATIC spi_device_handle_t dev_handle = NULL;

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC void spi_master_hd_tx_done_cb(spi_transaction_t *spi_trans);

/* PRIVATE FUNCTION */
BLE_LOG_SPI_MASTER_HD_CB_ATTR BLE_LOG_STATIC void spi_master_hd_tx_done_cb(spi_transaction_t *spi_trans)
{
    ble_log_prph_trans_ctx_t *ctx = (ble_log_prph_trans_ctx_t *)spi_trans->user;
    if (spi_trans == &ctx->data) {
        uint8_t old_status = __atomic_fetch_or(&ctx->status, BLE_LOG_SPI_HD_DATA_DONE, __ATOMIC_ACQ_REL);
        if (!(old_status & BLE_LOG_SPI_HD_END_QUEUE_FAILED)) {
            return;
        }
    }

    ctx->trans->pos = 0;
    ble_log_lbm_recycle_trans(ctx->trans);
}

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt)
{
    /* Avoid double init */
    if (prph_inited) {
        return true;
    }

    /* SPI master initialization */
    spi_bus_config_t bus_config = {
        .miso_io_num = -1,
        .mosi_io_num = BLE_LOG_SPI_MOSI_IO_NUM,
        .sclk_io_num = BLE_LOG_SPI_SCLK_IO_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = BLE_LOG_SPI_MAX_TRANSFER_SIZE,
#if CONFIG_SPI_MASTER_ISR_IN_IRAM
        .intr_flags = ESP_INTR_FLAG_IRAM
#endif // CONFIG_SPI_MASTER_ISR_IN_IRAM
    };
    if (spi_bus_initialize(BLE_LOG_SPI_BUS, &bus_config, SPI_DMA_CH_AUTO) != ESP_OK) {
        goto exit;
    }
    bus_inited = true;

    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = SPI_MASTER_FREQ_20M,
        .mode = 0,
        .spics_io_num = BLE_LOG_SPI_CS_IO_NUM,
        .queue_size = (int)(trans_cnt * 2),
        .command_bits = 8,
        .address_bits = 8,
        .dummy_bits = spi_ll_get_slave_hd_dummy_bits((spi_line_mode_t) {
            .cmd_lines = 1,
            .data_lines = 1,
        }),
        .post_cb = spi_master_hd_tx_done_cb,
        .flags = SPI_DEVICE_NO_RETURN_RESULT | SPI_DEVICE_HALFDUPLEX,
    };
    if (spi_bus_add_device(BLE_LOG_SPI_BUS, &dev_config, &dev_handle) != ESP_OK) {
        goto exit;
    }

    /* Initialization done */
    prph_inited = true;
    return true;

exit:
    ble_log_prph_deinit();
    return false;
}

void ble_log_prph_deinit(void)
{
    prph_inited = false;
    if (dev_handle) {
        if (spi_device_acquire_bus(dev_handle, portMAX_DELAY) == ESP_OK) {
            spi_device_release_bus(dev_handle);
        }
        spi_bus_remove_device(dev_handle);
        dev_handle = NULL;
    }

    if (bus_inited) {
        spi_bus_free(BLE_LOG_SPI_BUS);
        bus_inited = false;
    }
}

bool ble_log_prph_trans_init(ble_log_prph_trans_t **trans, size_t trans_size)
{
    /* Validate inputs */
    if (!trans || !trans_size) {
        return false;
    }

    /* Initialize peripheral transport data */
    *trans = (ble_log_prph_trans_t *)BLE_LOG_MALLOC(sizeof(ble_log_prph_trans_t));
    if (!(*trans)) {
        goto exit;
    }
    BLE_LOG_MEMSET(*trans, 0, sizeof(ble_log_prph_trans_t));
    (*trans)->size = trans_size;

    /* Initialize peripheral-specific transport context */
    ble_log_prph_trans_ctx_t *spi_trans_ctx = (ble_log_prph_trans_ctx_t *)BLE_LOG_MALLOC(sizeof(ble_log_prph_trans_ctx_t));
    if (!spi_trans_ctx) {
        goto exit;
    }
    BLE_LOG_MEMSET(spi_trans_ctx, 0, sizeof(ble_log_prph_trans_ctx_t));

    spi_line_mode_t line_mode = {
        .cmd_lines = 1,
        .data_lines = 1,
    };
    spi_trans_ctx->data.cmd = spi_ll_get_slave_hd_command(SPI_CMD_HD_WRDMA, line_mode);
    spi_trans_ctx->data.user = spi_trans_ctx;
    spi_trans_ctx->end.cmd = spi_ll_get_slave_hd_command(SPI_CMD_HD_WR_END, line_mode);
    spi_trans_ctx->end.user = spi_trans_ctx;
    spi_trans_ctx->trans = *trans;
    (*trans)->ctx = (void *)spi_trans_ctx;

    /* Initialize log buffer */
    (*trans)->buf = (uint8_t *)BLE_LOG_ALIGNED_MALLOC(trans_size);
    if (!(*trans)->buf) {
        goto exit;
    }
    BLE_LOG_MEMSET((*trans)->buf, 0, trans_size);
    spi_trans_ctx->data.tx_buffer = (const void *)(*trans)->buf;
    return true;

exit:
    ble_log_prph_trans_deinit(trans);
    return false;
}

void ble_log_prph_trans_deinit(ble_log_prph_trans_t **trans)
{
    /* Validate inputs */
    if (!trans || !(*trans)) {
        return;
    }

    /* Release log buffer */
    if ((*trans)->buf) {
        BLE_LOG_FREE((*trans)->buf);
    }

    /* Release peripheral-specific transport context */
    if ((*trans)->ctx) {
        BLE_LOG_FREE((*trans)->ctx);
    }

    /* Release peripheral transport data */
    BLE_LOG_FREE(*trans);
    *trans = NULL;
}

/* CRITICAL:
 * This function is designed to be called by BLE Log Runtime only,
 * function call from any other submodules is not allowed */
BLE_LOG_IRAM_ATTR void ble_log_prph_send_trans(ble_log_prph_trans_t *trans)
{
    ble_log_prph_trans_ctx_t *ctx = (ble_log_prph_trans_ctx_t *)trans->ctx;
    uint16_t tx_len = trans->pos;

    /*
     * SPI slave HD DMA requires transaction length to be 4-byte aligned.
     * Pad trailing bytes with zero to reduce transport loss on slave side.
     */
    uint16_t aligned_len = (uint16_t)((tx_len + (BLE_LOG_SPI_DMA_ALIGN_BYTES - 1U)) &
                                      ~(BLE_LOG_SPI_DMA_ALIGN_BYTES - 1U));
    if (aligned_len != tx_len) {
        uint16_t pad_len = (uint16_t)(aligned_len - tx_len);
        if (aligned_len <= trans->size) {
            BLE_LOG_MEMSET(trans->buf + tx_len, 0, pad_len);
            tx_len = aligned_len;
        }
    }

    ctx->status = 0;
    ctx->data.length = (tx_len << 3);
    ctx->data.rxlength = 0;
    if (spi_device_queue_trans(dev_handle, &ctx->data, 0) != ESP_OK) {
        ble_log_lbm_recycle_trans(trans);
        return;
    }

    if (spi_device_queue_trans(dev_handle, &ctx->end, 0) != ESP_OK) {
        uint8_t old_status = __atomic_fetch_or(&ctx->status, BLE_LOG_SPI_HD_END_QUEUE_FAILED, __ATOMIC_ACQ_REL);
        if (old_status & BLE_LOG_SPI_HD_DATA_DONE) {
            /* Data already on the wire: drop it from the buffer so the next
             * flush does not re-send these bytes (recycle keeps pos on purpose) */
            trans->pos = 0;
            ble_log_lbm_recycle_trans(trans);
        }
    }
}
