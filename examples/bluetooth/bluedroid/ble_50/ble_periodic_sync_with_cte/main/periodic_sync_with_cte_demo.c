/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_ble_cte_api.h"


#define FUNC_SEND_WAIT_SEM(func, sem) do {\
        esp_err_t __err_rc = (func);\
        if (__err_rc != ESP_OK) { \
            ESP_LOGE(LOG_TAG, "%s, message send fail, error = %d", __func__, __err_rc); \
        } \
        xSemaphoreTake(sem, portMAX_DELAY); \
} while(0);

#define LOG_TAG "PERIODIC_SYNC"
#define EXT_SCAN_DURATION     0
#define EXT_SCAN_PERIOD       0

static char remote_device_name[ESP_BLE_ADV_NAME_LEN_MAX] = "ESP_CTE_BLUEDROID";
static SemaphoreHandle_t test_sem = NULL;

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_ENABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK | ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
    .coded_cfg = {BLE_SCAN_TYPE_ACTIVE, 40, 40},
};

static esp_ble_gap_periodic_adv_sync_params_t periodic_adv_sync_params = {
    .filter_policy = 0,
    .sid = 0,
    .addr_type = BLE_ADDR_TYPE_RANDOM,
    .skip = 10,
    .sync_timeout = 1000,
};

uint8_t antenna_ids[4] = {0x00, 0x04, 0x08, 0x0D};
static esp_ble_cte_iq_sampling_params_t iq_sampling_en = {
    .sync_handle = 0,
    .sampling_en = ESP_BLE_CTE_SAMPLING_ENABLE,
    .slot_dur = ESP_BLE_CTE_SLOT_DURATION_2US,
    .max_sampled_ctes = ESP_BLE_CTE_MIN_SAMPLED_CTES,
    .switching_pattern_len = sizeof(antenna_ids),
    .antenna_ids = &antenna_ids[0],
};

bool periodic_sync = false;


static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "Extended scanning params set, status %d", param->set_ext_scan_params.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "Extended scanning start, status %d", param->ext_scan_start.status);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(LOG_TAG, "Extended scanning stop, status %d", param->ext_scan_stop.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising create sync, status %d", param->period_adv_create_sync.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync cancel, status %d", param->period_adv_sync_cancel.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync terminate, status %d", param->period_adv_sync_term.status);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync lost, sync handle %d", param->periodic_adv_sync_lost.sync_handle);
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT:
        ESP_LOGI(LOG_TAG, "Periodic advertising sync establish, status %d", param->periodic_adv_sync_estab.status);
        ESP_LOGI(LOG_TAG, "address "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(param->periodic_adv_sync_estab.adv_addr));
        ESP_LOGI(LOG_TAG, "sync handle %d sid %d perioic adv interval %d adv phy %d", param->periodic_adv_sync_estab.sync_handle,
                                                                                      param->periodic_adv_sync_estab.sid,
                                                                                      param->periodic_adv_sync_estab.period_adv_interval,
                                                                                      param->periodic_adv_sync_estab.adv_phy);
        iq_sampling_en.sync_handle = param->periodic_adv_sync_estab.sync_handle;
        esp_ble_cte_set_connectionless_iq_sampling_enable(&iq_sampling_en);
        break;
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        uint8_t *adv_name = NULL;
        uint8_t adv_name_len = 0;
	    adv_name = esp_ble_resolve_adv_data_by_type(param->ext_adv_report.params.adv_data,
                                            param->ext_adv_report.params.adv_data_len,
                                            ESP_BLE_AD_TYPE_NAME_CMPL,
                                            &adv_name_len);
	    if ((adv_name != NULL) && (memcmp(adv_name, remote_device_name, adv_name_len) == 0) && !periodic_sync) {
            // Note: If there are multiple devices with the same device name, the device may sync to an unintended one.
            // It is recommended to change the default device name to ensure it is unique.
            periodic_sync = true;
	        char adv_temp_name[30] = {'0'};
	        memcpy(adv_temp_name, adv_name, adv_name_len);
	        ESP_LOGI(LOG_TAG, "Create sync with the peer device %s", adv_temp_name);
            periodic_adv_sync_params.sid = param->ext_adv_report.params.sid;
	        periodic_adv_sync_params.addr_type = param->ext_adv_report.params.addr_type;
	        memcpy(periodic_adv_sync_params.addr, param->ext_adv_report.params.addr, sizeof(esp_bd_addr_t));
            esp_ble_gap_periodic_adv_create_sync(&periodic_adv_sync_params);
	    }
    }
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT:
        ESP_LOGI(LOG_TAG, "Periodic adv report, sync handle %d, data status %d, data len %d, rssi %d", param->period_adv_report.params.sync_handle,
                                                                                                    param->period_adv_report.params.data_status,
                                                                                                    param->period_adv_report.params.data_length,
                                                                                                    param->period_adv_report.params.rssi);
        //esp_log_buffer_hex("data", param->period_adv_report.params.data, param->period_adv_report.params.data_length);
        break;

    default:
        break;
    }
}

static void cte_event_handler(esp_ble_cte_cb_event_t event, esp_ble_cte_cb_param_t *param)
{
    switch (event) {
        case ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT, status %d", param->set_trans_params_cmpl.status);
            break;
        case ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT, status %d", param->set_trans_enable_cmpl.status);
            break;
        case ESP_BLE_CTE_SET_CONNLESS_IQ_SAMPLING_ENABLE_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "CTE IQ sampling enable, status %d", param->iq_sampling_enable_cmpl.status);
            break;
        case ESP_BLE_CTE_READ_ANT_INFOR_CMPL_EVT:
            ESP_LOGI(LOG_TAG, "CTE read ant information cmpl, status %d", param->read_ant_infor_cmpl.status);
            break;
        case ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT:
            ESP_LOGI(LOG_TAG, "sync_handle %d channel_idx %d rssi %d rssi_ant_id %d cte_type %d slot_dur %d pkt_status %d periodic_evt_counter %d sample_count %d",
                                param->connless_iq_rpt.sync_handle, param->connless_iq_rpt.channel_idx, param->connless_iq_rpt.rssi,
                                param->connless_iq_rpt.rssi_ant_id, param->connless_iq_rpt.cte_type, param->connless_iq_rpt.slot_dur,
                                param->connless_iq_rpt.pkt_status, param->connless_iq_rpt.periodic_evt_counter, param->connless_iq_rpt.sample_count);

            ESP_LOG_BUFFER_HEX("i_sample", &param->connless_iq_rpt.i_sample[0], param->connless_iq_rpt.sample_count);
            ESP_LOG_BUFFER_HEX("q_sample", &param->connless_iq_rpt.q_sample[0], param->connless_iq_rpt.sample_count);
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

    #if CONFIG_EXAMPLE_CI_PIPELINE_ID
    memcpy(remote_device_name, esp_bluedroid_get_example_name(), sizeof(remote_device_name));
    #endif

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

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
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

    esp_ble_cte_register_callback(cte_event_handler);

    vTaskDelay(200 / portTICK_PERIOD_MS);

    test_sem = xSemaphoreCreateBinary();

    FUNC_SEND_WAIT_SEM(esp_ble_gap_set_ext_scan_params(&ext_scan_params), test_sem);
    FUNC_SEND_WAIT_SEM(esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION, EXT_SCAN_PERIOD), test_sem);


    return;
}
