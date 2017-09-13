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
#include "esp_bt_main.h"
#include "btc_manage.h"
#include "btc_gattc.h"
#include "btc_gatt_util.h"

#if (GATTC_INCLUDED == TRUE)
esp_err_t esp_ble_gattc_register_callback(esp_gattc_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

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

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (app_id > ESP_APP_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_REGISTER;
    arg.app_reg.app_id = app_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gattc_if)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_UNREGISTER;
    arg.app_unreg.gattc_if = gattc_if;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda, bool is_direct)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_OPEN;
    arg.open.gattc_if = gattc_if;
    memcpy(arg.open.remote_bda, remote_bda, ESP_BD_ADDR_LEN);
    arg.open.is_direct = is_direct;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_close (esp_gatt_if_t gattc_if, uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CLOSE;
    arg.close.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_send_mtu_req (esp_gatt_if_t gattc_if, uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CFG_MTU;
    arg.cfg_mtu.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_search_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *filter_uuid)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_SEARCH_SERVICE;
    arg.search_srvc.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);

    if (filter_uuid) {
        arg.search_srvc.filter_uuid_enable = true;
        memcpy(&arg.search_srvc.filter_uuid, filter_uuid, sizeof(esp_bt_uuid_t));
    } else {
        arg.search_srvc.filter_uuid_enable = false;
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_gatt_status_t esp_ble_gattc_get_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *svc_uuid,
                                            esp_gattc_service_elem_t *result, uint16_t *count, uint16_t offset)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_service(conn_hdl, svc_uuid, result, count, offset);
}


esp_gatt_status_t esp_ble_gattc_get_all_char(esp_gatt_if_t gattc_if,
                                             uint16_t conn_id,
                                             uint16_t start_handle,
                                             uint16_t end_handle,
                                             esp_gattc_char_elem_t *result,
                                             uint16_t *count, uint16_t offset)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((start_handle == 0) && (end_handle == 0)) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_all_char(conn_hdl, start_handle, end_handle, result, count, offset);
}

esp_gatt_status_t esp_ble_gattc_get_all_descr(esp_gatt_if_t gattc_if,
                                              uint16_t conn_id,
                                              uint16_t char_handle,                                            
                                              esp_gattc_descr_elem_t *result,
                                              uint16_t *count, uint16_t offset)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (char_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_all_descr(conn_hdl, char_handle, result, count, offset);
}

esp_gatt_status_t esp_ble_gattc_get_char_by_uuid(esp_gatt_if_t gattc_if, 
                                                 uint16_t conn_id,
                                                 uint16_t start_handle,
                                                 uint16_t end_handle,
                                                 esp_bt_uuid_t char_uuid,
                                                 esp_gattc_char_elem_t *result,
                                                 uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (start_handle == 0 && end_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if,conn_id);
    return btc_ble_gattc_get_char_by_uuid(conn_hdl, start_handle, end_handle, char_uuid, result, count);
}


esp_gatt_status_t esp_ble_gattc_get_descr_by_uuid(esp_gatt_if_t gattc_if,
                                                  uint16_t conn_id,
                                                  uint16_t start_handle,
                                                  uint16_t end_handle,
                                                  esp_bt_uuid_t char_uuid,
                                                  esp_bt_uuid_t descr_uuid,
                                                  esp_gattc_descr_elem_t *result,
                                                  uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_descr_by_uuid(conn_hdl, start_handle, end_handle, char_uuid, descr_uuid, result, count);
}

esp_gatt_status_t esp_ble_gattc_get_descr_by_char_handle(esp_gatt_if_t gattc_if,
                                                         uint16_t conn_id,
                                                         uint16_t char_handle,
                                                         esp_bt_uuid_t descr_uuid,
                                                         esp_gattc_descr_elem_t *result,
                                                         uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (char_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }
    
    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_descr_by_char_handle(conn_hdl, char_handle, descr_uuid, result, count);
}

