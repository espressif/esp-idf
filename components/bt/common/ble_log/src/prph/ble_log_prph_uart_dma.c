/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ----------------------------------------- */
/* BLE Log - Peripheral Interface - UART DMA */
/* ----------------------------------------- */

/* INCLUDE */
#include "ble_log_prph_uart_dma.h"
#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_redir.h"
#include "ble_log_rt.h"

#if BLE_LOG_PRPH_UART_DMA_REDIR

#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "freertos/task.h"
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* MACRO */
#define BLE_LOG_UART_MAX_TRANSFER_SIZE      (10240)
#define BLE_LOG_UART_RX_BUF_SIZE            (256)
/* ponytail: data burst disabled - UHCI enforces burst-size alignment (addr+len) on
 * uhci_transmit() once GDMA weighted arbitration is enabled, and UART log bandwidth
 * is baud-rate limited anyway, so burst buys nothing here */
#define BLE_LOG_UART_DMA_BURST_SIZE         (1)
#define BLE_LOG_UART_FLUSH_TIMEOUT_TICKS    pdMS_TO_TICKS(1000)
#if BLE_LOG_PRPH_UART_DMA_REDIR
#define BLE_LOG_UART_REDIR_BUF_SIZE         (512)
#define BLE_LOG_UART_REDIR_FLUSH_PERIOD_US  (1000 * 1000)
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* VARIABLE */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool prph_inited = false;
BLE_LOG_STATIC uhci_controller_handle_t dev_handle = NULL;
#if BLE_LOG_PRPH_UART_DMA_REDIR
BLE_LOG_STATIC bool uart_driver_inited = false;
BLE_LOG_STATIC ble_log_redir_t *redir_lbm = NULL;
BLE_LOG_STATIC esp_timer_handle_t redir_flush_timer = NULL;
BLE_LOG_STATIC volatile uint32_t redir_writer_count = 0;
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* PRIVATE FUNCTION DECLARATION */
BLE_LOG_STATIC bool uart_dma_tx_done_cb(
    uhci_controller_handle_t uhci_ctrl, const uhci_tx_done_event_data_t *edata, void *user_ctx);

/* PRIVATE FUNCTION */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC bool uart_dma_tx_done_cb(
    uhci_controller_handle_t uhci_ctrl, const uhci_tx_done_event_data_t *edata, void *user_ctx)
{
    /* Unused arguments */
    (void)uhci_ctrl;
    (void)user_ctx;

    /* Recycle transport */
    ble_log_prph_trans_ctx_t *uart_trans_ctx = (ble_log_prph_trans_ctx_t *)(
                                                   (uint8_t *)edata->buffer - sizeof(ble_log_prph_trans_ctx_t)
                                               );
    ble_log_prph_trans_t *trans = uart_trans_ctx->trans;
    ble_log_lbm_recycle_trans(trans);
    return true;
}

#if BLE_LOG_PRPH_UART_DMA_REDIR
BLE_LOG_STATIC void esp_timer_cb_flush_log(void *arg)
{
    (void)arg;

    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(prph_inited) ||
            !ble_log_lbm_is_enabled()) {
        return;
    }

    /* Non-blocking trylock: skip if mutex is held by a writer.
     * The periodic timer will retry on the next tick.
     * stream_flush is a no-op when buffer is empty. */
    if (xSemaphoreTake(redir_lbm->mutex, 0) == pdTRUE) {
        ble_log_lbm_stream_flush(redir_lbm, BLE_LOG_SRC_REDIR);
        xSemaphoreGive(redir_lbm->mutex);
    }
}
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt)
{
    /* Avoid double init */
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(prph_inited)) {
        return true;
    }

    /* Initialize UART */
    uart_config_t uart_config = {
        .baud_rate = CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
    };
    if ((uart_param_config(CONFIG_BLE_LOG_PRPH_UART_DMA_PORT, &uart_config) != ESP_OK) ||
            (uart_set_pin(CONFIG_BLE_LOG_PRPH_UART_DMA_PORT,
                          CONFIG_BLE_LOG_PRPH_UART_DMA_TX_IO_NUM, -1, -1, -1) != ESP_OK)) {
        goto exit;
    }

    /* Initialize UHCI */
    uhci_controller_config_t uhci_config = {
        .uart_port = CONFIG_BLE_LOG_PRPH_UART_DMA_PORT,
        .tx_trans_queue_depth = trans_cnt,
        .max_receive_internal_mem = BLE_LOG_UART_RX_BUF_SIZE,
        .max_transmit_size = BLE_LOG_UART_MAX_TRANSFER_SIZE,
        .dma_burst_size = BLE_LOG_UART_DMA_BURST_SIZE,
        .rx_eof_flags.idle_eof = 1,
    };
    uhci_event_callbacks_t uhci_cbs = {
        .on_tx_trans_done = uart_dma_tx_done_cb,
    };
    if ((uhci_new_controller(&uhci_config, &dev_handle) != ESP_OK) ||
            (uhci_register_event_callbacks(dev_handle, &uhci_cbs, NULL) != ESP_OK)) {
        goto exit;
    }

    /* Redirection is required when utilizing UART port 0 */
