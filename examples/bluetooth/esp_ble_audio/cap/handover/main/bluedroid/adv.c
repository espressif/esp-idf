/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"

#include "adv.h"
#include "cap_handover.h"

static SemaphoreHandle_t adv_sem;

static esp_bt_status_t adv_op_status;

#define WAIT_API(_call) EXAMPLE_WAIT_API_CHECK(_call, adv_sem, portMAX_DELAY, adv_op_status)

static esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
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

static esp_ble_gap_periodic_adv_params_t periodic_adv_params = {
    .interval_min = ESP_BLE_GAP_PERIODIC_ADV_ITVL_MS(PER_ADV_INTERVAL_MS),
    .interval_max = ESP_BLE_GAP_PERIODIC_ADV_ITVL_MS(PER_ADV_INTERVAL_MS),
    .properties = 0,
};

static esp_ble_gap_ext_adv_t ext_adv_inst[1] = {
    [0] = { ADV_HANDLE, 0, 0 },
};

void adv_gap_event_handler(esp_gap_ble_cb_event_t event,
                           esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        adv_op_status = param->ext_adv_set_params.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        adv_op_status = param->ext_adv_data_set.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        adv_op_status = param->ext_adv_start.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        adv_op_status = param->ext_adv_stop.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
        adv_op_status = param->peroid_adv_set_params.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT:
        adv_op_status = param->period_adv_data_set.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
        adv_op_status = param->period_adv_start.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT:
        adv_op_status = param->period_adv_stop.status;
        xSemaphoreGive(adv_sem);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT:
        /* Not awaited: the Acceptor reports the result through its receive state. */
        if (param->period_adv_set_info_trans.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "PAST failed, status %d", param->period_adv_set_info_trans.status);
        }
        break;
    default:
        break;
    }
}

int adv_host_init(void)
{
    adv_sem = xSemaphoreCreateBinary();
    if (adv_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create adv semaphore");
        return -1;
    }

    return 0;
}

static int adv_set_configure(const uint8_t *ext_data, uint8_t ext_len)
{
    WAIT_API(esp_ble_gap_ext_adv_set_params(ADV_HANDLE, &ext_adv_params));
    WAIT_API(esp_ble_gap_config_ext_adv_data_raw(ADV_HANDLE, ext_len, ext_data));
    WAIT_API(esp_ble_gap_periodic_adv_set_params(ADV_HANDLE, &periodic_adv_params));

    return 0;
}

int per_adv_data_start(const uint8_t *per_data, uint8_t per_len)
{
#if CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH
    WAIT_API(esp_ble_gap_config_periodic_adv_data_raw(ADV_HANDLE, per_len, per_data, false));
    WAIT_API(esp_ble_gap_periodic_adv_start(ADV_HANDLE, true));
#else
    WAIT_API(esp_ble_gap_config_periodic_adv_data_raw(ADV_HANDLE, per_len, per_data));
    WAIT_API(esp_ble_gap_periodic_adv_start(ADV_HANDLE));
#endif

    ESP_LOGI(TAG, "Periodic advertising started (handle %u)", ADV_HANDLE);
    return 0;
}


int ext_adv_start_without_base(const uint8_t *ext_data, uint8_t ext_len)
{
    int err;

    err = adv_set_configure(ext_data, ext_len);
    if (err) {
        return err;
    }

    WAIT_API(esp_ble_gap_ext_adv_start(1, ext_adv_inst));

    ESP_LOGI(TAG, "Advertising started, BASE pending (handle %u)", ADV_HANDLE);
    return 0;
}

int adv_stop(void)
{
    uint8_t instance = ADV_HANDLE;

    WAIT_API(esp_ble_gap_periodic_adv_stop(ADV_HANDLE));
    WAIT_API(esp_ble_gap_ext_adv_stop(1, &instance));

    ESP_LOGI(TAG, "Advertising stopped (handle %u)", ADV_HANDLE);
    return 0;
}

int local_public_addr_get(uint8_t addr[6])
{
    const uint8_t *bda = esp_bt_dev_get_address();

    if (bda == NULL) {
        ESP_LOGE(TAG, "Local BD address unavailable");
        return -1;
    }

    /* Bluedroid hands out addresses MSB-first; the caller feeds this to the audio
     * stack, which stores a bt_addr_le_t and puts it on air LSB-first.
     */
    for (size_t i = 0; i < 6; i++) {
        addr[i] = bda[5 - i];
    }

    return 0;
}

int pa_set_info_transfer(uint16_t conn_handle, const uint8_t peer_addr[6],
                         uint16_t service_data)
{
    esp_bd_addr_t addr;

    (void)conn_handle;
    memcpy(addr, peer_addr, sizeof(addr));

    return esp_ble_gap_periodic_adv_set_info_trans(addr, service_data, ADV_HANDLE);
}