esp_gatt_status_t esp_ble_gattc_get_include_service(esp_gatt_if_t gattc_if,
                                                    uint16_t conn_id,
                                                    uint16_t start_handle,
                                                    uint16_t end_handle,
                                                    esp_bt_uuid_t *incl_uuid,
                                                    esp_gattc_incl_svc_elem_t *result,
                                                    uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (start_handle == 0 && end_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_include_service(conn_hdl, start_handle, end_handle, incl_uuid, result, count);
}

esp_gatt_status_t esp_ble_gattc_get_attr_count(esp_gatt_if_t gattc_if,
                                               uint16_t conn_id,
                                               esp_gatt_db_attr_type_t type,
                                               uint16_t start_handle,
                                               uint16_t end_handle,
                                               uint16_t char_handle,
                                               uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((start_handle == 0 && end_handle == 0) && (type != ESP_GATT_DB_DESCRIPTOR)) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (count == NULL) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_attr_count(conn_hdl, type, start_handle, end_handle, char_handle, count);
}

esp_gatt_status_t esp_ble_gattc_get_db(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
                                       esp_gattc_db_elem_t *db, uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (start_handle == 0 && end_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (db == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_db(conn_hdl, start_handle, end_handle, db, count);
}


esp_err_t esp_ble_gattc_read_char (esp_gatt_if_t gattc_if, 
                                   uint16_t conn_id, uint16_t handle,
                                   esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR;
    arg.read_char.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_char.handle = handle;
    arg.read_char.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_multiple(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id, esp_gattc_multi_t *read_multi,
                                      esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_MULTIPLE_CHAR;
    arg.read_multiple.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_multiple.num_attr = read_multi->num_attr;
    arg.read_multiple.auth_req = auth_req;

    if (read_multi->num_attr > 0) {
        memcpy(arg.read_multiple.handles, read_multi->handles, sizeof(uint16_t)*read_multi->num_attr);
    } else {
        LOG_ERROR("%s(), the num_attr should not be 0.", __func__);
        return ESP_FAIL;
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gattc_read_char_descr (esp_gatt_if_t gattc_if,
                                         uint16_t conn_id, uint16_t handle,
                                         esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR_DESCR;
    arg.read_descr.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_descr.handle = handle;
    arg.read_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_write_char(esp_gatt_if_t gattc_if,
                                   uint16_t conn_id, uint16_t handle,
                                   uint16_t value_len,
                                   uint8_t *value,
                                   esp_gatt_write_type_t write_type,
                                   esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR;
    arg.write_char.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.write_char.handle = handle;
    arg.write_char.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    arg.write_char.value = value;
    arg.write_char.write_type = write_type;
    arg.write_char.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_write_char_descr (esp_gatt_if_t gattc_if,
                                          uint16_t conn_id, uint16_t handle,
                                          uint16_t value_len,
                                          uint8_t *value,
                                          esp_gatt_write_type_t write_type,
                                          esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR_DESCR;
    arg.write_descr.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.write_descr.handle = handle;
    arg.write_descr.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    arg.write_descr.value = value;
    arg.write_descr.write_type = write_type;
    arg.write_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_prepare_write(esp_gatt_if_t gattc_if, 
                                      uint16_t conn_id, uint16_t handle,
                                      uint16_t offset,
                                      uint16_t value_len,
                                      uint8_t *value,
                                      esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_PREPARE_WRITE;
    arg.prep_write.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.prep_write.handle = handle;
    arg.prep_write.offset = offset;
    arg.prep_write.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len; // length check ?
    arg.prep_write.value = value;
    arg.prep_write.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_prepare_write_char_descr(esp_gatt_if_t gattc_if,
                                                 uint16_t conn_id, uint16_t handle,
                                                 uint16_t offset,
                                                 uint16_t value_len,
                                                 uint8_t *value,
                                                 esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR;
    arg.prep_write_descr.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.prep_write_descr.handle = handle;
    arg.prep_write_descr.offset = offset;
    arg.prep_write_descr.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len; // length check ?
    arg.prep_write_descr.value = value;
    arg.prep_write_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_execute_write (esp_gatt_if_t gattc_if, uint16_t conn_id, bool is_execute)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_EXECUTE_WRITE;
    arg.exec_write.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.exec_write.is_execute = is_execute;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_register_for_notify (esp_gatt_if_t gattc_if,
                                             esp_bd_addr_t server_bda, uint16_t handle)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_REG_FOR_NOTIFY;
    arg.reg_for_notify.gattc_if = gattc_if;
    memcpy(arg.reg_for_notify.remote_bda, server_bda, sizeof(esp_bd_addr_t));
    arg.reg_for_notify.handle = handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_unregister_for_notify (esp_gatt_if_t gattc_if,
                                               esp_bd_addr_t server_bda, uint16_t handle)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_UNREG_FOR_NOTIFY;
    arg.unreg_for_notify.gattc_if = gattc_if;
    arg.unreg_for_notify.handle = handle;
    memcpy(arg.unreg_for_notify.remote_bda, server_bda, sizeof(esp_bd_addr_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_cache_refresh(esp_bd_addr_t remote_bda)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CACHE_REFRESH;
    memcpy(arg.cache_refresh.remote_bda, remote_bda, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif  ///GATTC_INCLUDED == TRUE

