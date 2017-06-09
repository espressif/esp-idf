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
#include "esp_bt_main.h"
#include "btc_manage.h"
#include "btc_gatts.h"
#include "btc_gatt_util.h"
#include "bt_target.h"
#if (GATTS_INCLUDED == TRUE)
#define COPY_TO_GATTS_ARGS(_gatt_args, _arg, _arg_type) memcpy(_gatt_args, _arg, sizeof(_arg_type))

static esp_err_t esp_ble_gatts_add_char_desc_param_check(esp_attr_value_t *char_val, esp_attr_control_t *control);


esp_err_t esp_ble_gatts_register_callback(esp_gatts_cb_t callback)
{
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
        return ESP_ERR_INVALID_STATE;
    }
    return (btc_profile_cb_set(BTC_PID_GATTS, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_app_register(uint16_t app_id)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
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

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
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

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CREATE_SERVICE;
    arg.create_srvc.gatts_if = gatts_if;
    arg.create_srvc.num_handle = num_handle;
    memcpy(&arg.create_srvc.service_id, service_id, sizeof(esp_gatt_srvc_id_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_create_attr_tab(const esp_gatts_attr_db_t *gatts_attr_db,
                                        esp_gatt_if_t gatts_if,
                                        uint8_t max_nb_attr,
                                        uint8_t srvc_inst_id)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CREATE_ATTR_TAB;
    arg.create_attr_tab.gatts_if = gatts_if;
    arg.create_attr_tab.max_nb_attr = max_nb_attr;
    arg.create_attr_tab.srvc_inst_id = srvc_inst_id;
    arg.create_attr_tab.gatts_attr_db = (esp_gatts_attr_db_t *)gatts_attr_db;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), btc_gatts_arg_deep_copy)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_add_included_service(uint16_t service_handle, uint16_t included_service_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_ADD_INCLUDE_SERVICE;
    arg.add_incl_srvc.service_handle = service_handle;
    arg.add_incl_srvc.included_service_handle = included_service_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_add_char(uint16_t service_handle,  esp_bt_uuid_t  *char_uuid,
                                 esp_gatt_perm_t perm, esp_gatt_char_prop_t property, esp_attr_value_t *char_val,
                                 esp_attr_control_t *control)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;
    esp_err_t status;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    /* parameter validation check */
    status = esp_ble_gatts_add_char_desc_param_check(char_val, control);
    if (status != ESP_OK){
        return status;
    }

    memset(&arg, 0, sizeof(btc_ble_gatts_args_t));
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_ADD_CHAR;
    arg.add_char.service_handle = service_handle;
    arg.add_char.perm = perm;
    arg.add_char.property = property;
    if (char_val != NULL) {
        arg.add_char.char_val.attr_max_len = char_val->attr_max_len;
        arg.add_char.char_val.attr_len = char_val->attr_len;
        arg.add_char.char_val.attr_value = char_val->attr_value;
    }

    if (control != NULL) {
        arg.add_char.attr_control.auto_rsp = control->auto_rsp;
    }
    memcpy(&arg.add_char.char_uuid, char_uuid, sizeof(esp_bt_uuid_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gatts_add_char_descr (uint16_t service_handle,
                                        esp_bt_uuid_t   *descr_uuid,
                                        esp_gatt_perm_t perm, esp_attr_value_t *char_descr_val,
                                        esp_attr_control_t *control)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;
    esp_err_t status;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    /* parameter validation check */
    status = esp_ble_gatts_add_char_desc_param_check(char_descr_val, control);
    if (status != ESP_OK){
        return status;
    }
    
    memset(&arg, 0, sizeof(btc_ble_gatts_args_t));
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_ADD_CHAR_DESCR;
    arg.add_descr.service_handle = service_handle;
    arg.add_descr.perm = perm;

    if (char_descr_val != NULL) {
        arg.add_descr.descr_val.attr_max_len = char_descr_val->attr_max_len;
        arg.add_descr.descr_val.attr_len = char_descr_val->attr_len;
        arg.add_descr.descr_val.attr_value = char_descr_val->attr_value;
    }

    if (control != NULL) {
        arg.add_descr.attr_control.auto_rsp = control->auto_rsp;
    }
    memcpy(&arg.add_descr.descr_uuid, descr_uuid, sizeof(esp_bt_uuid_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
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

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
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

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
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

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_SEND_INDICATE;
    arg.send_ind.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);
    arg.send_ind.attr_handle = attr_handle;
    arg.send_ind.need_confirm = need_confirm;
    arg.send_ind.value_len = value_len;
    arg.send_ind.value = value;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t),
                                 btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_send_response(esp_gatt_if_t gatts_if, uint16_t conn_id, uint32_t trans_id,
                                      esp_gatt_status_t status, esp_gatt_rsp_t *rsp)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_SEND_RESPONSE;
    arg.send_rsp.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);
    arg.send_rsp.trans_id = trans_id;
    arg.send_rsp.status = status;
    arg.send_rsp.rsp = rsp;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t),
                                 btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_set_attr_value(uint16_t attr_handle, uint16_t length, const uint8_t *value)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_SET_ATTR_VALUE;
    arg.set_attr_val.handle = attr_handle;
    arg.set_attr_val.length = length;
    arg.set_attr_val.value  = (uint8_t *)value;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t),
                                 btc_gatts_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_get_attr_value(uint16_t attr_handle, uint16_t *length, const uint8_t **value)
{
    if (attr_handle == ESP_GATT_ILLEGAL_HANDLE) {
        return ESP_FAIL;
    }
    btc_gatts_get_attr_value(attr_handle, length, (uint8_t **)value);
    return ESP_OK;
}

esp_err_t esp_ble_gatts_open(esp_gatt_if_t gatts_if, esp_bd_addr_t remote_bda, bool is_direct)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_OPEN;
    arg.open.gatts_if = gatts_if;
    arg.open.is_direct = is_direct;
    memcpy(&arg.open.remote_bda, remote_bda, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gatts_close(esp_gatt_if_t gatts_if, uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CLOSE;
    arg.close.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


static esp_err_t esp_ble_gatts_add_char_desc_param_check(esp_attr_value_t *char_val, esp_attr_control_t *control)
{
    if ((control != NULL) && ((control->auto_rsp != ESP_GATT_AUTO_RSP) && (control->auto_rsp != ESP_GATT_RSP_BY_APP))){
            LOG_ERROR("Error in %s, line=%d, control->auto_rsp should be set to ESP_GATT_AUTO_RSP or ESP_GATT_RSP_BY_APP\n",\
                            __func__, __LINE__);
            return ESP_ERR_INVALID_ARG;
    }

    if ((control != NULL) && (control->auto_rsp == ESP_GATT_AUTO_RSP)){
        if (char_val == NULL){
            LOG_ERROR("Error in %s, line=%d, for stack respond attribute, char_val should not be NULL here\n",\
                            __func__, __LINE__);
            return ESP_ERR_INVALID_ARG;
        } else if (char_val->attr_max_len == 0){
            LOG_ERROR("Error in %s, line=%d, for stack respond attribute,  attribute max length should not be 0\n",\
                            __func__, __LINE__);
            return ESP_ERR_INVALID_ARG;
        }
    }

    return ESP_OK;
}

#endif  ///GATTS_INCLUDED
