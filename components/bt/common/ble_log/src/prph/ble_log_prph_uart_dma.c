/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ----------------------------------------- */
/* BLE Log - Peripheral Interface - UART DMA */
/* ----------------------------------------- */

/* INCLUDE */
#include "ble_log_prph_uart_dma.h"

#if BLE_LOG_PRPH_UART_DMA_REDIR
#include "ble_log.h"
#include "ble_log_rt.h"
#include "ble_log_lbm.h"

#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* MACRO */
#define BLE_LOG_UART_MAX_TRANSFER_SIZE      (10240)
#define BLE_LOG_UART_RX_BUF_SIZE            (32)
#define BLE_LOG_UART_DMA_BURST_SIZE         (32)
#if BLE_LOG_PRPH_UART_DMA_REDIR
#define BLE_LOG_UART_REDIR_BUF_SIZE         (512)
#define BLE_LOG_UART_REDIR_FLUSH_TIMEOUT    (100)
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* VARIABLE */
BLE_LOG_STATIC bool prph_inited = false;
BLE_LOG_STATIC uhci_controller_handle_t dev_handle = NULL;
#if BLE_LOG_PRPH_UART_DMA_REDIR
BLE_LOG_STATIC bool uart_driver_inited = false;
BLE_LOG_STATIC ble_log_lbm_t *redir_lbm = NULL;
BLE_LOG_STATIC uint32_t redir_last_write_ts = 0;
BLE_LOG_STATIC esp_timer_handle_t redir_flush_timer = NULL;
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
    trans->pos = 0;
    trans->prph_owned = false;
    return true;
}

#if BLE_LOG_PRPH_UART_DMA_REDIR
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void esp_timer_cb_flush_log(void)
{
    uint32_t os_ts = pdTICKS_TO_MS(xTaskGetTickCount());
    if ((os_ts - redir_last_write_ts) > BLE_LOG_UART_REDIR_FLUSH_TIMEOUT) {
        xSemaphoreTake(redir_lbm->mutex, portMAX_DELAY);
        int trans_idx = redir_lbm->trans_idx;
        for (int i = 0; i < BLE_LOG_TRANS_PING_PONG_BUF_CNT; i++) {
            ble_log_prph_trans_t **trans = &(redir_lbm->trans[trans_idx]);
            if (!(*trans)->prph_owned && (*trans)->pos) {
                ble_log_rt_queue_trans(trans);
            }
            trans_idx = !trans_idx;
        }
        xSemaphoreGive(redir_lbm->mutex);
    }
}
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt)
{
    /* Avoid double init */
    if (prph_inited) {
        return true;
    }

    /* Initialize UART */
    uart_config_t uart_config = {
        .baud_rate = CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
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
    /* Initialize a dedicated LBM for redirection */
    redir_lbm = (ble_log_lbm_t *)BLE_LOG_MALLOC(sizeof(ble_log_lbm_t));
    if (!redir_lbm) {
        goto exit;
    }
    BLE_LOG_MEMSET(redir_lbm, 0, sizeof(ble_log_lbm_t));

    /* Transport initialization */
    for (int i = 0; i < BLE_LOG_TRANS_PING_PONG_BUF_CNT; i++) {
        if (!ble_log_prph_trans_init(&(redir_lbm->trans[i]),
                                     BLE_LOG_UART_REDIR_BUF_SIZE)) {
            goto exit;
        }
    }

    /* Mutex initilaization */
    redir_lbm->mutex = xSemaphoreCreateMutex();
    if (!redir_lbm->mutex) {
        goto exit;
    }

    /* Initialize UART driver for redirection */
    if (!uart_is_driver_installed(UART_NUM_0)) {
        uart_driver_install(UART_NUM_0, BLE_LOG_UART_RX_BUF_SIZE, 0, 0, NULL, 0);
        uart_driver_inited = true;
    }
    uart_vfs_dev_use_driver(UART_NUM_0);

    /* Initialize periodic flush timer */
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_flush_log,
        .dispatch_method = ESP_TIMER_TASK,
    };
    if (esp_timer_create(&timer_args, &redir_flush_timer) != ESP_OK) {
        goto exit;
    }
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    prph_inited = true;
#if BLE_LOG_PRPH_UART_DMA_REDIR
    esp_timer_start_periodic(redir_flush_timer, BLE_LOG_UART_REDIR_FLUSH_TIMEOUT);
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    return true;

exit:
    ble_log_prph_deinit();
    return false;
}

