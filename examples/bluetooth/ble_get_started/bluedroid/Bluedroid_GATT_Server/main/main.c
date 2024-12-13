/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "heart_rate.h"
#include "led.h"

#define PROFILE_NUM 2
#define HEART_PROFILE_APP_ID 0
#define AUTO_IO_PROFILE_APP_ID 1
#define HEART_RATE_SVC_UUID 0x180D
#define HEART_RATE_CHAR_UUID 0x2A37
#define HEART_NUM_HANDLE 4
#define AUTO_IO_SVC_UUID 0x1815
#define AUTO_IO_NUM_HANDLE 3

#define ADV_CONFIG_FLAG      (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

///Declare the static function
static void heart_gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void auto_io_gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void example_write_event_env(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static const char *GATTS_TAG = "GATTS_DEMO";
static esp_gatt_char_prop_t heart_property = 0;
static esp_gatt_char_prop_t auto_io_property = 0;
static uint8_t heart_rate_val[2] = {0};
static uint8_t led_status[2] = {0};
static bool indicate_enabled = false;
static bool hrs_create_cmpl = false;  // Heart Rate Service
static uint8_t adv_config_done = 0;

static esp_attr_value_t heart_rate_attr = {
    .attr_max_len = 2,
    .attr_len     = sizeof(heart_rate_val),
    .attr_value   = heart_rate_val,
};

static esp_attr_value_t led_status_attr = {
    .attr_max_len = 2,
    .attr_len     = sizeof(led_status),
    .attr_value   = led_status,
};

static const uint8_t led_chr_uuid[] = {
    0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,  // 20ms
    .adv_int_max        = 0x40,  // 40ms
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [HEART_PROFILE_APP_ID] = {
        .gatts_cb = heart_gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [AUTO_IO_PROFILE_APP_ID] = {
        .gatts_cb = auto_io_gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static void heart_rate_task(void* param)
{
    ESP_LOGI(GATTS_TAG, "Heart Rate Task Start");

    while (1) {
        if (hrs_create_cmpl) {
            update_heart_rate();
            ESP_LOGI(GATTS_TAG, "Heart Rate updated to %d", get_heart_rate());

            heart_rate_val[1] = get_heart_rate();
            esp_ble_gatts_set_attr_value(gl_profile_tab[HEART_PROFILE_APP_ID].char_handle, 2, heart_rate_val);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(GATTS_TAG, "Advertising data set, status %d", param->adv_data_cmpl.status);
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(GATTS_TAG, "Scan response data set, status %d", param->scan_rsp_data_cmpl.status);
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "Advertising start failed, status %d", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(GATTS_TAG, "Advertising start successfully");
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(GATTS_TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                 param->update_conn_params.status,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
        ESP_LOGI(GATTS_TAG, "Packet length update, status %d, rx %d, tx %d",
                 param->pkt_data_length_cmpl.status,
                 param->pkt_data_length_cmpl.params.rx_len,
                 param->pkt_data_length_cmpl.params.tx_len);
        break;
    default:
        break;
    }
}

static void heart_gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "GATT server register, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gl_profile_tab[HEART_PROFILE_APP_ID].service_id.is_primary = true;
        gl_profile_tab[HEART_PROFILE_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[HEART_PROFILE_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[HEART_PROFILE_APP_ID].service_id.id.uuid.uuid.uuid16 = HEART_RATE_SVC_UUID;

        //config adv data
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret) {
            ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
            break;
        }

        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[HEART_PROFILE_APP_ID].service_id, HEART_NUM_HANDLE);
        break;
    case ESP_GATTS_CREATE_EVT:
        //service has been created, now add characteristic declaration
        ESP_LOGI(GATTS_TAG, "Service create, status %d, service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[HEART_PROFILE_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[HEART_PROFILE_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[HEART_PROFILE_APP_ID].char_uuid.uuid.uuid16 = HEART_RATE_CHAR_UUID;
        esp_ble_gatts_start_service(gl_profile_tab[HEART_PROFILE_APP_ID].service_handle);
        heart_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_INDICATE;
        ret = esp_ble_gatts_add_char(gl_profile_tab[HEART_PROFILE_APP_ID].service_handle, &gl_profile_tab[HEART_PROFILE_APP_ID].char_uuid,
                            ESP_GATT_PERM_READ,
                            heart_property,
                            &heart_rate_attr, NULL);
        if (ret) {
            ESP_LOGE(GATTS_TAG, "add char failed, error code = %x", ret);
        }
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        ESP_LOGI(GATTS_TAG, "Characteristic add, status %d, attr_handle %d, char_uuid %x",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.char_uuid.uuid.uuid16);
        gl_profile_tab[HEART_PROFILE_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[HEART_PROFILE_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[HEART_PROFILE_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        ESP_LOGI(GATTS_TAG, "heart rate char handle %d", param->add_char.attr_handle);
        ret = esp_ble_gatts_add_char_descr(gl_profile_tab[HEART_PROFILE_APP_ID].service_handle, &gl_profile_tab[HEART_PROFILE_APP_ID].descr_uuid,
                            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        ESP_LOGI(GATTS_TAG, "Descriptor add, status %d, attr_handle %u",
                 param->add_char_descr.status, param->add_char_descr.attr_handle);
        gl_profile_tab[HEART_PROFILE_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        hrs_create_cmpl = true;
        break;
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(GATTS_TAG, "Characteristic read");
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 2;
        memcpy(rsp.attr_value.value, heart_rate_val, sizeof(heart_rate_val));
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    case ESP_GATTS_WRITE_EVT:
        ESP_LOGI(GATTS_TAG, "Characteristic write, value len %u, value ", param->write.len);
        ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);

        if (gl_profile_tab[HEART_PROFILE_APP_ID].descr_handle == param->write.handle && param->write.len == 2) {
            uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
            if (descr_value == 0x0001) {
                if (heart_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY) {
                    ESP_LOGI(GATTS_TAG, "Notification enable");
                    uint8_t notify_data[15];
                    for (int i = 0; i < sizeof(notify_data); i++) {
                        notify_data[i] = i%0xff;
                    }
                    //the size of notify_data[] need less than MTU size
                    esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[HEART_PROFILE_APP_ID].char_handle,
                                            sizeof(notify_data), notify_data, false);
                }
            } else if (descr_value == 0x0002) {
                if (heart_property & ESP_GATT_CHAR_PROP_BIT_INDICATE) {
                    ESP_LOGI(GATTS_TAG, "Indication enable");
                    indicate_enabled = true;
                    uint8_t indicate_data[15];
                    for (int i = 0; i < sizeof(indicate_data); i++) {
                        indicate_data[i] = i%0xff;
                    }
                    //the size of indicate_data[] need less than MTU size
                    esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[HEART_PROFILE_APP_ID].char_handle,
                                            sizeof(indicate_data), indicate_data, true);
                }
            } else if (descr_value == 0x0000) {
                indicate_enabled = false;
                ESP_LOGI(GATTS_TAG, "Notification/Indication disable");
            } else {
                ESP_LOGE(GATTS_TAG, "Invalid descriptor value");
                ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);
            }
        }
        example_write_event_env(gatts_if, param);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "Service start, status %d, service_handle %d", param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR"",
                param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        gl_profile_tab[HEART_PROFILE_APP_ID].conn_id = param->connect.conn_id;
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        indicate_enabled = false;
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_TAG, "Confirm receive, status %d, attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK) {
            ESP_LOG_BUFFER_HEX(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;
    case ESP_GATTS_SET_ATTR_VAL_EVT:
        ESP_LOGI(GATTS_TAG, "Attribute value set, status %d", param->set_attr_val.status);
        if (indicate_enabled) {
            uint8_t indicate_data[2] = {0};
            memcpy(indicate_data, heart_rate_val, sizeof(heart_rate_val));
            esp_ble_gatts_send_indicate(gatts_if, gl_profile_tab[HEART_PROFILE_APP_ID].conn_id, gl_profile_tab[HEART_PROFILE_APP_ID].char_handle, sizeof(indicate_data), indicate_data, true);
        }
        break;
    default:
        break;
    }
}

static void auto_io_gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "GATT server register, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_id.is_primary = true;
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_id.id.uuid.uuid.uuid16 = AUTO_IO_SVC_UUID;
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_id, AUTO_IO_NUM_HANDLE);
        break;
    case ESP_GATTS_CREATE_EVT:
        //service has been created, now add characteristic declaration
        ESP_LOGI(GATTS_TAG, "Service create, status %d, service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].char_uuid.len = ESP_UUID_LEN_128;
        memcpy(gl_profile_tab[AUTO_IO_PROFILE_APP_ID].char_uuid.uuid.uuid128, led_chr_uuid, ESP_UUID_LEN_128);

        esp_ble_gatts_start_service(gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_handle);
        auto_io_property = ESP_GATT_CHAR_PROP_BIT_WRITE ;
        esp_err_t ret = esp_ble_gatts_add_char(gl_profile_tab[AUTO_IO_PROFILE_APP_ID].service_handle, &gl_profile_tab[AUTO_IO_PROFILE_APP_ID].char_uuid,
                            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE ,
                            auto_io_property,
                            &led_status_attr, NULL);
        if (ret) {
            ESP_LOGE(GATTS_TAG, "add char failed, error code = %x", ret);
        }
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        ESP_LOGI(GATTS_TAG, "Characteristic add, status %d, attr_handle %d, char_uuid %x",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.char_uuid.uuid.uuid16);
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].char_handle = param->add_char.attr_handle;
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        ESP_LOGI(GATTS_TAG, "Descriptor add, status %d", param->add_char_descr.status);
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        break;
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(GATTS_TAG, "Characteristic read");
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));

        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 1;
        rsp.attr_value.value[0] = 0x02;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    case ESP_GATTS_WRITE_EVT:
        ESP_LOGI(GATTS_TAG, "Characteristic write, value len %u, value ", param->write.len);
        ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);
        if (param->write.value[0]) {
            ESP_LOGI(GATTS_TAG, "LED ON!");
            led_on();
        } else {
            ESP_LOGI(GATTS_TAG, "LED OFF!");
            led_off();
        }
        example_write_event_env(gatts_if, param);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "Service start, status %d, service_handle %d", param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;
        ESP_LOGI(GATTS_TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR"",
                param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        gl_profile_tab[AUTO_IO_PROFILE_APP_ID].conn_id = param->connect.conn_id;
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_TAG, "Confirm receive, status %d, attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK) {
            ESP_LOG_BUFFER_HEX(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;

        } else {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    //gatts_if registered complete, call cb handlers
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while(0);
}


void app_main(void)
{
    esp_err_t ret;

    led_init();

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
        ESP_LOGE(GATTS_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(HEART_PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "app register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(AUTO_IO_PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "app register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatt_set_local_mtu(500);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "set local  MTU failed, error code = %x", ret);
    }

    xTaskCreate(heart_rate_task, "Heart Rate", 2 * 1024, NULL, 5, NULL);
}

void example_write_event_env(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.need_rsp) {
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
    }
}
