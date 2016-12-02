// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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



/****************************************************************************
*
* This file is for gatt client. It can scan ble device, connect one device,
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "controller.h"

#include "bt.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "btm_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"


#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

esp_gatt_if_t client_if;
esp_gatt_status_t status = ESP_GATT_ERROR;
bool connet = false;
uint16_t simpleClient_id = 0xEE;

const char device_name[] = "Heart Rate";

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = ESP_PUBLIC_ADDR,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};


static void esp_gap_cb(uint32_t event, void *param);

static void esp_gattc_cb(uint32_t event, void *param);

static void esp_gap_cb(uint32_t event, void *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second
        uint32_t duration = 10;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            for (int i = 0; i < 6; i++) {
                LOG_INFO("%x:", scan_result->scan_rst.bda[i]);
            }
            LOG_INFO("\n");
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            LOG_INFO("adv_name_len=%x\n", adv_name_len);
            for (int j = 0; j < adv_name_len; j++) {
                LOG_INFO("%c", adv_name[j]);
            }
            LOG_INFO("\n");
            for (int j = 0; j < adv_name_len; j++) {
                LOG_INFO("%c", device_name[j]);
            }
            LOG_INFO("\n");

            if (adv_name != NULL) {
                if (strcmp((char *)adv_name, device_name) == 0) {
                    LOG_INFO("the name eque to Heart Rate.\n");
                    if (status ==  ESP_GATT_OK && connet == false) {
                        connet = true;
                        LOG_INFO("Connet to the remote device.\n");
                        esp_ble_gap_stop_scanning();
                        esp_ble_gattc_open(client_if, scan_result->scan_rst.bda, true);
                    }
                }
            }
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}


static void esp_gattc_cb(uint32_t event, void *param)
{
    uint16_t conn_id = 0;
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    LOG_INFO("esp_gattc_cb, event = %x\n", event);
    switch (event) {
    case ESP_GATTC_REG_EVT:
        status = p_data->reg.status;
        client_if = p_data->reg.gatt_if;
        LOG_INFO("status = %x, client_if = %x\n", status, client_if);
        break;
    case ESP_GATTC_OPEN_EVT:
        conn_id = p_data->open.conn_id;
        LOG_INFO("ESP_GATTC_OPEN_EVT conn_id %d, if %d, status %d\n", conn_id, p_data->open.gatt_if, p_data->open.status);
        esp_ble_gattc_search_service(conn_id, NULL);
        break;
    case ESP_GATTC_SEARCH_RES_EVT: {
        esp_gatt_srvc_id_t *srvc_id = &p_data->search_res.srvc_id;
        conn_id = p_data->open.conn_id;
        LOG_INFO("SEARCH RES: conn_id = %x\n", conn_id);
        if (srvc_id->id.uuid.len == ESP_UUID_LEN_16) {
            LOG_INFO("UUID16: %x\n", srvc_id->id.uuid.uuid.uuid16);
        } else if (srvc_id->id.uuid.len == ESP_UUID_LEN_32) {
            LOG_INFO("UUID32: %x\n", srvc_id->id.uuid.uuid.uuid32);
        } else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128) {
            LOG_INFO("UUID128: %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n", srvc_id->id.uuid.uuid.uuid128[0],
                     srvc_id->id.uuid.uuid.uuid128[1], srvc_id->id.uuid.uuid.uuid128[2], srvc_id->id.uuid.uuid.uuid128[3],
                     srvc_id->id.uuid.uuid.uuid128[4], srvc_id->id.uuid.uuid.uuid128[5], srvc_id->id.uuid.uuid.uuid128[6],
                     srvc_id->id.uuid.uuid.uuid128[7], srvc_id->id.uuid.uuid.uuid128[8], srvc_id->id.uuid.uuid.uuid128[9],
                     srvc_id->id.uuid.uuid.uuid128[10], srvc_id->id.uuid.uuid.uuid128[11], srvc_id->id.uuid.uuid.uuid128[12],
                     srvc_id->id.uuid.uuid.uuid128[13], srvc_id->id.uuid.uuid.uuid128[14], srvc_id->id.uuid.uuid.uuid128[15]);
        } else {
            LOG_ERROR("UNKNOWN LEN %d\n", srvc_id->id.uuid.len);
        }
        break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
        conn_id = p_data->search_cmpl.conn_id;
        LOG_INFO("SEARCH_CMPL: conn_id = %x, status %d\n", conn_id, p_data->search_cmpl.status);
        break;
    default:
        break;
    }
}

void ble_client_appRegister(void)
{
    LOG_INFO("register callback\n");

    //register the scan callback function to the gap moudule
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        LOG_ERROR("gap register error, error code = %x\n", status);
        return;
    }

    //register the callback function to the gattc module
    if ((status = esp_ble_gattc_register_callback(esp_gattc_cb)) != ESP_OK) {
        LOG_ERROR("gattc register error, error code = %x\n", status);
        return;
    }
    esp_ble_gattc_app_register(simpleClient_id);
    esp_ble_gap_set_scan_params(&ble_scan_params);
}

void gattc_client_test(void)
{
    esp_init_bluetooth();
    esp_enable_bluetooth();
    ble_client_appRegister();
}

void app_main()
{
    bt_controller_init();
    gattc_client_test();
}