void ble_log_prph_deinit(void)
{
    prph_inited = false;

#if BLE_LOG_PRPH_UART_DMA_REDIR
    /* Release flush timer */
    if (redir_flush_timer) {
        esp_timer_stop(redir_flush_timer);
        esp_timer_delete(redir_flush_timer);
        redir_flush_timer = NULL;
    }

    /* Delete UART driver if it's installed by current module */
    if (uart_driver_inited) {
        uart_driver_delete(UART_NUM_0);
    }

    /* Release redirection LBM */
    if (redir_lbm) {
        /* Release mutex */
        if (redir_lbm->mutex) {
            xSemaphoreTake(redir_lbm->mutex, portMAX_DELAY);
            xSemaphoreGive(redir_lbm->mutex);
            vSemaphoreDelete(redir_lbm->mutex);
        }

        /* Release transport */
        for (int i = 0; i < BLE_LOG_TRANS_PING_PONG_BUF_CNT; i++) {
            ble_log_prph_trans_deinit(&(redir_lbm->trans[i]));
        }

        /* Release LBM */
        BLE_LOG_FREE(redir_lbm);
        redir_lbm = NULL;
    }
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */

    if (dev_handle) {
        uhci_wait_all_tx_transaction_done(dev_handle, portMAX_DELAY);
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
        trans->prph_owned = false;
    }
}

/* Redirection is required when utilizing UART port 0 */
#if BLE_LOG_PRPH_UART_DMA_REDIR
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC
void ble_log_redir_uart_tx_chars(const char *src, size_t len)
{
    xSemaphoreTake(redir_lbm->mutex, portMAX_DELAY);
    ble_log_prph_trans_t **trans = ble_log_lbm_get_trans(redir_lbm, len);
    if (trans) {
        uint8_t *buf = (*trans)->buf + (*trans)->pos;
        BLE_LOG_MEMCPY(buf, src, len);
        (*trans)->pos += len;
        redir_last_write_ts = pdTICKS_TO_MS(xTaskGetTickCount());

        if (BLE_LOG_TRANS_FREE_SPACE((*trans)) <= BLE_LOG_FRAME_OVERHEAD) {
            ble_log_rt_queue_trans(trans);
        }
    }
    xSemaphoreGive(redir_lbm->mutex);
}

int __real_uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len);
int __wrap_uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len)
{
    if (!prph_inited || (uart_num != UART_NUM_0)) {
        return __real_uart_tx_chars(uart_num, buffer, len);
    }
    ble_log_redir_uart_tx_chars(buffer, len);
    return len;
}

int __real_uart_write_bytes(uart_port_t uart_num, const void *src, size_t size);
int __wrap_uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
{
    if (!prph_inited || (uart_num != UART_NUM_0)) {
        return __real_uart_write_bytes(uart_num, src, size);
    }
    ble_log_redir_uart_tx_chars(src, size);
    return size;
}

int __real_uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len);
int __wrap_uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len)
{
    if (!prph_inited || (uart_num != UART_NUM_0)) {
        return __real_uart_write_bytes_with_break(uart_num, src, size, brk_len);
    } else {
        (void)brk_len;
        return __wrap_uart_write_bytes(uart_num, src, size);
    }
}
#endif /* BLE_LOG_PRPH_UART_DMA_REDIR */
