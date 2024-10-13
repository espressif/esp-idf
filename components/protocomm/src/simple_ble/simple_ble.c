/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <esp_system.h>
#include <esp_log.h>
#ifdef CONFIG_BT_CONTROLLER_ENABLED
#include "esp_bt.h"
#endif
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <esp_bt_main.h>
#include "esp_bt_device.h"
#include <esp_gatt_common_api.h>

#include "simple_ble.h"

static uint16_t g_ble_max_gatt_table_size;

static const char *TAG = "simple_ble";

static simple_ble_cfg_t *g_ble_cfg_p;
static uint16_t *g_gatt_table_map;

static uint8_t adv_config_done;
static esp_bd_addr_t s_cached_remote_bda = {0x0,};
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

uint8_t get_keep_ble_on()
{
    return g_ble_cfg_p->keep_ble_on;
}

const uint8_t *simple_ble_get_uuid128(uint16_t handle)
{
    const uint8_t *uuid128_ptr;

    for (int i = 0; i < g_ble_max_gatt_table_size; i++) {
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
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);

        if (g_ble_cfg_p->ble_addr) {
            esp_ble_gap_set_rand_addr(g_ble_cfg_p->ble_addr);
            g_ble_cfg_p->adv_params.own_addr_type = BLE_ADDR_TYPE_RANDOM;
        }

        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&g_ble_cfg_p->adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);

        if (g_ble_cfg_p->ble_addr) {
            esp_ble_gap_set_rand_addr(g_ble_cfg_p->ble_addr);
            g_ble_cfg_p->adv_params.own_addr_type = BLE_ADDR_TYPE_RANDOM;
        }

        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&g_ble_cfg_p->adv_params);
        }
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
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
        ret = esp_ble_gap_config_adv_data(g_ble_cfg_p->adv_data_p);
        if (ret) {
            ESP_LOGE(TAG, "config raw adv data failed, error code = 0x%x ", ret);
            return;
        }
        adv_config_done |= adv_config_flag;
        ret = esp_ble_gap_config_adv_data(g_ble_cfg_p->scan_rsp_data_p);
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
	memcpy(s_cached_remote_bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
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
        memset(s_cached_remote_bda, 0, sizeof(esp_bd_addr_t));
        esp_ble_gap_start_advertising(&g_ble_cfg_p->adv_params);
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "creating the attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != g_ble_cfg_p->gatt_db_count) {
            ESP_LOGE(TAG, "created attribute table abnormally ");
        } else {
            ESP_LOGD(TAG, "created attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
            g_gatt_table_map = (uint16_t *) calloc(param->add_attr_tab.num_handle, sizeof(uint16_t));
            if (g_gatt_table_map == NULL) {
                ESP_LOGE(TAG, "Memory allocation for GATT_TABLE_MAP failed ");
                break;
            }
            /* Update g_ble_max_gatt_table_size with number of handles */
            g_ble_max_gatt_table_size = param->add_attr_tab.num_handle;
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

simple_ble_cfg_t *simple_ble_init(void)
{
    simple_ble_cfg_t *ble_cfg_p = (simple_ble_cfg_t *) calloc(1, sizeof(simple_ble_cfg_t));
    if (ble_cfg_p == NULL) {
        ESP_LOGE(TAG, "No memory for simple_ble_cfg_t");
        return NULL;
    }
    return ble_cfg_p;
}

esp_err_t simple_ble_deinit(void)
{
    free(g_ble_cfg_p->gatt_db);
    g_ble_cfg_p->gatt_db = NULL;
    free(g_ble_cfg_p);
    g_ble_cfg_p = NULL;

    free(g_gatt_table_map);
    g_gatt_table_map = NULL;
    g_ble_max_gatt_table_size = 0;

    return ESP_OK;
}

/* Expects the pointer stays valid throughout */
esp_err_t simple_ble_start(simple_ble_cfg_t *cfg)
{
    g_ble_cfg_p = cfg;
    ESP_LOGD(TAG, "Free mem at start of simple_ble_init %" PRIu32, esp_get_free_heap_size());
    esp_err_t ret;

#ifdef CONFIG_BT_CONTROLLER_ENABLED
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed %d", __func__, ret);
        return ret;
    }

#ifdef CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY
    ESP_LOGE(TAG, "Configuration mismatch. Select BLE Only or BTDM mode from menuconfig");
    return ESP_FAIL;
#elif CONFIG_BTDM_CTRL_MODE_BTDM
    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
#else  //For all other chips supporting BLE Only
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
#endif

    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed %d", __func__, ret);
        return ret;
    }
#endif

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
    ESP_LOGD(TAG, "Free mem at end of simple_ble_init %" PRIu32, esp_get_free_heap_size());

    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    esp_ble_auth_req_t auth_req= ESP_LE_AUTH_REQ_MITM;
    if (cfg->ble_bonding) {
	auth_req |= ESP_LE_AUTH_BOND;     //bonding with peer device after authentication
    }
    if (cfg->ble_sm_sc) {
	auth_req |= ESP_LE_AUTH_REQ_SC_ONLY;
    }
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));

    return ESP_OK;
}

esp_err_t simple_ble_stop(void)
{
    esp_err_t err;
    ESP_LOGD(TAG, "Free mem at start of simple_ble_stop %" PRIu32, esp_get_free_heap_size());
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
#ifdef CONFIG_BT_CONTROLLER_ENABLED
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
#endif
    ESP_LOGD(TAG, "Free mem at end of simple_ble_stop %" PRIu32, esp_get_free_heap_size());
    return ESP_OK;
}

esp_err_t simple_ble_disconnect(void)
{
    return esp_ble_gap_disconnect(s_cached_remote_bda);
}