#if BLE_LOG_PRPH_UART_DMA_REDIR
    /* Initialize a dedicated redirection manager (separate from the pool) */
    redir_lbm = (ble_log_redir_t *)BLE_LOG_MALLOC(sizeof(ble_log_redir_t));
    if (!redir_lbm) {
        goto exit;
    }
    BLE_LOG_MEMSET(redir_lbm, 0, sizeof(ble_log_redir_t));

    /* Transport initialization */
    for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
        if (!ble_log_prph_trans_init(&(redir_lbm->trans[i]),
                                     BLE_LOG_UART_REDIR_BUF_SIZE)) {
            goto exit;
        }
        /* Redirection transports are not part of the global pool. */
        redir_lbm->trans[i]->id = BLE_LOG_TRANS_ID_NONE;
        redir_lbm->trans[i]->owner_kind = BLE_LOG_TRANS_OWNER_REDIR;
        redir_lbm->trans[i]->state = BLE_LOG_TRANS_STATE_FREE;
    }

    /* Mutex initialization */
    redir_lbm->mutex = xSemaphoreCreateMutex();
    if (!redir_lbm->mutex) {
        goto exit;
    }

    /* Initialize UART driver for redirection. */
    if (!uart_is_driver_installed(UART_NUM_0)) {
        if (uart_driver_install(UART_NUM_0, BLE_LOG_UART_RX_BUF_SIZE,
                                0, 0, NULL, 0) != ESP_OK) {
            goto exit;
        }
        uart_driver_inited = true;
    }
    uart_vfs_dev_use_driver(UART_NUM_0);

    /* Initialize periodic flush timer */
    esp_timer_create_args_t timer_args = {
        .callback = esp_timer_cb_flush_log,
        .dispatch_method = ESP_TIMER_TASK,
        .skip_unhandled_events = true,
    };
    if (esp_timer_create(&timer_args, &redir_flush_timer) != ESP_OK) {
        goto exit;
    }
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    BLE_LOG_ATOMIC_STORE_RELEASE(prph_inited, true);
#if BLE_LOG_PRPH_UART_DMA_REDIR
    if (esp_timer_start_periodic(redir_flush_timer,
                                 BLE_LOG_UART_REDIR_FLUSH_PERIOD_US) != ESP_OK) {
        goto exit;
    }
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    return true;

exit:
    ble_log_prph_deinit();
    return false;
}

