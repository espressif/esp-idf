// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <freertos/FreeRTOS.h>
#include <esp_system.h>
#include <esp_log.h>
#include <esp_bt.h>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <esp_bt_main.h>
#include <esp_gatt_common_api.h>

#include "simple_ble.h"

#define SIMPLE_BLE_MAX_GATT_TABLE_SIZE  20

static const char *TAG = "simple_ble";

static simple_ble_cfg_t *g_ble_cfg_p;
static uint16_t g_gatt_table_map[SIMPLE_BLE_MAX_GATT_TABLE_SIZE];

static uint8_t adv_config_done;
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

const uint8_t *simple_ble_get_uuid128(uint16_t handle)
{
    const uint8_t *uuid128_ptr;

    for (int i = 0; i < SIMPLE_BLE_MAX_GATT_TABLE_SIZE; i++) {
        if (g_gatt_table_map[i] == handle) {
            uuid128_ptr = (const uint8_t *) g_ble_cfg_p->gatt_db[i].att_desc.uuid_p;
            return uuid128_ptr;
        }
    }
    return NULL;
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&g_ble_cfg_p->adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&g_ble_cfg_p->adv_params);
        }
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t p_gatts_if, esp_ble_gatts_cb_param_t *param)
{
    static esp_gatt_if_t gatts_if = ESP_GATT_IF_NONE;
    esp_err_t ret;
    uint8_t service_instance_id = 0;
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gatts_if = p_gatts_if;
        } else {
            ESP_LOGE(TAG, "reg app failed, app_id 0x0x%x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }

    if (gatts_if != ESP_GATT_IF_NONE && gatts_if != p_gatts_if) {
        return;
    }

    switch (event) {
    case ESP_GATTS_REG_EVT:
        ret = esp_ble_gatts_create_attr_tab(g_ble_cfg_p->gatt_db, gatts_if, g_ble_cfg_p->gatt_db_count, service_instance_id);
        if (ret) {
            ESP_LOGE(TAG, "create attr table failed, error code = 0x%x", ret);
            return;
        }
        ret = esp_ble_gap_set_device_name(g_ble_cfg_p->device_name);
        if (ret) {
            ESP_LOGE(TAG, "set device name failed, error code = 0x%x", ret);
            return;
        }
        ret = esp_ble_gap_config_adv_data_raw(g_ble_cfg_p->raw_adv_data_p,
                                              g_ble_cfg_p->raw_adv_data_len);
        if (ret) {
            ESP_LOGE(TAG, "config raw adv data failed, error code = 0x%x ", ret);
            return;
        }
        adv_config_done |= adv_config_flag;
        ret = esp_ble_gap_config_scan_rsp_data_raw(g_ble_cfg_p->raw_scan_rsp_data_p,
                                                   g_ble_cfg_p->raw_scan_rsp_data_len);
        if (ret) {
            ESP_LOGE(TAG, "config raw scan rsp data failed, error code = 0x%x", ret);
            return;
        }
        adv_config_done |= scan_rsp_config_flag;
        break;
    case ESP_GATTS_READ_EVT:
        g_ble_cfg_p->read_fn(event, gatts_if, param);
        break;
    case ESP_GATTS_WRITE_EVT:
        g_ble_cfg_p->write_fn(event, gatts_if, param);
        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
        g_ble_cfg_p->exec_write_fn(event, gatts_if, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGD(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        if (g_ble_cfg_p->set_mtu_fn) {
            g_ble_cfg_p->set_mtu_fn(event, gatts_if, param);
        }
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGD(TAG, "ESP_GATTS_CONF_EVT, status = %d", param->conf.status);
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGD(TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGD(TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
        g_ble_cfg_p->connect_fn(event, gatts_if, param);
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the iOS system, please refer the official Apple documents about BLE connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGD(TAG, "ESP_GATTS_DISCONNECT_EVT, reason = %d", param->disconnect.reason);
        g_ble_cfg_p->disconnect_fn(event, gatts_if, param);
        esp_ble_gap_start_advertising(&g_ble_cfg_p->adv_params);
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "creating the attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != g_ble_cfg_p->gatt_db_count) {
            ESP_LOGE(TAG, "created attribute table abnormally ");
        } else {
            ESP_LOGD(TAG, "created attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
            memcpy(g_gatt_table_map, param->add_attr_tab.handles, param->add_attr_tab.num_handle * sizeof(g_gatt_table_map[0]));
            /* We assume, for now, that the first entry is always the index to the 'service' definition */
            esp_ble_gatts_start_service(g_gatt_table_map[0]);
        }
        break;
    }
    case ESP_GATTS_STOP_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    case ESP_GATTS_UNREG_EVT:
    case ESP_GATTS_DELETE_EVT:
    default:
        break;
    }
}

simple_ble_cfg_t *simple_ble_init()
{
    simple_ble_cfg_t *ble_cfg_p = (simple_ble_cfg_t *) malloc(sizeof(simple_ble_cfg_t));
    if (ble_cfg_p == NULL) {
        ESP_LOGE(TAG, "No memory for simple_ble_cfg_t");
        return NULL;
    }
    return ble_cfg_p;
}

esp_err_t simple_ble_deinit()
{
    free(g_ble_cfg_p->gatt_db);
    free(g_ble_cfg_p);
    g_ble_cfg_p = NULL;
    return ESP_OK;
}

/* Expects the pointer stays valid throughout */
esp_err_t simple_ble_start(simple_ble_cfg_t *cfg)
{
    g_ble_cfg_p = cfg;
    ESP_LOGD(TAG, "Free mem at start of simple_ble_init %d", esp_get_free_heap_size());
    esp_err_t ret;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed %d", __func__, ret);
        return ret;
    }

#ifdef CONFIG_BTDM_CONTROLLER_MODE_BTDM
    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
#elif defined CONFIG_BTDM_CONTROLLER_MODE_BLE_ONLY
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
#else
    ESP_LOGE(TAG, "Configuration mismatch. Select BLE Only or BTDM mode from menuconfig");
    return ESP_FAIL;
#endif
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed %d", __func__, ret);
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed %d", __func__, ret);
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed %d", __func__, ret);
        return ret;
    }

    ret = esp_ble_gatts_register_callback(gatts_profile_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gatts register error, error code = 0x%x", ret);
        return ret;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gap register error, error code = 0x%x", ret);
        return ret;
    }

    uint16_t app_id = 0x55;
    ret = esp_ble_gatts_app_register(app_id);
    if (ret) {
        ESP_LOGE(TAG, "gatts app register error, error code = 0x%x", ret);
        return ret;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret) {
        ESP_LOGE(TAG, "set local  MTU failed, error code = 0x%x", local_mtu_ret);
    }
    ESP_LOGD(TAG, "Free mem at end of simple_ble_init %d", esp_get_free_heap_size());
    return ESP_OK;
}

esp_err_t simple_ble_stop()
{
    esp_err_t err;
    ESP_LOGD(TAG, "Free mem at start of simple_ble_stop %d", esp_get_free_heap_size());
    err = esp_bluedroid_disable();
    if (err != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "esp_bluedroid_disable called successfully");
    err = esp_bluedroid_deinit();
    if (err != ESP_OK) {
        return err;
    }
    ESP_LOGD(TAG, "esp_bluedroid_deinit called successfully");
    err = esp_bt_controller_disable();
    if (err != ESP_OK) {
        return ESP_FAIL;
    }

    /* The API `esp_bt_controller_deinit` will have to be removed when we add support for
     * `reset to provisioning`
     */
    ESP_LOGD(TAG, "esp_bt_controller_disable called successfully");
    err = esp_bt_controller_deinit();
    if (err != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "esp_bt_controller_deinit called successfully");

    ESP_LOGD(TAG, "Free mem at end of simple_ble_stop %d", esp_get_free_heap_size());
    return ESP_OK;
}
