/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ------- */
/* BLE Log */
/* ------- */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_rt.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_prph.h"
#include "ble_log_util.h"
#include "esp_log.h"
#include "esp_system.h"

/* VARIABLE */
#define TAG "ble_log"

BLE_LOG_STATIC bool ble_log_inited = false;
BLE_LOG_STATIC bool shutdown_handler_registered = false;

BLE_LOG_STATIC void ble_log_shutdown_handler(void)
{
    ble_log_flush();
}

/* INTERFACE */
bool ble_log_init(void)
{
    /* Avoid double init */
    if (ble_log_inited) {
        return true;
    }

    /* Allocate pool and dedicated Internal transport before runtime starts. */
    if (!ble_log_lbm_init()) {
        goto exit;
    }

    if (!ble_log_prph_init(BLE_LOG_TRANS_TOTAL_CNT)) {
        goto exit;
    }

    if (!ble_log_rt_init()) {
        goto exit;
    }

    ble_log_inited = true;
    /* Queue the required INIT snapshot before starting the periodic path or
     * opening the public producer gate, so it starts the receiver epoch.
     * INIT/FLUSH samples never toggle sync IO. */
    ble_log_ts_info_t ts_info;
    ble_log_rt_ts_sample(&ts_info, false);
    if (!ble_log_internal_snapshot(BLE_LOG_SNAPSHOT_REASON_INIT, &ts_info, true) ||
            !ble_log_rt_start_periodic() || !ble_log_enable(true)) {
        goto exit;
    }
    esp_err_t ret = esp_register_shutdown_handler(ble_log_shutdown_handler);
    if (ret == ESP_OK) {
        shutdown_handler_registered = true;
    } else {
        ESP_LOGW(TAG, "Register shutdown handler failed, ret = 0x%x", ret);
    }

    return true;

exit:
    ble_log_deinit();
    return false;
}

void ble_log_deinit(void)
{
    if (shutdown_handler_registered) {
        esp_err_t ret = esp_unregister_shutdown_handler(ble_log_shutdown_handler);
        if (ret == ESP_OK) {
            shutdown_handler_registered = false;
        } else {
            ESP_LOGW(TAG, "Unregister shutdown handler failed, ret = 0x%x", ret);
        }
    }
    ble_log_inited = false;
    ble_log_lbm_begin_deinit();

    /* Seal and dispatch residual pool and UART0 REDIR data while the runtime
     * queue is still alive. The peripheral waits below complete delivery. */
    ble_log_lbm_drain_open_trans();
#if BLE_LOG_UART_REDIR_ENABLED
    (void)ble_log_prph_flush();
#endif

    /* CRITICAL - Deinit ordering rationale:
     *
     * 1. The LBM writer gate is closed before submodule teardown. Writers
     *    already inside the gate keep a reference until they finish; later
     *    writers are rejected. With writers gone, the pool and REDIR drains
     *    seal and dispatch every residual frame while runtime is still live.
     *
     * 2. Runtime dispatch is stopped FIRST to prevent it from sending
     *    transports to an already-destroyed peripheral driver. Active
     *    submissions and callbacks finish before the timers and queue are
     *    deleted.
     *
     * 3. Peripheral interface is deinitialized SECOND. It waits for DMA
     *    operations started before runtime dispatch stopped, then destroys
     *    the driver. No new DMA operations can start after runtime teardown.
     *
     * 4. LBM is deinitialized LAST. At this point all DMA has completed
     *    (ensured by step 3) and all queued transports have been drained
     *    (ensured by steps 1 and 2), so freeing the buffers is safe. */
    ble_log_rt_deinit();
    ble_log_prph_deinit();
    ble_log_lbm_deinit();
}
