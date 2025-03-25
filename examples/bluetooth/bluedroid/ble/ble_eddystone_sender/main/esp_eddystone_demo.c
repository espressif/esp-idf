/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


/****************************************************************************
*
* This file is used for eddystone receiver.
*
****************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "esp_bt.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"
#include "esp_gap_ble_api.h"
#include "freertos/FreeRTOS.h"

#include "esp_eddystone_protocol.h"
#include "esp_eddystone_api.h"

static const char* DEMO_TAG = "EDDYSTONE_DEMO";

/* declare static functions */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);
static void eddystone_send_raw(const esp_eddystone_result_t *res);

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void eddystone_send_raw(const esp_eddystone_result_t *res)
{
    uint8_t raw_adv_data[31] = {0};
    uint8_t index =0;

    raw_adv_data[index++] = 2; // length
    raw_adv_data[index++] = ESP_BLE_AD_TYPE_FLAG;
    raw_adv_data[index++] = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    raw_adv_data[index++] = 3; //length
    raw_adv_data[index++] = ESP_BLE_AD_TYPE_16SRV_CMPL;
    raw_adv_data[index++] = 0xAA; //uuid for eddystone
    raw_adv_data[index++] = 0xFE;

    switch(res->common.frame_type)
    {
        case EDDYSTONE_FRAME_TYPE_UID: {
            raw_adv_data[index++] = 23; //length
            raw_adv_data[index++] = ESP_BLE_AD_TYPE_SERVICE_DATA;
            raw_adv_data[index++] = 0xAA;
            raw_adv_data[index++] = 0xFE;
            uint8_t service_data[EDDYSTONE_UID_DATA_LEN+3] = {0};
            service_data[0] = EDDYSTONE_FRAME_TYPE_UID;
            service_data[1] = res->inform.uid.ranging_data;
            memcpy(&service_data[2], res->inform.uid.namespace_id, EDDYSTONE_UID_NAMESPACE_LEN);
            memcpy(&service_data[12], res->inform.uid.instance_id, EDDYSTONE_UID_INSTANCE_LEN);
            service_data[18] = 0x00;
            service_data[19] = 0x00;
            memcpy(&raw_adv_data[index], service_data, EDDYSTONE_UID_DATA_LEN+3);
            index += EDDYSTONE_UID_DATA_LEN + 3;
            break;
        }

        case EDDYSTONE_FRAME_TYPE_URL: {
            size_t url_len = strlen((char*)res->inform.url.encoded_url); //encoded url length
            if(url_len > EDDYSTONE_URL_MAX_LEN){
                url_len = EDDYSTONE_URL_MAX_LEN;
            }

            raw_adv_data[index++] = url_len+6; //length
            raw_adv_data[index++] = ESP_BLE_AD_TYPE_SERVICE_DATA;
            raw_adv_data[index++] = 0xAA;
            raw_adv_data[index++] = 0xFE;
            uint8_t service_data[EDDYSTONE_URL_MAX_LEN+2] = {0};
            service_data[0] = EDDYSTONE_FRAME_TYPE_URL;
            service_data[1] = res->inform.url.tx_power;
            service_data[2] = res->inform.url.url_scheme;
            memcpy(&service_data[3], res->inform.url.encoded_url, url_len);
            memcpy(&raw_adv_data[index], service_data, url_len+3);
            index += url_len+3;
            break;
        }

        case EDDYSTONE_FRAME_TYPE_TLM: {
            raw_adv_data[index++] = 17; //length
            raw_adv_data[index++] = ESP_BLE_AD_TYPE_SERVICE_DATA;
            raw_adv_data[index++] = 0xAA;
            raw_adv_data[index++] = 0xFE;
            uint8_t service_data[EDDYSTONE_TLM_DATA_LEN+1] = {0};
            service_data[0] = EDDYSTONE_FRAME_TYPE_TLM;
            service_data[1] = res->inform.tlm.version;
            service_data[2] = (res->inform.tlm.battery_voltage >> 8) & 0xFF;
            service_data[3] = res->inform.tlm.battery_voltage & 0xFF;
            service_data[4] = (res->inform.tlm.temperature >> 8) & 0xFF;
            service_data[4] = res->inform.tlm.temperature & 0xFF;
            service_data[6] = (res->inform.tlm.adv_count >> 24) & 0xFF;
            service_data[7] = (res->inform.tlm.adv_count >> 16) & 0xFF;
            service_data[8] = (res->inform.tlm.adv_count >> 8) & 0xFF;
            service_data[9] = res->inform.tlm.adv_count & 0xFF;
            service_data[10] = (res->inform.tlm.time >> 24) & 0xFF;
            service_data[11] = (res->inform.tlm.time >> 16) & 0xFF;
            service_data[12] = (res->inform.tlm.time >> 8) & 0xFF;
            service_data[13] = res->inform.tlm.time & 0xFF;
            memcpy(&raw_adv_data[index], service_data, EDDYSTONE_TLM_DATA_LEN+1);
            index += EDDYSTONE_TLM_DATA_LEN+1;
            break;
        }

        default:
            break;
    }
    esp_ble_gap_config_adv_data_raw(raw_adv_data, index);
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param)
{
    switch(event)
    {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:  {
            ESP_LOGI(DEMO_TAG,"Raw advertising data set complete");
            esp_ble_gap_start_advertising(&adv_params);
            break;
        }

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: {
            if(param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS){
                ESP_LOGI(DEMO_TAG,"Advertising started successfully");
            }else{
                ESP_LOGE(DEMO_TAG,"Failed to start advertising, error code = %x",param->adv_start_cmpl.status);
            }
            break;
        }

        default:
            break;
    }
}

