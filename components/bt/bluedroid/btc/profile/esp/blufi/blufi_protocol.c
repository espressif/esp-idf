// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>


#include "bt_target.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "gatt_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "bta_gatts_int.h"

#include "btc_blufi_prf.h"
#include "btc_task.h"
#include "btc_manage.h"

#include "blufi_int.h"

#include "esp_wifi.h"

#if (GATTS_INCLUDED == TRUE)
extern tBLUFI_ENV blufi_env;

void btc_blufi_protocol_handler(uint8_t type, uint8_t *data, int len)
{
    btc_msg_t msg;
    esp_blufi_cb_param_t param;
    uint8_t *output_data = NULL;
    int output_len = 0;
    bool need_free = false;

    switch (BLUFI_GET_TYPE(type)) {
    case BLUFI_TYPE_CTRL:
        switch (BLUFI_GET_SUBTYPE(type)) {
        case BLUFI_TYPE_CTRL_SUBTYPE_ACK:
            /* TODO: check sequence */
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_SET_SEC_MODE:
            blufi_env.sec_mode = data[0];
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_SET_WIFI_OPMODE:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_SET_WIFI_OPMODE;
            param.wifi_mode.op_mode = data[0];

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_CONN_TO_AP:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP;

            btc_transfer_context(&msg, NULL, 0, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_DISCONN_FROM_AP:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP;

            btc_transfer_context(&msg, NULL, 0, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_WIFI_STATUS:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_GET_WIFI_STATUS;

            btc_transfer_context(&msg, NULL, 0, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_DEAUTHENTICATE_STA:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_DEAUTHENTICATE_STA;

            btc_transfer_context(&msg, NULL, 0, NULL);
            break;
        case BLUFI_TYPE_CTRL_SUBTYPE_GET_VERSION: {
            uint8_t type = BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_REPLY_VERSION);
            uint8_t data[2];

            data[0] = BTC_BLUFI_GREAT_VER;
            data[1] = BTC_BLUFI_SUB_VER;
            btc_blufi_send_encap(type, &data[0], sizeof(data));
            break;
        }
        case BLUFI_TYPE_CTRL_SUBTYPE_DISCONNECT_BLE:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE;
            btc_transfer_context(&msg, NULL, 0, NULL);
            break;
        default:
            LOG_ERROR("%s Unkown Ctrl pkt %02x\n", __func__, type);
            break;
        }
        break;
    case BLUFI_TYPE_DATA:
        switch (BLUFI_GET_SUBTYPE(type)) {
        case BLUFI_TYPE_DATA_SUBTYPE_NEG:
            if (blufi_env.cbs && blufi_env.cbs->negotiate_data_handler) {
                blufi_env.cbs->negotiate_data_handler(data, len, &output_data, &output_len, &need_free);
            }

            if (output_data && output_len > 0) {
                btc_blufi_send_encap(BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_NEG),
                             output_data, output_len);
            }
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_STA_BSSID;
            memcpy(param.sta_bssid.bssid, &data[0], 6);

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_SSID:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_STA_SSID;
            param.sta_ssid.ssid = &data[0];
            param.sta_ssid.ssid_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_STA_PASSWD;
            param.sta_passwd.passwd = &data[0];
            param.sta_passwd.passwd_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_SSID;
            param.softap_ssid.ssid = &data[0];
            param.softap_ssid.ssid_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD;
            param.softap_passwd.passwd = &data[0];
            param.softap_passwd.passwd_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM;
            param.softap_max_conn_num.max_conn_num = data[0];

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE;
            param.softap_auth_mode.auth_mode = data[0];

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL;
            param.softap_channel.channel = data[0];

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_USERNAME:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_USERNAME;
            param.username.name = &data[0];
            param.username.name_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CA:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CA_CERT;
            param.ca.cert = &data[0];
            param.ca.cert_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CLIENT_CERT:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CLIENT_CERT;
            param.client_cert.cert = &data[0];
            param.client_cert.cert_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SERVER_CERT:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SERVER_CERT;
            param.client_cert.cert = &data[0];
            param.client_cert.cert_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_CLIENT_PRIV_KEY:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY;
            param.client_pkey.pkey = &data[0];
            param.client_pkey.pkey_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        case BLUFI_TYPE_DATA_SUBTYPE_SERVER_PRIV_KEY:
            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY;
            param.client_pkey.pkey = &data[0];
            param.client_pkey.pkey_len = len;

            btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), btc_blufi_cb_deep_copy);
            break;
        default:
            LOG_ERROR("%s Unkown Ctrl pkt %02x\n", __func__, type);
            break;
        }
        break;
    default:
        break;
    }
}

#endif	///(GATTS_INCLUDED == TRUE)
