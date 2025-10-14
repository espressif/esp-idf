/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ----------------------------------------------- */
/* BLE Log - Peripheral Interface - SPI Master DMA */
/* ----------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph_spi_master_dma.h"

#include "esp_timer.h"

/* MACRO */
#define BLE_LOG_SPI_BUS                     SPI2_HOST
#define BLE_LOG_SPI_MAX_TRANSFER_SIZE       (10240)
#define BLE_LOG_SPI_TRANS_ITVL_MIN_US       (30)

/* VARIABLE */
BLE_LOG_STATIC bool prph_inited = false;
BLE_LOG_STATIC spi_device_handle_t dev_handle = NULL;
BLE_LOG_STATIC uint32_t last_tx_done_ts = 0;

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC void spi_master_dma_tx_done_cb(spi_transaction_t *spi_trans);
BLE_LOG_STATIC void spi_master_dma_pre_tx_cb(spi_transaction_t *spi_trans);

/* PRIVATE FUNCTION */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void spi_master_dma_tx_done_cb(spi_transaction_t *spi_trans)
{
    /* SPI slave performance issue workaround */
    last_tx_done_ts = esp_timer_get_time();

    /* Recycle transport */
    ble_log_prph_trans_t *trans = (ble_log_prph_trans_t *)(spi_trans->user);
    trans->pos = 0;
    trans->prph_owned = false;
}

BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void spi_master_dma_pre_tx_cb(spi_transaction_t *spi_trans)
{
    /* SPI slave performance issue workaround */
    while ((esp_timer_get_time() - last_tx_done_ts) < BLE_LOG_SPI_TRANS_ITVL_MIN_US) {}
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
        .mosi_io_num = CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_MOSI_IO_NUM,
        .sclk_io_num = CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_SCLK_IO_NUM,
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

    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = SPI_MASTER_FREQ_20M,
        .mode = 0,
        .spics_io_num = CONFIG_BLE_LOG_PRPH_SPI_MASTER_DMA_CS_IO_NUM,
        .queue_size = trans_cnt,
        .post_cb = spi_master_dma_tx_done_cb,
        .pre_cb = spi_master_dma_pre_tx_cb,
        .flags = SPI_DEVICE_NO_RETURN_RESULT
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
        /* Drain all queued transactions */
        if (spi_device_acquire_bus(dev_handle, portMAX_DELAY) == ESP_OK) {
            spi_device_release_bus(dev_handle);
            spi_bus_remove_device(dev_handle);
            dev_handle = NULL;
        }
    }

    /* Note: We don't care if the bus has been inited or not */
    spi_bus_free(BLE_LOG_SPI_BUS);
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
    spi_trans_ctx->user = (void *)(*trans);
    (*trans)->ctx = (void *)spi_trans_ctx;

    /* Initialize log buffer */
    (*trans)->buf = (uint8_t *)BLE_LOG_MALLOC(trans_size);
    if (!(*trans)->buf) {
        goto exit;
    }
    BLE_LOG_MEMSET((*trans)->buf, 0, trans_size);
    spi_trans_ctx->tx_buffer = (const void *)(*trans)->buf;
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
    spi_transaction_t *spi_trans = (spi_transaction_t *)trans->ctx;

    /* CRITICAL:
     * Bytes to bits length conversion is required for tx, and rxlength must be
     * cleared regardless of whether it is used for rx as per SPI master driver */
    spi_trans->length = (trans->pos << 3);
    spi_trans->rxlength = 0;
    if (spi_device_queue_trans(dev_handle, spi_trans, 0) != ESP_OK) {
        trans->prph_owned = false;
    }
}