void ble_log_prph_deinit(void)
{
    __atomic_store_n(&prph_inited, false, __ATOMIC_SEQ_CST);

#if BLE_LOG_PRPH_UART_DMA_REDIR
    /* Release flush timer */
    if (redir_flush_timer) {
        esp_timer_stop_blocking(redir_flush_timer, portMAX_DELAY);
        esp_timer_delete(redir_flush_timer);
        redir_flush_timer = NULL;
    }

    while (__atomic_load_n(&redir_writer_count, __ATOMIC_SEQ_CST) > 0) {
        vTaskDelay(1);
    }

    /* Flush redirection buffers before waiting for all submitted DMA. */
    if (redir_lbm) {
        if (redir_lbm->mutex) {
            xSemaphoreTake(redir_lbm->mutex, portMAX_DELAY);
            ble_log_lbm_stream_flush(redir_lbm, BLE_LOG_SRC_REDIR);
            xSemaphoreGive(redir_lbm->mutex);
        }
    }
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    if (dev_handle) {
        uhci_wait_all_tx_transaction_done(dev_handle, portMAX_DELAY);
    }

#if BLE_LOG_PRPH_UART_DMA_REDIR
    /* Restore the VFS before deleting a driver installed by this module. */
    if (uart_driver_inited) {
        uart_vfs_dev_use_nonblocking(UART_NUM_0);
        uart_driver_delete(UART_NUM_0);
        uart_driver_inited = false;
    }

    /* Release redirection LBM only after DMA callbacks have completed. */
    if (redir_lbm) {
        if (redir_lbm->mutex) {
            vSemaphoreDelete(redir_lbm->mutex);
        }

        /* Release transport */
        for (int i = 0; i < BLE_LOG_TRANS_BUF_CNT; i++) {
            ble_log_prph_trans_deinit(&(redir_lbm->trans[i]));
        }

        /* Release LBM */
        BLE_LOG_FREE(redir_lbm);
        redir_lbm = NULL;
    }
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    if (dev_handle) {
        uhci_del_controller(dev_handle);
        dev_handle = NULL;
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
    size_t trans_ctx_size = sizeof(ble_log_prph_trans_ctx_t) + trans_size;
    ble_log_prph_trans_ctx_t *uart_trans_ctx = (ble_log_prph_trans_ctx_t *)BLE_LOG_MALLOC(trans_ctx_size);
    if (!uart_trans_ctx) {
        goto exit;
    }
    BLE_LOG_MEMSET(uart_trans_ctx, 0, trans_ctx_size);

    /* Log buffer linking */
    (*trans)->ctx = (void *)uart_trans_ctx;
    (*trans)->buf = (uint8_t *)uart_trans_ctx->trans_buf;
    uart_trans_ctx->trans = *trans;
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
    if (uhci_transmit(dev_handle, trans->buf, trans->pos) != ESP_OK) {
        /* No tx_done will fire on failure: recycle here to avoid leaking. */
        ble_log_lbm_recycle_trans(trans);
    }
}

/* Redirection is required when utilizing UART port 0 */
#if BLE_LOG_PRPH_UART_DMA_REDIR
BLE_LOG_STATIC
bool ble_log_redir_uart_tx_chars(const char *src, size_t len)
{
    __atomic_add_fetch(&redir_writer_count, 1, __ATOMIC_SEQ_CST);
    if (!__atomic_load_n(&prph_inited, __ATOMIC_SEQ_CST) ||
            !ble_log_lbm_is_enabled()) {
        __atomic_sub_fetch(&redir_writer_count, 1, __ATOMIC_SEQ_CST);
        return false;
    }

    if (BLE_LOG_IN_ISR() || xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
        __atomic_sub_fetch(&redir_writer_count, 1, __ATOMIC_SEQ_CST);
        return true;
    }
    xSemaphoreTake(redir_lbm->mutex, portMAX_DELAY);
    ble_log_lbm_stream_write(redir_lbm, BLE_LOG_SRC_REDIR,
                             (const uint8_t *)src, len);
    xSemaphoreGive(redir_lbm->mutex);
    __atomic_sub_fetch(&redir_writer_count, 1, __ATOMIC_SEQ_CST);
    return true;
}

int __real_uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len);
int __wrap_uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len)
{
    if ((uart_num != UART_NUM_0) ||
            !ble_log_redir_uart_tx_chars(buffer, len)) {
        return __real_uart_tx_chars(uart_num, buffer, len);
    }
    return len;
}

int __real_uart_write_bytes(uart_port_t uart_num, const void *src, size_t size);
int __wrap_uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
{
    if ((uart_num != UART_NUM_0) ||
            !ble_log_redir_uart_tx_chars(src, size)) {
        return __real_uart_write_bytes(uart_num, src, size);
    }
    return size;
}

int __real_uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len);
int __wrap_uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len)
{
    if ((uart_num != UART_NUM_0) ||
            !ble_log_redir_uart_tx_chars(src, size)) {
        return __real_uart_write_bytes_with_break(uart_num, src, size, brk_len);
    }
    return size;
}

BLE_LOG_IRAM_ATTR ble_log_redir_t *ble_log_prph_get_redir_lbm(void)
{
    return redir_lbm;
}
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

#if BLE_LOG_PRPH_UART_DMA_REDIR
bool ble_log_prph_flush(void)
{
    while (__atomic_load_n(&redir_writer_count, __ATOMIC_SEQ_CST) > 0) {
        vTaskDelay(1);
    }
    if (!redir_lbm) {
        return true;
    }

    xSemaphoreTake(redir_lbm->mutex, portMAX_DELAY);
    ble_log_lbm_stream_flush(redir_lbm, BLE_LOG_SRC_REDIR);
    xSemaphoreGive(redir_lbm->mutex);
    (void)ble_log_rt_drain();

    TickType_t start_tick = xTaskGetTickCount();
    while (BLE_LOG_ATOMIC_LOAD_ACQUIRE(redir_lbm->inflight) > 0) {
        if ((xTaskGetTickCount() - start_tick) >= BLE_LOG_UART_FLUSH_TIMEOUT_TICKS) {
            return false;
        }
        vTaskDelay(1);
    }
    return true;
}

void ble_log_prph_reset_util_counters(void)
{
    if (redir_lbm) {
        uint32_t inflight = BLE_LOG_ATOMIC_LOAD_RELAXED(redir_lbm->inflight);
        BLE_LOG_ATOMIC_STORE_RELAXED(redir_lbm->inflight_peak, inflight);
    }
}
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */
