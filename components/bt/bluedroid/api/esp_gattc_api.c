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

#include <string.h>

#include "esp_gattc_api.h"
#include "btc_manage.h"
#include "btc_gattc.h"

esp_err_t esp_ble_gattc_register_callback(esp_profile_cb_t callback)
{
    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_GATTC, callback);
    return ESP_OK;
}

esp_err_t esp_ble_gattc_app_register(uint16_t app_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    //if (app_id < ESP_APP_ID_MIN || app_id > ESP_APP_ID_MAX) {
    if (app_id > ESP_APP_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_REGISTER;
    arg.app_reg.app_id = app_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gatt_if)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_UNREGISTER;
    arg.app_unreg.gatt_if = gatt_if;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_open(esp_gatt_if_t gatt_if, esp_bd_addr_t remote_bda, bool is_direct)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_OPEN;
    arg.open.gatt_if = gatt_if;
    memcpy(arg.open.remote_bda, remote_bda, ESP_BD_ADDR_LEN);
    arg.open.is_direct = is_direct;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_close (uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CLOSE;
    arg.close.conn_id = conn_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_config_mtu (uint16_t conn_id, uint16_t mtu)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    if ((mtu < ESP_GATT_DEF_BLE_MTU_SIZE) || (mtu > ESP_GATT_MAX_MTU_SIZE)) {
        return ESP_GATT_ILLEGAL_PARAMETER;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CFG_MTU;
    arg.cfg_mtu.conn_id = conn_id;
    arg.cfg_mtu.mtu = mtu;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_search_service(uint16_t conn_id, esp_bt_uuid_t *filter_uuid)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_SEARCH_SERVICE;
    arg.search_srvc.conn_id = conn_id;
    if (filter_uuid) {
        arg.search_srvc.filter_uuid_enable = true;
        memcpy(&arg.search_srvc.filter_uuid, filter_uuid, sizeof(esp_bt_uuid_t));
    } else {
        arg.search_srvc.filter_uuid_enable = false;
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_get_characteristic(uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *start_char_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    if (start_char_id) {
        arg.get_next_char.conn_id = conn_id;
        memcpy(&arg.get_next_char.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
        memcpy(&arg.get_next_char.char_id, start_char_id, sizeof(esp_gatt_id_t));
        msg.act = BTC_GATTC_ACT_GET_NEXT_CHAR;
    } else {
        arg.get_first_char.conn_id = conn_id;
        memcpy(&arg.get_first_char.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
        msg.act = BTC_GATTC_ACT_GET_FIRST_CHAR;
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_get_descriptor(uint16_t conn_id,
                                       esp_gatt_srvc_id_t *srvc_id,
                                       esp_gatt_id_t *char_id,
                                       esp_gatt_id_t *start_descr_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;

    if (start_descr_id) {
        arg.get_next_descr.conn_id = conn_id;
        memcpy(&arg.get_next_descr.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
        memcpy(&arg.get_next_descr.char_id, char_id, sizeof(esp_gatt_id_t));
        memcpy(&arg.get_next_descr.descr_id, start_descr_id, sizeof(esp_gatt_id_t));
        msg.act = BTC_GATTC_ACT_GET_NEXT_DESCR;
    } else {
        arg.get_first_descr.conn_id = conn_id;
        memcpy(&arg.get_first_descr.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
        memcpy(&arg.get_first_descr.char_id, char_id, sizeof(esp_gatt_id_t));
        msg.act = BTC_GATTC_ACT_GET_FIRST_DESCR;
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_get_included_service(uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_srvc_id_t *start_incl_srvc_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;

    if (start_incl_srvc_id) {
        arg.get_next_incl_srvc.conn_id = conn_id;
        memcpy(&arg.get_next_incl_srvc.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
        memcpy(&arg.get_next_incl_srvc.start_service_id, start_incl_srvc_id, sizeof(esp_gatt_srvc_id_t));
        msg.act = BTC_GATTC_ACT_GET_NEXT_INCL_SERVICE;
    } else {
        arg.get_first_incl_srvc.conn_id = conn_id;
        memcpy(&arg.get_first_incl_srvc.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
        msg.act = BTC_GATTC_ACT_GET_FIRST_INCL_SERVICE;
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_char (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                   esp_gatt_id_t *char_id, esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR;
    arg.read_char.conn_id = conn_id;
    memcpy(&arg.read_char.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.read_char.char_id, char_id, sizeof(esp_gatt_id_t));
    arg.read_char.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_char_descr (uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t  *char_id,
        esp_gatt_id_t  *descr_id,
        esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR_DESCR;
    arg.read_descr.conn_id = conn_id;
    memcpy(&arg.read_descr.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.read_descr.char_id, char_id, sizeof(esp_gatt_id_t));
    memcpy(&arg.read_descr.descr_id, descr_id, sizeof(esp_gatt_id_t));
    arg.read_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_write_char( uint16_t conn_id,
                                    esp_gatt_srvc_id_t *srvc_id,
                                    esp_gatt_id_t *char_id,
                                    uint16_t value_len,
                                    uint8_t *value,
									esp_gatt_write_type_t write_type,
                                    esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR;
    arg.write_char.conn_id = (uint16_t) conn_id;
    memcpy(&arg.write_char.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.write_char.char_id, char_id, sizeof(esp_gatt_id_t));
    arg.write_char.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    arg.write_char.value = value;
    arg.write_char.write_type = write_type;
    arg.write_char.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_write_char_descr (uint16_t conn_id,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *char_id,
        esp_gatt_id_t *descr_id,
        uint16_t value_len,
        uint8_t *value,
		esp_gatt_write_type_t write_type,
        esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR_DESCR;
    arg.write_descr.conn_id = (uint16_t) conn_id;
    memcpy(&arg.write_descr.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.write_descr.char_id, char_id, sizeof(esp_gatt_id_t));
    memcpy(&arg.write_descr.descr_id, descr_id, sizeof(esp_gatt_id_t));
    arg.write_descr.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    arg.write_descr.value = value;
    arg.write_descr.write_type = write_type;
    arg.write_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_prepare_write(uint16_t conn_id,
                                      esp_gatt_srvc_id_t *srvc_id,
                                      esp_gatt_id_t *char_id,
                                      uint16_t offset,
                                      uint16_t value_len,
                                      uint8_t *value,
                                      esp_gatt_auth_req_t auth_req)
{
    //TODO: Review this function
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_PREPARE_WRITE;
    arg.prep_write.conn_id = conn_id;
    memcpy(&arg.prep_write.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.prep_write.char_id, char_id, sizeof(esp_gatt_id_t));
    arg.prep_write.offset = offset;
    arg.prep_write.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len; // length check ?
    arg.prep_write.value = value;
    arg.prep_write.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_execute_write (uint16_t conn_id, bool is_execute)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_EXECUTE_WRITE;
    arg.exec_write.conn_id = conn_id;
    arg.exec_write.is_execute = is_execute;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_gatt_status_t esp_ble_gattc_register_for_notify (esp_gatt_if_t gatt_if,
        esp_bd_addr_t server_bda,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *char_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_REG_FOR_NOTIFY;
    arg.reg_for_notify.gatt_if = gatt_if;
    memcpy(arg.reg_for_notify.remote_bda, server_bda, sizeof(esp_bd_addr_t));
    memcpy(&arg.reg_for_notify.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.reg_for_notify.char_id, char_id, sizeof(esp_gatt_id_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_gatt_status_t esp_ble_gattc_unregister_for_notify (esp_gatt_if_t gatt_if,
        esp_bd_addr_t server_bda,
        esp_gatt_srvc_id_t *srvc_id,
        esp_gatt_id_t *char_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_UNREG_FOR_NOTIFY;
    arg.unreg_for_notify.gatt_if = gatt_if;
    memcpy(arg.unreg_for_notify.remote_bda, server_bda, sizeof(esp_bd_addr_t));
    memcpy(&arg.unreg_for_notify.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.unreg_for_notify.char_id, char_id, sizeof(esp_gatt_id_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

