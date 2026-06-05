/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#include "esp_ble_audio_common_api.h"

#include "cap_acceptor.h"

/* Shared with scan.c — single host_sem because init is serialized and only
 * peripheral.c owns the GAP cb registration. */
SemaphoreHandle_t cap_sem;

/* Controller status latched by gap_event_handler for EXAMPLE_WAIT_API_CHECK. */
esp_bt_status_t cap_op_status;

#define WAIT_API(_call) EXAMPLE_WAIT_API_CHECK(_call, cap_sem, portMAX_DELAY, cap_op_status)

static esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = ESP_BLE_GAP_ADV_ITVL_MS(ADV_INTERVAL_MS),
    .interval_max = ESP_BLE_GAP_ADV_ITVL_MS(ADV_INTERVAL_MS),
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = ADV_SID,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .tx_power = ADV_TX_POWER,
};

static esp_ble_gap_ext_adv_t ext_adv_inst[1] = {
    [0] = { ADV_HANDLE, 0, 0 },
};

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    /* Adv-side completion */
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        cap_op_status = param->ext_adv_set_params.status;
        xSemaphoreGive(cap_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        cap_op_status = param->ext_adv_data_set.status;
        xSemaphoreGive(cap_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        cap_op_status = param->ext_adv_start.status;
        xSemaphoreGive(cap_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        cap_op_status = param->ext_adv_stop.status;
        xSemaphoreGive(cap_sem);
        break;
    /* Scan-side completion */
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        cap_op_status = param->set_ext_scan_params.status;
        xSemaphoreGive(cap_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        cap_op_status = param->ext_scan_start.status;
        xSemaphoreGive(cap_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        cap_op_status = param->ext_scan_stop.status;
        xSemaphoreGive(cap_sem);
        break;
    /* PAST params setup — pa_sync_with_past / pa_past_cancel wait via WAIT_API.
     * CREATE_SYNC / SYNC_TERMINATE complete events are intentionally
     * omitted — those calls are fire-and-forget (sync est/lost is air-
     * dependent and surfaces via PA_SYNC_ESTAB / PA_SYNC_LOST). Giving
     * the sem here would leave it stale-available for the next WAIT_API. */
    case ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT:
        cap_op_status = param->set_past_params.status;
        xSemaphoreGive(cap_sem);
        break;

    /* SMP request handling for Just Works pairing (IO_CAP=NONE). The peer
     * (central) initiates; we accept the security request and confirm the
     * numeric comparison. */
    case ESP_GAP_BLE_SEC_REQ_EVT:
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        break;

    /* AUTH_CMPL has no BTA channel — app must forward it. PA sync / ext adv
     * events: already forwarded by adapter's BTA path, don't re-post here. */
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        esp_ble_audio_gap_app_post_event(event, param);
        break;

    default:
        break;
    }
}

int app_host_init(void)
{
    esp_err_t err;

    cap_sem = xSemaphoreCreateBinary();
    if (cap_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create host semaphore");
        return -1;
    }

    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err) {
        ESP_LOGE(TAG, "Failed to register GAP callback, err %d", err);
        vSemaphoreDelete(cap_sem);
        return err;
    }

    return 0;
}

int set_device_name(void)
{
    return esp_ble_gap_set_device_name(LOCAL_DEVICE_NAME);
}

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len)
{
    WAIT_API(esp_ble_gap_ext_adv_set_params(ADV_HANDLE, &ext_adv_params));
    WAIT_API(esp_ble_gap_config_ext_adv_data_raw(ADV_HANDLE, ext_len, ext_data));
    WAIT_API(esp_ble_gap_ext_adv_start(1, ext_adv_inst));

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);
    return 0;
}
