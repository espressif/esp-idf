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
    if (!ble_log_prph_init(BLE_LOG_LBM_CNT)) {
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

    /* CRITICAL:
     * BLE Log peripheral interface must be deinitialized at first,
     * because there's a risky scenario that may cause severe peripheral
     * driver fault - if a log buffer is sent to peripheral driver, and
     * ble_log_deinit is called; in this case, if LBM is deinitialized
     * before peripheral interface, the log buffer may be freed before
     * peripheral driver completing tx, and the result would be faulty */
    ble_log_prph_deinit();

    /* Deinitialize BLE Log LBM */
    ble_log_lbm_deinit();

    /* Deinitialize BLE Log Runtime */
    ble_log_rt_deinit();

#if CONFIG_BLE_LOG_TS_ENABLED
    /* Deinitialize BLE Log TS */
    ble_log_ts_deinit();
#endif /* CONFIG_BLE_LOG_TS_ENABLED */
}
