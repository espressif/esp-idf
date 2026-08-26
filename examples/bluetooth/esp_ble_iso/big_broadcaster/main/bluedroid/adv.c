/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#include "adv.h"

static SemaphoreHandle_t adv_sem;

/* Controller status latched by gap_event_handler for EXAMPLE_WAIT_API_CHECK. */
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

static void gap_event_handler(esp_gap_ble_cb_event_t event,
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
    default:
        break;
    }
}

int app_host_init(void)
{
    esp_err_t err;

    adv_sem = xSemaphoreCreateBinary();
    if (adv_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create adv semaphore");
        return -1;
    }

    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err) {
        ESP_LOGE(TAG, "Failed to register GAP callback, err %d", err);
        vSemaphoreDelete(adv_sem);
        return err;
    }

    return 0;
}

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len,
                  const uint8_t *per_data, uint8_t per_len)
{
    WAIT_API(esp_ble_gap_ext_adv_set_params(ADV_HANDLE, &ext_adv_params));
    WAIT_API(esp_ble_gap_config_ext_adv_data_raw(ADV_HANDLE, ext_len, ext_data));
    WAIT_API(esp_ble_gap_periodic_adv_set_params(ADV_HANDLE, &periodic_adv_params));
#if CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH
    WAIT_API(esp_ble_gap_config_periodic_adv_data_raw(ADV_HANDLE, per_len, per_data, false));
    WAIT_API(esp_ble_gap_periodic_adv_start(ADV_HANDLE, true));
#else
    WAIT_API(esp_ble_gap_config_periodic_adv_data_raw(ADV_HANDLE, per_len, per_data));
    WAIT_API(esp_ble_gap_periodic_adv_start(ADV_HANDLE));
#endif
    WAIT_API(esp_ble_gap_ext_adv_start(1, ext_adv_inst));

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);
    return 0;
}
