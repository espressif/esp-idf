/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE Periodic Advertising with Responses (PAwR) on the advertiser side.
* The device starts extended advertising and periodic advertising with subevents.
* When the host receives ESP_GAP_BLE_PERIODIC_ADV_SUBEVT_DATA_REQUEST_EVT, it provides
* per-subevent data via esp_ble_gap_set_periodic_adv_subevent_data().
* If a synchronizer sends responses, the advertiser will receive
* ESP_GAP_BLE_PERIODIC_ADV_RESPONSE_REPORT_EVT.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"

#include "sdkconfig.h"

#define TAG "BLE_PAWR_ADV"

#define EXT_ADV_HANDLE      0
#define NUM_EXT_ADV         1

#define SEM_WAIT_TIMEOUT_MS     5000

#define FUNC_SEND_WAIT_SEM(func, sem) do {\
        esp_err_t __err_rc = (func);\
        if (__err_rc != ESP_OK) { \
            ESP_LOGE(TAG, "%s failed, error = %d", #func, __err_rc); \
            return; \
        } \
        xSemaphoreTake(sem, portMAX_DELAY); \
} while(0);

// PAwR parameters
#define BLE_PAWR_SUB_DATA_LEN                 (20)

static SemaphoreHandle_t test_sem = NULL;
static esp_ble_subevent_params sub_data_params[10];
static uint8_t sub_data_pattern[BLE_PAWR_SUB_DATA_LEN] = {0};
static uint8_t subevent_data_buffers[10][BLE_PAWR_SUB_DATA_LEN];

// Extended advertising data
static uint8_t raw_ext_adv_data[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x02, ESP_BLE_AD_TYPE_TX_PWR, 0xeb,
    0x10, ESP_BLE_AD_TYPE_NAME_CMPL, 'E', 'S', 'P', '_', 'E', 'X', 'T', 'P', 'A', 'w', 'R', '_', 'A', 'D', 'V'
};

static esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
    .interval_min = 600,
    .interval_max = 600,
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
    .interval_min = 600,
    .interval_max = 600,
    .properties = 0, // Do not include TX power
    .num_subevents = 10,
    .subevent_interval = 44,
    .rsp_slot_delay = 20,
    .rsp_slot_spacing = 32,
    .num_rsp_slots = 5,
};

