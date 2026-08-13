/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ---------------------------------------------- */
/* BLE Log - Peripheral-specific Transport - Test */
/* ---------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph_test.h"
#include "ble_log_lbm.h"
#include "esp_timer.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

/* VARIABLE */
static QueueHandle_t s_pending_trans;
static SemaphoreHandle_t s_tx_done;
static esp_timer_handle_t s_tx_timer;
static int64_t s_tx_deadline_us;
static uint32_t s_tx_rate;

static void test_tx_done(void *arg)
{
    (void)arg;
    xSemaphoreGive(s_tx_done);
}

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt)
{
    if (s_pending_trans) {
        return true;
    }

    s_pending_trans = xQueueCreate(trans_cnt, sizeof(ble_log_prph_trans_t *));
    s_tx_done = xSemaphoreCreateBinary();
    const esp_timer_create_args_t timer_args = {
        .callback = test_tx_done,
        .name = "ble_log_test",
    };
    if (!s_pending_trans || !s_tx_done ||
            esp_timer_create(&timer_args, &s_tx_timer) != ESP_OK) {
        ble_log_prph_deinit();
        return false;
    }
    return true;
}

void ble_log_prph_deinit(void)
{
    s_tx_deadline_us = 0;
    s_tx_rate = 0;
    if (s_tx_timer) {
        esp_timer_stop(s_tx_timer);
        esp_timer_delete(s_tx_timer);
        s_tx_timer = NULL;
    }
    if (s_tx_done) {
        vSemaphoreDelete(s_tx_done);
        s_tx_done = NULL;
    }
    if (!s_pending_trans) {
        return;
    }

    ble_log_prph_trans_t *trans;
    while (xQueueReceive(s_pending_trans, &trans, 0) == pdTRUE) {
        ble_log_lbm_recycle_trans(trans);
    }
    vQueueDelete(s_pending_trans);
    s_pending_trans = NULL;
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
    ble_log_prph_trans_ctx_t *test_trans_ctx = (ble_log_prph_trans_ctx_t *)BLE_LOG_MALLOC(sizeof(ble_log_prph_trans_ctx_t));
    if (!test_trans_ctx) {
        goto exit;
    }
    BLE_LOG_MEMSET(test_trans_ctx, 0, sizeof(ble_log_prph_trans_ctx_t));
    (*trans)->ctx = (void *)test_trans_ctx;

    /* Initialize log buffer */
    (*trans)->buf = (uint8_t *)BLE_LOG_MALLOC(trans_size);
    if (!(*trans)->buf) {
        goto exit;
    }
    BLE_LOG_MEMSET((*trans)->buf, 0, trans_size);
    test_trans_ctx->trans_buf = (*trans)->buf;
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

/* Model DMA ownership transfer: the test peripheral owns the transaction
 * after enqueueing it. ble_log_prph_test_read() consumes and recycles it,
 * equivalent to a real peripheral's asynchronous tx_done callback. */
void ble_log_prph_send_trans(ble_log_prph_trans_t *trans)
{
    if (xQueueSend(s_pending_trans, &trans, 0) != pdTRUE) {
        trans->pos = 0;
        ble_log_lbm_recycle_trans(trans);
        BLE_LOG_ASSERT(false);
    }
}

size_t ble_log_prph_test_read(uint8_t *data, size_t len, TickType_t timeout,
                              uint32_t bytes_per_second)
{
    if (!data) {
        return 0;
    }

    ble_log_prph_trans_t *trans;
    if (xQueueReceive(s_pending_trans, &trans, timeout) != pdTRUE) {
        return 0;
    }

    if (bytes_per_second) {
        int64_t now_us = esp_timer_get_time();
        uint64_t tx_time_us = ((uint64_t)trans->pos * 1000000ULL +
                               bytes_per_second - 1) / bytes_per_second;
        if (!s_tx_deadline_us || s_tx_rate != bytes_per_second) {
            s_tx_deadline_us = now_us;
        }
        s_tx_rate = bytes_per_second;
        s_tx_deadline_us += (int64_t)tx_time_us;
        int64_t delay_us = s_tx_deadline_us - now_us;
        if (delay_us > 0 &&
                (esp_timer_start_once(s_tx_timer, (uint64_t)delay_us) != ESP_OK ||
                 xSemaphoreTake(s_tx_done, portMAX_DELAY) != pdTRUE)) {
            trans->pos = 0;
            ble_log_lbm_recycle_trans(trans);
            BLE_LOG_ASSERT(false);
            return 0;
        }
    } else {
        s_tx_deadline_us = 0;
        s_tx_rate = 0;
    }

    size_t copied = len < trans->pos ? len : trans->pos;
    BLE_LOG_MEMCPY(data, trans->buf, copied);
    trans->pos = 0;
    ble_log_lbm_recycle_trans(trans);
    if (uxQueueMessagesWaiting(s_pending_trans) == 0) {
        s_tx_deadline_us = 0;
    }
    return copied;
}