void esp_eddystone_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(DEMO_TAG,"Register callback");

    /*<! register the scan callback function to the gap module */
    if((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(DEMO_TAG,"gap register error: %s", esp_err_to_name(status));
        return;
    }
}

void esp_eddystone_init(void)
{
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_eddystone_appRegister();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_eddystone_init();
    esp_eddystone_result_t eddystone_result;

#if (CONFIG_EXMAPLE_EDDYSTONE_SEND_UID)
    eddystone_result.common.frame_type= EDDYSTONE_FRAME_TYPE_UID;
    eddystone_result.inform.uid.ranging_data = 0;
    memcpy(eddystone_result.inform.uid.namespace_id, (uint8_t[]){0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A}, 10);
    memcpy(eddystone_result.inform.uid.instance_id, (uint8_t[]){0xB1,0xB2,0xB3,0xB4,0xB5,0xB6}, 6);
#endif /* #if (CONFIG_EXMAPLE_EDDYSTONE_SEND_UID) */

#if (CONFIG_EXMAPLE_EDDYSTONE_SEND_URL)
    eddystone_result.common.frame_type= EDDYSTONE_FRAME_TYPE_URL;
    eddystone_result.inform.url.tx_power = 0;
    eddystone_result.inform.url.url_scheme = 0x00;
    strcpy(eddystone_result.inform.url.encoded_url,"example.com");
#endif /* #if (CONFIG_EXMAPLE_EDDYSTONE_SEND_URL) */

#if (CONFIG_EXMAPLE_EDDYSTONE_SEND_TLM)
    eddystone_result.common.frame_type = EDDYSTONE_FRAME_TYPE_TLM;
    eddystone_result.inform.tlm.version = 0x00;
    eddystone_result.inform.tlm.battery_voltage = 3000;
    eddystone_result.inform.tlm.temperature = 220;
    eddystone_result.inform.tlm.adv_count = 1000;
    eddystone_result.inform.tlm.time = 50000;
#endif /* #if (CONFIG_EXMAPLE_EDDYSTONE_SEND_TLM) */

    eddystone_send_raw(&eddystone_result);
}
