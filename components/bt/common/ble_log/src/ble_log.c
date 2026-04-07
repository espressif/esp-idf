/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ------- */
/* BLE Log */
/* ------- */

/* INCLUDE */
#include "ble_log.h"
#include "ble_log_rt.h"
#include "ble_log_lbm.h"
#include "ble_log_prph.h"
#include "ble_log_util.h"
#if CONFIG_BLE_LOG_TS_ENABLED
#include "ble_log_ts.h"
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

/* VARIABLE */
BLE_LOG_STATIC bool ble_log_inited = false;

/* INTERFACE */
bool ble_log_init(void)
{
    /* Avoid double init */
    if (ble_log_inited) {
        return true;
    }

#if CONFIG_BLE_LOG_TS_ENABLED
    /* Initialize BLE Log TS */
    if (!ble_log_ts_init()) {
        goto exit;
    }
#endif /* CONFIG_BLE_LOG_TS_ENABLED */

    /* Initialize BLE Log Runtime */
    if (!ble_log_rt_init()) {
        goto exit;
    }

    /* Initialize BLE Log LBM */
    if (!ble_log_lbm_init()) {
        goto exit;
    }

    /* Initialize BLE Log peripheral interface */
    if (!ble_log_prph_init(BLE_LOG_TRANS_TOTAL_CNT)) {
        goto exit;
    }

    /* Initialization done */
    ble_log_inited = true;
    ble_log_enable(true);

    /* Write initialization done log */
    ble_log_info_t ble_log_info = {
        .int_src_code = BLE_LOG_INT_SRC_INIT_DONE,
        .version = BLE_LOG_VERSION,
    };
    ble_log_write_hex(BLE_LOG_SRC_INTERNAL, (const uint8_t *)&ble_log_info, sizeof(ble_log_info_t));
    return true;

exit:
    ble_log_deinit();
    return false;
}

void ble_log_deinit(void)
{
    ble_log_enable(false);
    ble_log_inited = false;

    /* CRITICAL — Deinit ordering rationale:
     *
     * 1. Runtime task must be stopped FIRST to prevent it from sending
     *    transports to an already-destroyed peripheral driver. The queue
     *    is drained and pending transports are discarded.
     *
     * 2. Peripheral interface is deinitialized SECOND. It waits for any
     *    in-flight DMA operations (started before the task was killed) to
     *    complete, then destroys the driver. This is safe because no new
     *    DMA operations can be started (the task is already dead).
     *
     * 3. LBM is deinitialized LAST. At this point all DMA has completed
     *    (ensured by step 2) and all queued transports have been drained
     *    (ensured by step 1), so freeing the buffers is safe. */
    ble_log_rt_deinit();
    ble_log_prph_deinit();
    ble_log_lbm_deinit();

#if CONFIG_BLE_LOG_TS_ENABLED
    /* Deinitialize BLE Log TS */
    ble_log_ts_deinit();
#endif /* CONFIG_BLE_LOG_TS_ENABLED */
}
