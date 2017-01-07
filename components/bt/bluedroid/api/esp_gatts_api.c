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

#include "string.h"
#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"
#include "btc_manage.h"
#include "btc_gatts.h"
#include "btc_gatt_util.h"

#define COPY_TO_GATTS_ARGS(_gatt_args, _arg, _arg_type) memcpy(_gatt_args, _arg, sizeof(_arg_type))

esp_err_t esp_ble_gatts_register_callback(esp_gatts_cb_t callback)
{
    return (btc_profile_cb_set(BTC_PID_GATTS, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_app_register(uint16_t app_id)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    //if (app_id < ESP_APP_ID_MIN || app_id > ESP_APP_ID_MAX) {
    if (app_id > ESP_APP_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_APP_REGISTER;
    arg.app_reg.app_id = app_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatts_if)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_APP_UNREGISTER;
    arg.app_unreg.gatts_if = gatts_if;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_create_service(esp_gatt_if_t gatts_if,
                                       esp_gatt_srvc_id_t *service_id, uint16_t num_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CREATE_SERVICE;
    arg.create_srvc.gatts_if = gatts_if;
    arg.create_srvc.num_handle = num_handle;
    memcpy(&arg.create_srvc.service_id, service_id, sizeof(esp_gatt_srvc_id_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_add_included_service(uint16_t service_handle, uint16_t included_service_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_ADD_INCLUDE_SERVICE;
    arg.add_incl_srvc.service_handle = service_handle;
    arg.add_incl_srvc.included_service_handle = included_service_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_add_char(uint16_t service_handle,  esp_bt_uuid_t  *char_uuid,
                                 esp_gatt_perm_t perm, esp_gatt_char_prop_t property)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_ADD_CHAR;
    arg.add_char.service_handle = service_handle;
    arg.add_char.perm = perm;
    arg.add_char.property = property;
    memcpy(&arg.add_char.char_uuid, char_uuid, sizeof(esp_bt_uuid_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_add_char_descr (uint16_t service_handle,
                                        esp_bt_uuid_t   *descr_uuid,
                                        esp_gatt_perm_t perm)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_ADD_CHAR_DESCR;
    arg.add_descr.service_handle = service_handle;
    arg.add_descr.perm = perm;
    memcpy(&arg.add_descr.descr_uuid, descr_uuid, sizeof(esp_bt_uuid_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_DELETE_SERVICE;
    arg.delete_srvc.service_handle = service_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_start_service(uint16_t service_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_START_SERVICE;
    arg.start_srvc.service_handle = service_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_stop_service(uint16_t service_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_STOP_SERVICE;
    arg.stop_srvc.service_handle = service_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_send_indicate(esp_gatt_if_t gatts_if, uint16_t conn_id, uint16_t attr_handle,
                                      uint16_t value_len, uint8_t *value, bool need_confirm)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_SEND_INDICATE;
    arg.send_ind.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);
    arg.send_ind.attr_handle = attr_handle;
    arg.send_ind.need_confirm = need_confirm;
    arg.send_ind.value_len = value_len;
    arg.send_ind.value = value;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_send_response(esp_gatt_if_t gatts_if, uint16_t conn_id, uint32_t trans_id,
                                      esp_gatt_status_t status, esp_gatt_rsp_t *rsp)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_SEND_RESPONSE;
    arg.send_rsp.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);
    arg.send_rsp.trans_id = trans_id;
    arg.send_rsp.status = status;
    arg.send_rsp.rsp = rsp;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_open(esp_gatt_if_t gatts_if, esp_bd_addr_t remote_bda, bool is_direct)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_OPEN;
    arg.open.gatts_if = gatts_if;
    arg.open.is_direct = is_direct;
    memcpy(&arg.open.remote_bda, remote_bda, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_close(esp_gatt_if_t gatts_if, uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CLOSE;
    arg.close.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
