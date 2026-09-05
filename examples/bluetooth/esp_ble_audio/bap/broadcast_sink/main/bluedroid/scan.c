/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#include "esp_ble_audio_common_api.h"

#include "scan.h"

static SemaphoreHandle_t scan_sem;

/* Controller status latched by gap_event_handler for EXAMPLE_WAIT_API_CHECK. */
static esp_bt_status_t scan_op_status;

#define WAIT_API(_call) EXAMPLE_WAIT_API_CHECK(_call, scan_sem, portMAX_DELAY, scan_op_status)

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK,
    .uncoded_cfg = {
        .scan_type = BLE_SCAN_TYPE_PASSIVE,
        .scan_interval = SCAN_INTERVAL,
        .scan_window = SCAN_WINDOW,
    },
};

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        scan_op_status = param->set_ext_scan_params.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        scan_op_status = param->ext_scan_start.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        scan_op_status = param->ext_scan_stop.status;
        xSemaphoreGive(scan_sem);
        break;

#if CONFIG_EXAMPLE_PAST
    /* pa_sync_with_past() waits on this; without it WAIT_API blocks forever. */
    case ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT:
        scan_op_status = param->set_past_params.status;
        xSemaphoreGive(scan_sem);
        break;
#endif /* CONFIG_EXAMPLE_PAST */

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        scan_op_status = param->ext_adv_set_params.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        scan_op_status = param->ext_adv_data_set.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        scan_op_status = param->ext_adv_start.status;
        xSemaphoreGive(scan_sem);
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

    /* AUTH_CMPL has no BTA channel — app must forward it. */
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        esp_ble_audio_gap_app_post_event(event, param);
        break;
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

    default:
        break;
    }
}

int app_host_init(void)
{
    esp_err_t err;

    scan_sem = xSemaphoreCreateBinary();
    if (scan_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create scan semaphore");
        return -1;
    }

    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err) {
        ESP_LOGE(TAG, "Failed to register GAP callback, err %d", err);
        vSemaphoreDelete(scan_sem);
        return err;
    }

    return 0;
}

int ext_scan_start(void)
{
    WAIT_API(esp_ble_gap_set_ext_scan_params(&ext_scan_params));
    WAIT_API(esp_ble_gap_start_ext_scan(0, 0));

    ESP_LOGI(TAG, "Scanning for broadcast source...");
    return ESP_OK;
}

int ext_scan_stop(void)
{
    WAIT_API(esp_ble_gap_stop_ext_scan());
    return ESP_OK;
}

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid)
{
    esp_ble_gap_periodic_adv_sync_params_t params = {
        .filter_policy = 0,
        .sid = sid,
        .addr_type = addr_type,
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
    };

    memcpy(params.addr, addr, sizeof(params.addr));

    /* Fire-and-forget: sync establishment (PERIODIC_ADV_SYNC_ESTAB_EVT) is
     * air-dependent and surfaces asynchronously. */
    return esp_ble_gap_periodic_adv_create_sync(&params);
}

int pa_sync_terminate(uint16_t sync_handle)
{
    return esp_ble_gap_periodic_adv_sync_terminate(sync_handle);
}

#if CONFIG_EXAMPLE_PAST
int pa_sync_with_past(uint16_t conn_handle, const uint8_t addr[6])
{
    /* mode 0x02 = sync and report; the report stream comes with it, so no
     * separate enable. Addressed by the Assistant's BD address, since BTM
     * dispatches over its ACL. */
    esp_ble_gap_past_params_t params = {
        .mode = ESP_BLE_GAP_PAST_MODE_DUP_FILTER_DISABLED,
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
        .cte_type = 0,
    };
    esp_bd_addr_t peer_addr;

    (void)conn_handle;

    memcpy(peer_addr, addr, sizeof(peer_addr));

    WAIT_API(esp_ble_gap_set_periodic_adv_sync_trans_params(peer_addr, &params));

    /* Sync handle arrives later on the PA_SYNC_PAST event. */
    return ESP_OK;
}
#endif /* CONFIG_EXAMPLE_PAST */

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
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
    return ESP_OK;
}
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */
