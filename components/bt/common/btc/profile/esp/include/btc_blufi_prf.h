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

#ifndef __BTC_BLUFI_PRF_H__
#define __BTC_BLUFI_PRF_H__

#include "blufi_int.h"
#include "btc/btc_task.h"
#include "esp_blufi_api.h"
#include "esp_err.h"

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "stack/gatt_api.h"
#define ESP_BLUFI_ERROR GATT_ERROR
#define ESP_BLUFI_SUCCESS GATT_SUCCESS
#else
#define ESP_BLUFI_ERROR 0x85
#define ESP_BLUFI_SUCCESS 0x00
#endif

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

#define GATT_UUID_CHAR_CLIENT_CONFIG    0x2902      /*  Client Characteristic Configuration */
//define the blufi serivce uuid
#define BLUFI_SERVICE_UUID  0xFFFF
//define the blufi Char uuid (PHONE to ESP32)
#define BLUFI_CHAR_P2E_UUID 0xFF01
//define the blufi Char uuid (ESP32 to PHONE)
#define BLUFI_CHAR_E2P_UUID 0xFF02
//define the blufi Descriptor uuid (ESP32 to PHONE)
#define BLUFI_DESCR_E2P_UUID GATT_UUID_CHAR_CLIENT_CONFIG
//define the blufi APP ID
#define BLUFI_APP_UUID      0xFFFF

#define BLUFI_HDL_NUM   6

struct pkt_info{
   uint8_t *pkt;
   int pkt_len;
};

static  const  tBT_UUID blufi_srvc_uuid = {LEN_UUID_16, {BLUFI_SERVICE_UUID}};
static  const  tBT_UUID blufi_char_uuid_p2e = {LEN_UUID_16, {BLUFI_CHAR_P2E_UUID}};
static  const  tBT_UUID blufi_char_uuid_e2p = {LEN_UUID_16, {BLUFI_CHAR_E2P_UUID}};
static  const  tBT_UUID blufi_descr_uuid_e2p = {LEN_UUID_16, {BLUFI_DESCR_E2P_UUID}};
static  const  tBT_UUID blufi_app_uuid = {LEN_UUID_16, {BLUFI_APP_UUID}};

typedef enum {
    BTC_BLUFI_ACT_INIT = 0,
    BTC_BLUFI_ACT_DEINIT,
    BTC_BLUFI_ACT_SEND_CFG_REPORT,
    BTC_BLUFI_ACT_SEND_WIFI_LIST,
    BTC_BLUFI_ACT_SEND_ERR_INFO,
    BTC_BLUFI_ACT_SEND_CUSTOM_DATA,
} btc_blufi_act_t;

typedef union {
    struct blufi_cfg_report {
        wifi_mode_t opmode;
        esp_blufi_sta_conn_state_t sta_conn_state;
        uint8_t softap_conn_num;
        esp_blufi_extra_info_t *extra_info;
        int extra_info_len;
    } wifi_conn_report;
    /*
        BTC_BLUFI_ACT_SEND_WIFI_LIST
    */
    struct blufi_wifi_list {
        uint16_t apCount;
        esp_blufi_ap_record_t *list;
    } wifi_list;
    /*
        BTC_BLUFI_ACT_SEND_ERR_INFO
    */
    struct blufi_error_infor {
        esp_blufi_error_state_t state;
    } blufi_err_infor;
    /*
        BTC_BLUFI_ACT_SEND_CUSTOM_DATA
    */
    struct blufi_custom_data {
         uint8_t *data;
         uint32_t data_len;
    } custom_data;
} btc_blufi_args_t;

void btc_blufi_cb_to_app(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);
void btc_blufi_cb_handler(btc_msg_t *msg);
void btc_blufi_call_handler(btc_msg_t *msg);
void btc_blufi_set_callbacks(esp_blufi_callbacks_t *callbacks);

void btc_blufi_recv_handler(uint8_t *data, int len);
void btc_blufi_send_notify(uint8_t *pkt, int pkt_len);
void btc_blufi_call_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_blufi_call_deep_free(btc_msg_t *msg);

uint16_t btc_blufi_get_version(void);

#endif /* __BTC_BLUFI_PRF_H__ */
