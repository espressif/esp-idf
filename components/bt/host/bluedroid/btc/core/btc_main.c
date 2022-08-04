/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "btc/btc_task.h"
#include "btc/btc_main.h"
#include "btc/btc_dm.h"
#include "osi/future.h"
#include "esp_err.h"
#include "btc/btc_config.h"
#include "osi/alarm.h"
#include "btc/btc_ble_storage.h"
#include "btc_gap_ble.h"
#include "bta_gattc_int.h"
#include "bta_gatts_int.h"
#include "bta_dm_int.h"

static future_t *main_future[BTC_MAIN_FUTURE_NUM];

extern int bte_main_boot_entry(void *cb);
extern int bte_main_shutdown(void);

future_t **btc_main_get_future_p(btc_main_future_type_t type)
{
    return &main_future[type];
}

static void btc_enable_bluetooth(void)
{
    if (BTA_EnableBluetooth(btc_dm_sec_evt) != BTA_SUCCESS) {
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_FAIL);
    }
}

static void btc_disable_bluetooth(void)
{
#if (SMP_INCLUDED)
    btc_config_shut_down();
#endif
    if (BTA_DisableBluetooth() != BTA_SUCCESS) {
        future_ready(*btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE), FUTURE_FAIL);
    }
}

void btc_init_callback(void)
{
    future_ready(*btc_main_get_future_p(BTC_MAIN_INIT_FUTURE), FUTURE_SUCCESS);
}

static void btc_init_bluetooth(void)
{
    osi_alarm_create_mux();
    osi_alarm_init();
    bte_main_boot_entry(btc_init_callback);
#if (SMP_INCLUDED)
    btc_config_init();

#if (BLE_INCLUDED == TRUE)
    //load the ble local key which has been stored in the flash
    btc_dm_load_ble_local_keys();
#endif  ///BLE_INCLUDED == TRUE
#endif /* #if (SMP_INCLUDED) */
#if BTA_DYNAMIC_MEMORY
    deinit_semaphore = xSemaphoreCreateBinary();
#endif /* #if BTA_DYNAMIC_MEMORY */
}


static void btc_deinit_bluetooth(void)
{
    /* Wait for the disable operation to complete */
#if BTA_DYNAMIC_MEMORY
    xSemaphoreTake(deinit_semaphore, BTA_DISABLE_DELAY / portTICK_PERIOD_MS);
#endif /* #if BTA_DYNAMIC_MEMORY */
    bta_dm_sm_deinit();
#if (GATTC_INCLUDED)
    bta_gattc_deinit();
#endif /* #if (GATTC_INCLUDED) */
#if (GATTS_INCLUDED)
    bta_gatts_deinit();
#endif /* #if (GATTS_INCLUDED) */
    bte_main_shutdown();
#if (SMP_INCLUDED)
    btc_config_clean_up();
#endif
    osi_alarm_deinit();
    osi_alarm_delete_mux();
    future_ready(*btc_main_get_future_p(BTC_MAIN_DEINIT_FUTURE), FUTURE_SUCCESS);
#if BTA_DYNAMIC_MEMORY
    vSemaphoreDelete(deinit_semaphore);
    deinit_semaphore = NULL;
#endif /* #if BTA_DYNAMIC_MEMORY */
}

void btc_main_call_handler(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s act %d\n", __func__, msg->act);

    switch (msg->act) {
    case BTC_MAIN_ACT_INIT:
        btc_init_bluetooth();
        break;
    case BTC_MAIN_ACT_DEINIT:
        btc_deinit_bluetooth();
        break;
    case BTC_MAIN_ACT_ENABLE:
        btc_enable_bluetooth();
        break;
    case BTC_MAIN_ACT_DISABLE:
        btc_disable_bluetooth();
        break;
    default:
        BTC_TRACE_ERROR("%s UNKNOWN ACT %d\n", __func__, msg->act);
        break;
    }
}
