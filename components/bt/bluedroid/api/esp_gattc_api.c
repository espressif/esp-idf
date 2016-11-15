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

/*******************************************************************************
**
** @function        esp_ble_gattc_app_register_callback
**
** @brief	      	This function is called to register application callbacks
**                  with GATTC module.
**
** @param[in]       callback - pointer to the application callback function.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_register_callback(esp_profile_cb_t callback)
{
    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_GATTC, callback);
    return ESP_OK;
}

/*******************************************************************************
**
** @function        esp_ble_gattc_app_register
**
** @brief	      	This function is called to register application
**                  with GATTC module.
**
** @param[in]       app_id : Application Identitfy (UUID), for different application
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_app_register(uint16_t app_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_REGISTER;
    arg.app_id = app_id;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_app_unregister
**
** @brief      		This function is called to unregister an application
**                  from GATTC module.
**
** @param[in]       client_if - client interface identifier.
**
** @return          None
**
*******************************************************************************/
esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gatt_if)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_UNREGISTER;
    arg.gatt_if = gatt_if;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_conn
**
** @brief      		Open a direct connection or add a background auto connection
**                  bd address
**
** @param[in]       gatt_if: application identity.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**                  
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_open(esp_gatt_if_t gatt_if, esp_bd_addr_t remote_bda, bool is_direct)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_OPEN;
    arg.gatt_if = gatt_if;
    memcpy(arg.remote_bda, remote_bda, ESP_BD_ADDR_LEN);
    arg.is_direct = is_direct;
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_close
**
** @brief      		Close a connection to a GATT server.
**
** @param[in]       conn_id: connectino ID to be closed.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_close (uint16_t conn_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CLOSE;
    arg.conn_id = conn_id;
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


/*******************************************************************************
**
** @function        esp_ble_gattc_config_mtu
**
** @brief      		Configure the MTU size in the GATT channel. This can be done
**                  only once per connection.
**
** @param[in]       conn_id: connection ID.
**                  mtu: desired MTU size to use.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
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
    arg.conn_id = conn_id;
    arg.mtu = mtu;
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_search_service
**
** @brief      		This function is called to request a GATT service discovery
**                  on a GATT server. This function report service search result
**                  by a callback event, and followed by a service search complete
**                  event.
**
** @param[in]       conn_id: connection ID.
** @param[in]       filter_uuid: a UUID of the service application is interested in.
**                  If Null, discover for all services.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_search_service(uint16_t conn_id, esp_bt_uuid_t *filter_uuid)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_SEARCH_SERVICE;
    arg.conn_id = conn_id;
    memcpy(&arg.uuid, filter_uuid, sizeof(esp_bt_uuid_t));
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


/****************************************************************************************************
**
** @function        esp_ble_gattc_get_characteristic
**
** @brief      		This function is called to find the first characteristic of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
**          
** @param[in]       srvc_id: serivce ID
**                 
** @param[in]       start_char_id:  the start characteristic ID 
**
** @return          ESP_OK - success, other - failed
**
*****************************************************************************************************/

esp_err_t esp_ble_gattc_get_characteristic(uint16_t conn_id,
                                           esp_gatt_srvc_id_t *srvc_id,
                                           esp_gatt_id_t *start_char_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    arg.conn_id = conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    if (start_char_id) {
        memcpy(&arg.char_id, start_char_id, sizeof(esp_gatt_id_t));
        msg.act = BTC_GATTC_ACT_GET_NEXT_CHAR;
    } else {
        msg.act = BTC_GATTC_ACT_GET_FIRST_CHAR;
    }
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


/****************************************************************************************************
**
** @function        esp_ble_gattc_get_descriptor
**
** @brief      		This function is called to find the descriptor of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       srvc_id: the service ID of which the characteristic is belonged to.
** @param[in]       char_id: Characteristic ID, if NULL find the first available
**                  characteristic.
** @param[in]       start_descr_id:  the sctart descriptor id
**
** @return          ESP_OK - success, other - failed
**
*****************************************************************************************************/
esp_err_t esp_ble_gattc_get_descriptor(uint16_t conn_id, 
                                       esp_gatt_srvc_id_t *srvc_id,
                                       esp_gatt_id_t *char_id,
                                       esp_gatt_id_t *start_descr_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    arg.conn_id = conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    if (start_descr_id) {
        memcpy(&arg.descr_id, start_descr_id, sizeof(esp_gatt_id_t));
        msg.act = BTC_GATTC_ACT_GET_NEXT_DESCR;
    } else {
        msg.act = BTC_GATTC_ACT_GET_FIRST_DESCR;
    }
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


/****************************************************************************************************
**
** @function        esp_ble_gattc_get_include_service
**
** @brief      	    This function is called to find the first characteristic of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       srvc_id: the service ID of which the characteristic is belonged to.
** @param[in]       start_incl_srvc_id: the start include service id
**
** @return          ESP_OK - success, other - failed
**
*****************************************************************************************************/


esp_err_t esp_ble_gattc_get_included_service(uint16_t conn_id,
                                             esp_gatt_srvc_id_t *srvc_id,
                                             esp_gatt_srvc_id_t *start_incl_srvc_id)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    arg.conn_id = conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    if (start_incl_srvc_id) {
        memcpy(&arg.start_service_id, start_incl_srvc_id, sizeof(esp_gatt_srvc_id_t));
        msg.act = BTC_GATTC_ACT_GET_NEXT_INCL_SERVICE;
    } else {
        msg.act = BTC_GATTC_ACT_GET_FIRST_INCL_SERVICE;
    }
    
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_read_char
**
** @brief      	    This function is called to read a service's characteristics of
**                  the given characteritisc ID.UTH_REQ_NO_SCATTERNET
**
** @param[in]       conn_id - connectino ID.
** @param[in]       srvc_id - serivcie ID.
** @param[in]       char_id - characteritic ID to read.
** @param[in]       auth_req - authenticate request type
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_read_char (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                   esp_gatt_id_t *char_id, esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR;
    arg.conn_id = conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    arg.auth_req = auth_req;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_read_char_descr
**
** @brief	        This function is called to read a characteristics descriptor.
**
** @param[in]       conn_id - connection ID.
** @param[in]       srvc_id - serivcie ID.
** @param[in]       descr_id - characteritic descriptor ID to read.
** @param[in]       auth_req - authenticate request type
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
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
    arg.conn_id = conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    memcpy(&arg.descr_id, descr_id, sizeof(esp_gatt_id_t));
    arg.auth_req = auth_req;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_write_char
**
** @brief      		This function is called to write characteristic value.
**
** @param[in]       conn_id - connection ID.
** @param[in]       srvc_id - serivcie ID.
** @param[in]       char_id - characteristic ID to write.
** @param[in]       value_len: length of the value to be written.
** @param[in]       value - the value to be written.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_write_char( uint16_t conn_id,
                                    esp_gatt_srvc_id_t *srvc_id,
                                    esp_gatt_id_t *char_id,
                                    uint16_t value_len,
                                    uint8_t *value,
                                    esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR;
    arg.conn_id = (uint16_t) conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    arg.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    memcpy(arg.value, value, arg.value_len);
    arg.auth_req = auth_req;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_write_char_descr
**
** @brief	      	This function is called to write characteristic descriptor value.
**
** @param[in]       conn_id - connection ID
** @param[in]       srvc_id - serivcie ID.
** @param[in]       char_id - characteristic ID.
** @param[in]       descr_id - characteristic descriptor ID to write.
** @param[in]       value_len: length of the value to be written.
** @param[in]       value - the value to be written.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_write_char_descr (uint16_t conn_id,
                                          esp_gatt_srvc_id_t *srvc_id,
                                          esp_gatt_id_t *char_id,
                                          esp_gatt_id_t *descr_id,
                                          uint16_t value_len,
                                          uint8_t *value,
                                          esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR_DESCR;
    arg.conn_id = (uint16_t) conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    memcpy(&arg.descr_id, descr_id, sizeof(esp_gatt_id_t));
    arg.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    memcpy(arg.value, value, arg.value_len);
    arg.auth_req = auth_req;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

/*******************************************************************************
**
** @function        esp_ble_gattc_prepare_write
**
** @brief      		This function is called to prepare write a characteristic value.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - GATT characteritic ID of the service.
** @param[in]       offset - offset of the write value.
** @param[in]       value_len: length of the value to be written.
** @param[in]       value - the value to be written.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
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
    arg.conn_id = conn_id;
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    arg.offset = offset;
    arg.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len; // length check ?
    memcpy(arg.value, value, arg.value_len);
    arg.auth_req = auth_req;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}



/*******************************************************************************
**
** @function        esp_ble_gattc_execu_write
**
** @brief           This function is called to execute write a prepare write sequence.
**
** @param[in]       conn_id - connection ID.
** @param[in]       is_execute - execute or cancel.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gattc_execute_write (uint16_t conn_id, bool is_execute)
{
    btc_msg_t msg;
    btc_ble_gattc_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_EXECUTE_WRITE;
    arg.conn_id = conn_id;
    arg.is_execute = is_execute;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}



/*******************************************************************************
**
** @function        esp_ble_gattc_register_for_notify
**
** @brief           This function is called to register for notification of a service.
**
** @param[in]       gatt_if - gatt interface id.
** @param[in]       bda - target GATT server.
** @param[in]       srvc_id - pointer to GATT service ID.
** @param[in]       char_id - pointer to GATT characteristic ID.
**
** @return          OK if registration succeed, otherwise failed.
**
*******************************************************************************/
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
    arg.gatt_if = gatt_if;
    memcpy(&arg.remote_bda, &server_bda, sizeof(esp_bd_addr_t));
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}



/*******************************************************************************
**
** @function        esp_ble_gattc_unregister_ntf
**
** @brief           This function is called to de-register for notification of a service.
**
** @param[in]       gatt_if - gatt interface id.
** @param[in]       bda - target GATT server.
** @param[in]       srvc_id - pointer to GATT service ID.
** @param[in]       char_id - pointer to GATT characteristic ID.
**
** @return          OK if deregistration succeed, otherwise failed.
**
*******************************************************************************/
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
    arg.gatt_if = gatt_if;
    memcpy(&arg.remote_bda, &server_bda, sizeof(esp_bd_addr_t));
    memcpy(&arg.service_id, srvc_id, sizeof(esp_gatt_srvc_id_t));
    memcpy(&arg.char_id, char_id, sizeof(esp_gatt_id_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

