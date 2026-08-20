/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ----------------- */
/* BLE Log - Runtime */
/* ----------------- */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_rt.h"
#include "ble_log_lbm.h"

#include "esp_log.h"

/* MACRO */
#define TAG                                      "ble_log_rt"

/* VARIABLE */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t rt_inited = 0;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR volatile uint32_t rt_ref_count = 0;
BLE_LOG_STATIC TaskHandle_t rt_task_handle = NULL;
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR QueueHandle_t rt_queue_handle = NULL;
#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR uint32_t rt_ts_enabled = 0;
BLE_LOG_STATIC esp_timer_handle_t rt_ts_timer = NULL;
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* PRIVATE FUNCTION DECLARATION */
#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC void ble_log_rt_task(void *pvParameters);
BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg);
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* PRIVATE FUNCTION */
BLE_LOG_STATIC void ble_log_rt_task(void *pvParameters)
{
    (void)pvParameters;
    ble_log_prph_trans_t *trans = NULL;
    uint32_t curr_os_ts = 0;
    uint32_t last_hook_os_ts = 0;
    while (1)
    {
        /* CRITICAL:
         * Blocking queue receive is mandatory for light sleep support */
        if (xQueueReceive(rt_queue_handle, &trans, portMAX_DELAY) == pdTRUE) {
            ble_log_prph_send_trans(trans);
        }

        /* Task hook */
        curr_os_ts = pdTICKS_TO_MS(xTaskGetTickCount());
        if ((curr_os_ts - last_hook_os_ts) < BLE_LOG_TS_TRIGGER_TIMEOUT_MS) {
            continue;
        }
        last_hook_os_ts = curr_os_ts;

        /* Write BLE Log info log */
        ble_log_info_t ble_log_info = {
            .int_src_code = BLE_LOG_INT_SRC_INFO,
            .version = BLE_LOG_VERSION,
        };
        ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&ble_log_info, sizeof(ble_log_info_t));

        ble_log_write_enh_stat();
        ble_log_write_buf_util();
    }
}

#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC void ble_log_rt_ts_trigger(void *arg)
{
    (void)arg;
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited) ||
        !BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_ts_enabled)) {
        return;
    }
    ble_log_ts_info_t *ts_info = NULL;
    ble_log_ts_info_update(&ts_info);
    if (ts_info) {
        ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)ts_info, sizeof(ble_log_ts_info_t));
    }
}
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* INTERFACE */
bool ble_log_rt_init(void)
{
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return true;
    }

    /* CRITICAL:
     * Queue must be initialized before creating task */
    rt_queue_handle = xQueueCreate(BLE_LOG_TRANS_TOTAL_CNT, sizeof(ble_log_prph_trans_t *));
    if (!rt_queue_handle) {
        goto exit;
    }

    /* Initialize task */
    if (xTaskCreate(ble_log_rt_task, "ble_log", BLE_LOG_TASK_STACK_SIZE, NULL,
        BLE_LOG_TASK_PRIO, &rt_task_handle) != pdTRUE) {
        goto exit;
    }

#if CONFIG_BLE_LOG_TS_ENABLED
    BLE_LOG_ATOMIC_STORE_RELAXED(rt_ts_enabled, false);
    /* Initialize ESP Timer Trigger */
    esp_timer_create_args_t ts_timer_args = {
        .callback = ble_log_rt_ts_trigger,
        .arg = NULL,
        .name = "ble_log_ts_timer",
        .skip_unhandled_events = true,
    };
    if (esp_timer_create(&ts_timer_args, &rt_ts_timer) != ESP_OK) {
        goto exit;
    }
    if (esp_timer_start_periodic(rt_ts_timer, BLE_LOG_TS_TRIGGER_TIMEOUT_MS * 1000) != ESP_OK) {
        goto exit;
    }
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

    BLE_LOG_ATOMIC_STORE_RELEASE(rt_inited, true);
    return true;

exit:
    ble_log_rt_deinit();
    return false;
}

void ble_log_rt_deinit(void)
{
    /* Closing gate: seq_cst on both sides (see also submit) so a submitter
     * either sees rt_inited == false and bails, or its reference is visible
     * to the ref-count wait before the task and queue are torn down. */
    BLE_LOG_ATOMIC_STORE_SEQ_CST(rt_inited, false);
    while (!ble_log_ref_count_wait(&rt_ref_count, 0)) {
        ESP_LOGE(TAG, "Timed out waiting for BLE Log runtime references");
        BLE_LOG_ASSERT(false);
    }
#if CONFIG_BLE_LOG_TS_ENABLED
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_ts_enabled, false);
    if (rt_ts_timer) {
        esp_timer_stop_blocking(rt_ts_timer, portMAX_DELAY);
        esp_timer_delete(rt_ts_timer);
        rt_ts_timer = NULL;
    }
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

    /* CRITICAL:
     * Task must be deinitialized before deinitializing queue */
    if (rt_task_handle) {
        vTaskDelete(rt_task_handle);
        rt_task_handle = NULL;
    }

    /* Drain remaining queue items to clean up transport state */
    if (rt_queue_handle) {
        ble_log_prph_trans_t *trans = NULL;
        while (xQueueReceive(rt_queue_handle, &trans, 0) == pdTRUE) {
            trans->pos = 0;
            ble_log_lbm_recycle_trans(trans);
        }
        vQueueDelete(rt_queue_handle);
        rt_queue_handle = NULL;
    }
}

BLE_LOG_IRAM_ATTR void ble_log_rt_submit_trans(ble_log_prph_trans_t *trans)
{
    if (!ble_log_ref_count_try_acquire(&rt_ref_count, &rt_inited)) {
        ble_log_lbm_recycle_trans(trans);
        return;
    }
    /* Queue depth == total transport buffer count, so a timeout-0 send cannot
     * fail for a valid transport; recycling on failure keeps submitters from
     * blocking while they hold a lifetime reference. */
    BaseType_t queued = BLE_LOG_IN_ISR()
                        ? xQueueSendFromISR(rt_queue_handle, &trans, NULL)
                        : xQueueSend(rt_queue_handle, &trans, 0);
    if (queued != pdTRUE) {
        BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
        ble_log_lbm_recycle_trans(trans);
        return;
    }
    BLE_LOG_REF_COUNT_RELEASE(&rt_ref_count);
}

#if CONFIG_BLE_LOG_TS_ENABLED
bool ble_log_sync_enable(bool enable)
{
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(rt_inited)) {
        return false;
    }
    BLE_LOG_ATOMIC_STORE_RELEASE(rt_ts_enabled, enable);
    ble_log_ts_reset(enable);
    return true;
}
#endif /* CONFIG_BLE_LOG_TS_ENABLED */
