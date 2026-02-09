/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
    bta_dm_co_security_param_init();
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
#if (GATTS_INCLUDED == TRUE)
    bta_gatts_deinit();
#endif /* #if (GATTS_INCLUDED == TRUE) */
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
    BTC_TRACE_DEBUG("%s act %d", __func__, msg->act);

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

uint32_t btc_get_ble_status(void)
{
    uint32_t status = BTC_BLE_STATUS_IDLE;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        BTC_TRACE_ERROR("%s Bluedroid not enabled", __func__);
        return status;
    }

#if (BLE_INCLUDED == TRUE)
    // Number of active advertising
    extern uint8_t btm_ble_adv_active_count(void);
    uint8_t adv_cnt = btm_ble_adv_active_count();
    if (adv_cnt) {
        BTC_TRACE_WARNING("%s advertising active, cnt %d", __func__, adv_cnt);
        status |= BIT(BTC_BLE_STATUS_ADV);
    }

    // Number of active scanning
    extern uint8_t btm_ble_scan_active_count(void);
    uint8_t scan_cnt = btm_ble_scan_active_count();
    if (scan_cnt) {
        BTC_TRACE_WARNING("%s scan active, cnt %d", __func__, scan_cnt);
        status |= BIT(BTC_BLE_STATUS_SCAN);
    }

    // Number of active GATT tcb
    extern uint8_t gatt_tcb_active_count(void);
    uint8_t gatt_tcb_cnt = gatt_tcb_active_count();
    if (gatt_tcb_cnt) {
        BTC_TRACE_WARNING("%s gatt tcb active, cnt %d", __func__, gatt_tcb_cnt);
        status |= BIT(BTC_BLE_STATUS_CONN);
    }

    // Number of active ACL connection
    extern uint8_t btm_ble_acl_active_count(void);
    uint8_t acl_cnt = btm_ble_acl_active_count();
    if (acl_cnt) {
        BTC_TRACE_WARNING("%s acl connection active, cnt %d", __func__, acl_cnt);
        status |= BIT(BTC_BLE_STATUS_CONN);
    }

    // Number of active L2C plcb
    extern uint8_t l2cu_ble_plcb_active_count(void);
    uint8_t plcb_cnt = l2cu_ble_plcb_active_count();
    if (plcb_cnt) {
        BTC_TRACE_WARNING("%s l2c plcb active, cnt %d", __func__, plcb_cnt);
        status |= BIT(BTC_BLE_STATUS_CONN);
    }

#if ((SMP_INCLUDED == TRUE) || (BLE_PRIVACY_SPT == TRUE))
    // Address resolve status
    extern uint8_t btm_get_ble_addr_resolve_disable_status(void);
    uint8_t addr_resolve_disable = btm_get_ble_addr_resolve_disable_status();
    if (addr_resolve_disable) {
        BTC_TRACE_WARNING("%s address resolve disabled", __func__);
        status |= BIT(BTC_BLE_STATUS_ADDR_RESOLVE_DISABLE);
    }
#endif // #if ((SMP_INCLUDED == TRUE) || (BLE_PRIVACY_SPT == TRUE))

#if (SMP_INCLUDED == TRUE)
    // Number of recorded devices
    extern uint8_t btm_ble_sec_dev_record_count(void);
    uint8_t sec_dev_cnt = btm_ble_sec_dev_record_count();
    if (sec_dev_cnt) {
        BTC_TRACE_WARNING("%s security device record count %d", __func__, sec_dev_cnt);
        status |= BIT(BTC_BLE_STATUS_DEVICE_REC);
    }

    // Number of saved bonded devices
    int bond_cnt = btc_storage_get_num_ble_bond_devices();
    if (bond_cnt) {
        BTC_TRACE_WARNING("%s bonded devices count %d", __func__, bond_cnt);
        status |= BIT(BTC_BLE_STATUS_BOND);
    }
#endif // SMP_INCLUDED

#if (BLE_PRIVACY_SPT == TRUE)
    // Privacy enabled
    extern uint8_t btm_ble_privacy_is_enabled(void);
    uint8_t privacy_en = btm_ble_privacy_is_enabled();
    if (privacy_en) {
        BTC_TRACE_WARNING("%s privacy enabled", __func__);
        status |= BIT(BTC_BLE_STATUS_PRIVACY);
    }
#endif // BLE_PRIVACY_SPT

#endif // BLE_INCLUDED

#if (BLE_50_EXTEND_ADV_EN == TRUE)
    // Number of active extended advertising
    extern uint8_t btm_ble_ext_adv_active_count(void);
    uint8_t ext_adv_cnt = btm_ble_ext_adv_active_count();
    if (ext_adv_cnt) {
        BTC_TRACE_WARNING("%s extended advertising active, cnt %d", __func__, ext_adv_cnt);
        status |= BIT(BTC_BLE_STATUS_EXT_ADV);
    }
#endif // BLE_50_EXTEND_ADV_EN

#if (GATTC_INCLUDED == TRUE)
    // Number of registered GATTC APP
    extern uint8_t bta_gattc_cl_rcb_active_count(void);
    uint8_t gattc_app_cnt = bta_gattc_cl_rcb_active_count();
    if (gattc_app_cnt) {
        BTC_TRACE_WARNING("%s GATTC app active, cnt %d", __func__, gattc_app_cnt);
        status |= BIT(BTC_BLE_STATUS_GATTC_APP);
    }

    // Number of saved GATTC cache
    extern UINT8 bta_gattc_co_get_addr_num(void);
    uint8_t gattc_cache_cnt = bta_gattc_co_get_addr_num();
    if (gattc_cache_cnt) {
        BTC_TRACE_WARNING("%s GATTC cache count %d", __func__, gattc_cache_cnt);
        status |= BIT(BTC_BLE_STATUS_GATTC_CACHE);
    }
#endif // GATTC_INCLUDED

#if (GATTS_INCLUDED == TRUE)
    // Number of registered GATTS service
    extern uint8_t bta_gatts_srvc_active_count(void);
    uint8_t gatts_srvc_cnt = bta_gatts_srvc_active_count();
    if (gatts_srvc_cnt) {
        BTC_TRACE_WARNING("%s GATTS service active, cnt %d", __func__, gatts_srvc_cnt);
        status |= BIT(BTC_BLE_STATUS_GATTS_SRVC);
    }
#endif // GATTS_INCLUDED

#if (SMP_INCLUDED == TRUE)
    extern uint8_t smp_get_state(void);
    uint8_t smp_state = smp_get_state();
    if (smp_state) {
        BTC_TRACE_WARNING("%s SMP state active, state=%d", __func__, smp_state);
        status |= BIT(BTC_BLE_STATUS_SMP_STATE);
    }
#endif // SMP_INCLUDED

    BTC_TRACE_WARNING("%s exit, final status=0x%x", __func__, status);

    return status;
}
