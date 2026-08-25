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
#include "freertos/task.h"

/* VARIABLE */
static QueueHandle_t s_pending_trans;
static SemaphoreHandle_t s_tx_done;
static esp_timer_handle_t s_tx_timer;
static int64_t s_tx_deadline_us;
static uint32_t s_tx_rate;

typedef struct {
    ble_log_prph_test_auto_recycle_hook_t hook;
    void *ctx;
} ble_log_prph_test_hook_reg_t;

static ble_log_prph_test_hook_reg_t s_auto_recycle_slots[2];
static volatile uint32_t s_auto_recycle_idx;
static volatile uint32_t s_auto_recycle_busy;

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
    ble_log_prph_test_set_auto_recycle_hook(NULL, NULL);
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
    ble_log_prph_trans_ctx_t *ctx = trans->ctx;
    ctx->received_at_us = esp_timer_get_time();

    __atomic_fetch_add(&s_auto_recycle_busy, 1, __ATOMIC_ACQUIRE);
    uint32_t idx = __atomic_load_n(&s_auto_recycle_idx, __ATOMIC_ACQUIRE);
    ble_log_prph_test_hook_reg_t reg = s_auto_recycle_slots[idx & 1];
    if (reg.hook) {
        trans->pos = 0;
        ble_log_lbm_recycle_trans(trans);
        reg.hook(reg.ctx);
        __atomic_fetch_sub(&s_auto_recycle_busy, 1, __ATOMIC_RELEASE);
        return;
    }
    __atomic_fetch_sub(&s_auto_recycle_busy, 1, __ATOMIC_RELEASE);

    if (xQueueSend(s_pending_trans, &trans, 0) != pdTRUE) {
        trans->pos = 0;
        ble_log_lbm_recycle_trans(trans);
        BLE_LOG_ASSERT(false);
    }
}

void ble_log_prph_test_set_auto_recycle_hook(ble_log_prph_test_auto_recycle_hook_t hook,
                                             void *ctx)
{
    while (__atomic_load_n(&s_auto_recycle_busy, __ATOMIC_ACQUIRE) != 0) {
        vTaskDelay(1);
    }

    uint32_t next = !__atomic_load_n(&s_auto_recycle_idx, __ATOMIC_RELAXED);
    s_auto_recycle_slots[next].hook = hook;
    s_auto_recycle_slots[next].ctx = ctx;
    __atomic_store_n(&s_auto_recycle_idx, next, __ATOMIC_RELEASE);

    if (!hook) {
        while (__atomic_load_n(&s_auto_recycle_busy, __ATOMIC_ACQUIRE) != 0) {
            vTaskDelay(1);
        }
    }
}

size_t ble_log_prph_test_read(uint8_t *data, size_t len, TickType_t timeout,
                              uint32_t bytes_per_second, int64_t *received_at_us)
{
    if (received_at_us) {
        *received_at_us = 0;
    }
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

    if (received_at_us) {
        ble_log_prph_trans_ctx_t *ctx = trans->ctx;
        *received_at_us = ctx->received_at_us;
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
