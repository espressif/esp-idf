/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE GATT server. It can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server demo.
* Client demo will enable gatt_server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"

#include "freertos/semphr.h"

#define LOG_TAG "MULTI_ADV_DEMO"

#define FUNC_SEND_WAIT_SEM(func, sem) do {\
        esp_err_t __err_rc = (func);\
        if (__err_rc != ESP_OK) { \
            ESP_LOGE(LOG_TAG, "%s, message send fail, error = %d", __func__, __err_rc); \
        } \
        xSemaphoreTake(sem, portMAX_DELAY); \
} while(0);

#define EXT_ADV_HANDLE      0
#define NUM_EXT_ADV         1

static SemaphoreHandle_t test_sem = NULL;


uint8_t addr_2m[6] = {0xc0, 0xde, 0x52, 0x00, 0x00, 0x02};

esp_ble_gap_ext_adv_params_t ext_adv_params_2M = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
    .interval_min = 0x30,
    .interval_max = 0x30,
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_2M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    .tx_power = EXT_ADV_TX_PWR_NO_PREFERENCE,
};

static esp_ble_gap_periodic_adv_params_t periodic_adv_params = {
    .interval_min = 0x40, // 80 ms interval
    .interval_max = 0x40,
    .properties = 0, // Do not include TX power
};

static uint8_t periodic_adv_raw_data[] = {
        0x02, 0x01, 0x06,
        0x02, 0x0a, 0xeb,
        0x03, 0x03, 0xab, 0xcd,
        0x11, 0x09, 'E', 'S', 'P', '_', 'P', 'E', 'R', 'I', 'O', 'D', 'I',
        'C', '_', 'A', 'D', 'V'
};

static uint8_t raw_ext_adv_data_2m[] = {
        0x02, 0x01, 0x06,
        0x02, 0x0a, 0xeb,
        0x13, 0x09, 'E', 'S', 'P', '_', 'M', 'U', 'L', 'T', 'I', '_', 'A',
        'D', 'V', '_', '8', '0', 'M', 'S'
};

static esp_ble_gap_ext_adv_t ext_adv[1] = {
    // instance, duration, period
    [0] = {EXT_ADV_HANDLE, 0, 0},
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status %d", param->ext_adv_set_rand_addr.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status %d", param->ext_adv_set_params.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status %d", param->ext_adv_data_set.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT, status %d", param->scan_rsp_set.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status %d", param->ext_adv_start.status);
        break;
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT, status %d", param->ext_adv_stop.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, status %d", param->peroid_adv_set_params.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT, status %d", param->period_adv_data_set.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT, status %d", param->period_adv_start.status);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(LOG_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(LOG_TAG, "gap register error, error code = %x", ret);
        return;
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);

    test_sem = xSemaphoreCreateBinary();
    // 2M phy extend adv, Connectable advertising
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params_2M), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(EXT_ADV_HANDLE, addr_2m), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE, sizeof(raw_ext_adv_data_2m), &raw_ext_adv_data_2m[0]), test_sem);

    // start all adv
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(NUM_EXT_ADV, &ext_adv[0]), test_sem);

    // set periodic adv param
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_set_params(EXT_ADV_HANDLE, &periodic_adv_params), test_sem);

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
    // set periodic adv raw data
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_periodic_adv_data_raw(EXT_ADV_HANDLE, sizeof(periodic_adv_raw_data), &periodic_adv_raw_data[0], false), test_sem);
    // start periodic adv, include the ADI field in AUX_SYNC_IND PDUs
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_start(EXT_ADV_HANDLE, true), test_sem);
    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        // just update the Advertising DID of the periodic advertising, unchanged data
        FUNC_SEND_WAIT_SEM(esp_ble_gap_config_periodic_adv_data_raw(EXT_ADV_HANDLE, 0, NULL, true), test_sem);
    }
#else
    // set periodic adv raw data
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_periodic_adv_data_raw(EXT_ADV_HANDLE, sizeof(periodic_adv_raw_data), &periodic_adv_raw_data[0]), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_start(EXT_ADV_HANDLE), test_sem);
#endif

    return;
}
