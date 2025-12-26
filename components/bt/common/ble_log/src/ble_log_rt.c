/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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

/* VARIABLE */
BLE_LOG_STATIC bool rt_inited = false;
BLE_LOG_STATIC TaskHandle_t rt_task_handle = NULL;
BLE_LOG_STATIC QueueHandle_t rt_queue_handle = NULL;
#if CONFIG_BLE_LOG_TS_ENABLED
BLE_LOG_STATIC bool rt_ts_enabled = false;
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* PRIVATE FUNCTION */
BLE_LOG_IRAM_ATTR BLE_LOG_STATIC void ble_log_rt_task(void *pvParameters)
{
    (void)pvParameters;
    ble_log_prph_trans_t *trans = NULL;
    uint32_t curr_os_ts = 0;
    uint32_t last_hook_os_ts = 0;
#ifndef UNIT_TEST
    while (1)
#endif /* !UNIT_TEST */
    {
        /* CRITICAL:
         * Blocking queue receive is mandatory for light sleep support */
        if (xQueueReceive(rt_queue_handle, &trans, portMAX_DELAY) == pdTRUE) {
            ble_log_prph_send_trans(trans);
        }

        /* Task hook */
        curr_os_ts = pdTICKS_TO_MS(xTaskGetTickCount());
        if ((curr_os_ts - last_hook_os_ts) < BLE_LOG_TASK_HOOK_TIMEOUT_MS) {
#ifndef UNIT_TEST
            continue;
#else /* UNIT_TEST */
            return;
#endif /* !UNIT_TEST */
        }
        last_hook_os_ts = curr_os_ts;

        /* Write BLE Log info log */
        ble_log_info_t ble_log_info = {
            .int_src_code = BLE_LOG_INT_SRC_INFO,
            .version = BLE_LOG_VERSION,
        };
        ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&ble_log_info, sizeof(ble_log_info_t));

#if CONFIG_BLE_LOG_TS_ENABLED
        if (rt_ts_enabled) {
            ble_log_ts_info_t *ts_info = NULL;
            ble_log_ts_info_update(&ts_info);
            if (ts_info) {
                ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)ts_info, sizeof(ble_log_ts_info_t));
            }
        }
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

#if CONFIG_BLE_LOG_ENH_STAT_ENABLED
        ble_log_write_enh_stat();
#endif /* CONFIG_BLE_LOG_ENH_STAT_ENABLED */
    }
}

/* INTERFACE */
bool ble_log_rt_init(void)
{
    if (rt_inited) {
        return true;
    }

    /* CRITICAL:
     * Queue must be initialized before creating task */
    rt_queue_handle = xQueueCreate(BLE_LOG_LBM_CNT, sizeof(ble_log_prph_trans_t *));
    if (!rt_queue_handle) {
        goto exit;
    }

    /* Initialize task */
    if (xTaskCreate(ble_log_rt_task, "ble_log", BLE_LOG_TASK_STACK_SIZE, NULL,
        BLE_LOG_TASK_PRIO, &rt_task_handle) != pdTRUE) {
        goto exit;
    }

    rt_inited = true;
#if CONFIG_BLE_LOG_TS_ENABLED
    rt_ts_enabled = false;
#endif /* CONFIG_BLE_LOG_TS_ENABLED */
    return true;

exit:
    ble_log_rt_deinit();
    return false;
}

void ble_log_rt_deinit(void)
{
    rt_inited = false;
#if CONFIG_BLE_LOG_TS_ENABLED
    rt_ts_enabled = false;
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

    /* CRITICAL:
     * Task must be deinitialized before deinitializing queue */
    if (rt_task_handle) {
        vTaskDelete(rt_task_handle);
        rt_task_handle = NULL;
    }

    /* Release task queue */
    if (rt_queue_handle) {
        vQueueDelete(rt_queue_handle);
        rt_queue_handle = NULL;
    }
}

BLE_LOG_IRAM_ATTR void ble_log_rt_queue_trans(ble_log_prph_trans_t **trans)
{
    (*trans)->prph_owned = true;
    if (BLE_LOG_IN_ISR()) {
        xQueueSendFromISR(rt_queue_handle, trans, NULL);
    } else {
        xQueueSend(rt_queue_handle, trans, portMAX_DELAY);
    }
}

#if CONFIG_BLE_LOG_TS_ENABLED
bool ble_log_sync_enable(bool enable)
{
    if (!rt_inited) {
        return false;
    }
    rt_ts_enabled = enable;
    ble_log_ts_reset(enable);
    return true;
}
#endif /* CONFIG_BLE_LOG_TS_ENABLED */