static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {EXT_ADV_HANDLE, 0, 0},
};

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GAP event handler received NULL param");
        return;
    }

    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising params set, status %d, instance %d",
                 param->ext_adv_set_params.status, param->ext_adv_set_params.instance);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising data set, status %d, instance %d",
                 param->ext_adv_data_set.status, param->ext_adv_data_set.instance);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising start, status %d, instance num %d",
                 param->ext_adv_start.status, param->ext_adv_start.instance_num);
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising set random address, status %d, instance %d",
                 param->ext_adv_set_rand_addr.status, param->ext_adv_set_rand_addr.instance);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Periodic advertising params set, status %d, instance %d",
                 param->peroid_adv_set_params.status, param->peroid_adv_set_params.instance);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Periodic advertising data set, status %d, instance %d",
                 param->period_adv_data_set.status, param->period_adv_data_set.instance);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Periodic advertising start, status %d, instance %d",
                 param->period_adv_start.status, param->period_adv_start.instance);
        break;
    case ESP_GAP_BLE_SET_PERIODIC_ADV_SUBEVT_DATA_EVT:
        ESP_LOGI(TAG, "Periodic adv subevent data set, status %d, adv_handle %d",
                 param->pa_subevt_data_evt.status, param->pa_subevt_data_evt.adv_handle);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SUBEVT_DATA_REQUEST_EVT: {
        ESP_LOGI(TAG, "[Request] data: 0x%02x, subevt start:%d, subevt count:%d",
                 sub_data_pattern[0],
                 param->pa_subevt_data_req_evt.subevt_start,
                 param->pa_subevt_data_req_evt.subevt_data_count);

        uint8_t sent_num = param->pa_subevt_data_req_evt.subevt_data_count;
        if (sent_num > 10) {
            ESP_LOGE(TAG, "Invalid subevent count: %d", sent_num);
            sent_num = 10;
        }

        // Prepare subevent data
        for (uint8_t i = 0; i < sent_num && i < 10; i++) {
            uint8_t sub = (i + param->pa_subevt_data_req_evt.subevt_start) % 10;

            // Prepare data pattern with overflow protection
            sub_data_pattern[0] = (sub_data_pattern[0] + 1) % 256;
            memset(&sub_data_pattern[1], sub, BLE_PAWR_SUB_DATA_LEN - 1);

            // Copy to buffer
            memcpy(subevent_data_buffers[i], sub_data_pattern, BLE_PAWR_SUB_DATA_LEN);

            // Set subevent parameters
            sub_data_params[i].subevent = sub;
            sub_data_params[i].response_slot_start = 0;
            sub_data_params[i].response_slot_count = 5;
            sub_data_params[i].subevent_data_len = BLE_PAWR_SUB_DATA_LEN;
            sub_data_params[i].subevent_data = subevent_data_buffers[i];
        }

        // Set subevent data
        esp_ble_per_adv_subevent_data_params subevent_data_params = {0};
        subevent_data_params.adv_handle = param->pa_subevt_data_req_evt.adv_handle;
        subevent_data_params.num_subevents_with_data = sent_num;
        subevent_data_params.subevent_params = sub_data_params;

        esp_err_t ret = esp_ble_gap_set_periodic_adv_subevent_data(&subevent_data_params);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set Subevent Data, error = 0x%x", ret);
        }
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_RESPONSE_REPORT_EVT: {
        ESP_LOGI(TAG, "[Response] adv_handle:%d, subevent:%d, tx_status:%d, num_rsp:%d",
                param->pa_rsp_rpt_evt.adv_handle,
                param->pa_rsp_rpt_evt.subevt,
                param->pa_rsp_rpt_evt.tx_status,
                param->pa_rsp_rpt_evt.num_rsp);

        if (param->pa_rsp_rpt_evt.pa_rsp_info && param->pa_rsp_rpt_evt.num_rsp > 0) {
            // Limit num_rsp to prevent array overflow
            uint8_t num_rsp = (param->pa_rsp_rpt_evt.num_rsp > 10) ? 10 : param->pa_rsp_rpt_evt.num_rsp;
            for (uint8_t i = 0; i < num_rsp && i < 10; i++) {
                esp_ble_pa_rsp_info *rsp_info = &param->pa_rsp_rpt_evt.pa_rsp_info[i];
                if (rsp_info->data_status == 0x00) {  // Data complete
                    ESP_LOGI(TAG, "Response slot:%d, data_length:%d",
                             rsp_info->rsp_slot, rsp_info->data_len);
                    if (rsp_info->data && rsp_info->data_len >= 2) {
                        ESP_LOGI(TAG, "data: 0x%02x, 0x%02x", rsp_info->data[0], rsp_info->data[1]);
                    }
                } else {
                    ESP_LOGW(TAG, "Response slot:%d, data_status:%d",
                             rsp_info->rsp_slot, rsp_info->data_status);
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

/**
 * @brief Start periodic advertising with PAwR
 */
static void start_periodic_adv(void)
{
    // Create static random address
    esp_bd_addr_t rand_addr;
    esp_ble_gap_addr_create_static(rand_addr);

    ESP_LOG_BUFFER_HEX(TAG, rand_addr, ESP_BD_ADDR_LEN);

    // Set extended advertising parameters
    ESP_LOGI(TAG, "Set extended advertising params...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_rand_addr(EXT_ADV_HANDLE, rand_addr), test_sem);

    // Set extended advertising data
    ESP_LOGI(TAG, "Set extended advertising data...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE,
                                                           sizeof(raw_ext_adv_data),
                                                           raw_ext_adv_data), test_sem);

    // Start extended advertising
    ESP_LOGI(TAG, "Start extended advertising...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(NUM_EXT_ADV, ext_adv), test_sem);

    // Set periodic advertising parameters
    ESP_LOGI(TAG, "Set periodic adv (PAwR) params...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_set_params(EXT_ADV_HANDLE, &periodic_adv_params), test_sem);

    // Start periodic advertising (PAwR)
    ESP_LOGI(TAG, "Start periodic advertising (PAwR)...");
    FUNC_SEND_WAIT_SEM(esp_ble_gap_periodic_adv_start(EXT_ADV_HANDLE), test_sem);

    ESP_LOGI(TAG, "PAwR advertising started successfully");
}

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
        return;
    }

    // Create semaphore
    test_sem = xSemaphoreCreateBinary();

    // Start periodic advertising with PAwR
    start_periodic_adv();

    ESP_LOGI(TAG, "BLE PAwR Advertiser example started");
}
