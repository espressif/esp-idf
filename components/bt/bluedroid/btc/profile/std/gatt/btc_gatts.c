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

#include "bta_gatt_api.h"

#include "btc_task.h"
#include "btc_manage.h"
#include "btc_gatts.h"
#include "btc_gatt_util.h"
#include "future.h"
#include "btc_main.h"
#include "esp_gatts_api.h"

#if (GATTS_INCLUDED == TRUE)

#define A2C_GATTS_EVT(_bta_event) (_bta_event) //BTA TO BTC EVT
#define C2A_GATTS_EVT(_btc_event) (_btc_event) //BTC TO BTA EVT

typedef struct {
    future_t *complete_future;
    uint16_t svc_start_hdl;
    esp_bt_uuid_t svc_uuid;
    bool        is_tab_creat_svc;
    bool      is_use_svc;
    uint8_t   num_handle;
    uint8_t   handle_idx;
    uint16_t handles[ESP_GATT_ATTR_HANDLE_MAX];
} esp_btc_creat_tab_t;

static esp_btc_creat_tab_t btc_creat_tab_env;


static esp_gatt_status_t btc_gatts_check_valid_attr_tab(esp_gatts_attr_db_t *gatts_attr_db,
                                                                          uint8_t max_nb_attr);

static inline void btc_gatts_cb_to_app(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_gatts_cb_t btc_gatts_cb = (esp_gatts_cb_t)btc_profile_cb_get(BTC_PID_GATTS);
    if (btc_gatts_cb) {
        btc_gatts_cb(event, gatts_if, param);
    }
}

static inline void btc_gatts_uuid_format_convert(esp_bt_uuid_t* dest_uuid, uint16_t src_uuid_len, uint8_t* src_uuid_p)
{
    dest_uuid->len = src_uuid_len;
    if(src_uuid_len == ESP_UUID_LEN_16){
        dest_uuid->uuid.uuid16 = src_uuid_p[0] + (src_uuid_p[1]<<8);
    }
    else if(src_uuid_len == ESP_UUID_LEN_32){
        dest_uuid->uuid.uuid32 = src_uuid_p[0] + (src_uuid_p[1]<<8) + (src_uuid_p[2]<<16) + (src_uuid_p[3]<<24);
    }
    else if(src_uuid_len == ESP_UUID_LEN_128){
        memcpy(dest_uuid->uuid.uuid128, src_uuid_p, src_uuid_len);
    }
    else{
        LOG_ERROR("%s wrong uuid length %d\n", __func__, src_uuid_len);
    }

}


void btc_gatts_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_gatts_args_t *dst = (btc_ble_gatts_args_t *) p_dest;
    btc_ble_gatts_args_t *src = (btc_ble_gatts_args_t *)p_src;

    switch (msg->act) {
    case BTC_GATTS_ACT_SEND_INDICATE: {
        dst->send_ind.value = (uint8_t *)GKI_getbuf(src->send_ind.value_len);
        if (dst->send_ind.value) {
            memcpy(dst->send_ind.value, src->send_ind.value, src->send_ind.value_len);
        } else {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        break;
    }
    case BTC_GATTS_ACT_SEND_RESPONSE: {
        if (src->send_rsp.rsp) {
            dst->send_rsp.rsp = (esp_gatt_rsp_t *)GKI_getbuf(sizeof(esp_gatt_rsp_t));
            if (dst->send_rsp.rsp) {
                memcpy(dst->send_rsp.rsp, src->send_rsp.rsp, sizeof(esp_gatt_rsp_t));
            } else {
                LOG_ERROR("%s %d no mem\n", __func__, msg->act);
            }
        }
        break;
    
    }
    case BTC_GATTS_ACT_ADD_CHAR:{
        if (src->add_char.char_val.attr_value != NULL){
            dst->add_char.char_val.attr_value = (uint8_t *)GKI_getbuf(src->add_char.char_val.attr_len);
            if(dst->add_char.char_val.attr_value != NULL){
                memcpy(dst->add_char.char_val.attr_value, src->add_char.char_val.attr_value, 
                        src->add_char.char_val.attr_len);
            }else{
                LOG_ERROR("%s %d no mem\n", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_GATTS_ACT_ADD_CHAR_DESCR:{
        if(src->add_descr.descr_val.attr_value != NULL){
            dst->add_descr.descr_val.attr_value = (uint8_t *)GKI_getbuf(src->add_descr.descr_val.attr_len);
            if(dst->add_descr.descr_val.attr_value != NULL){
                memcpy(dst->add_descr.descr_val.attr_value, src->add_descr.descr_val.attr_value,
                        src->add_descr.descr_val.attr_len);
            }else{
                LOG_ERROR("%s %d no mem\n", __func__, msg->act);
            }
        }
        break;
    }
    case BTC_GATTS_ACT_CREATE_ATTR_TAB:{
        uint8_t num_attr = src->create_attr_tab.max_nb_attr;
        if(src->create_attr_tab.gatts_attr_db != NULL){
            dst->create_attr_tab.gatts_attr_db = (esp_gatts_attr_db_t *)GKI_getbuf(sizeof(esp_gatts_attr_db_t)*num_attr);
            if(dst->create_attr_tab.gatts_attr_db != NULL){
                memcpy(dst->create_attr_tab.gatts_attr_db, src->create_attr_tab.gatts_attr_db,
                        sizeof(esp_gatts_attr_db_t)*num_attr);
            }else{
                LOG_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    }
   case BTC_GATTS_ACT_SET_ATTR_VALUE:{
        uint8_t len = src->set_attr_val.length;
        if(src->set_attr_val.value){
            dst->set_attr_val.value = (uint8_t *)GKI_getbuf(len);
            if(dst->set_attr_val.value != NULL){
                memcpy(dst->set_attr_val.value, src->set_attr_val.value, len);
            }else{
                LOG_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    }
    default:
        LOG_DEBUG("%s Unhandled deep copy %d\n", __func__, msg->act);
        break;
    }

}

void btc_gatts_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_gatts_args_t *arg = (btc_ble_gatts_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_GATTS_ACT_SEND_INDICATE: {
        if (arg->send_ind.value) {
            GKI_freebuf(arg->send_ind.value);
        }
        break;
    }
    case BTC_GATTS_ACT_SEND_RESPONSE: {
        if (arg->send_rsp.rsp) {
            GKI_freebuf(arg->send_rsp.rsp);
        }
        break;
    }
    case BTC_GATTS_ACT_ADD_CHAR:{
        if (arg->add_char.char_val.attr_value != NULL) {
            GKI_freebuf(arg->add_char.char_val.attr_value);
        }
        break;
    }
    case BTC_GATTS_ACT_ADD_CHAR_DESCR:{
        if (arg->add_descr.descr_val.attr_value != NULL){
            GKI_freebuf(arg->add_descr.descr_val.attr_value);
        }
        break;
    }
    case BTC_GATTS_ACT_CREATE_ATTR_TAB:{
        if (arg->create_attr_tab.gatts_attr_db != NULL){
            GKI_freebuf(arg->create_attr_tab.gatts_attr_db);
        }
        break;
    }
    case BTC_GATTS_ACT_SET_ATTR_VALUE:{
        if (arg->set_attr_val.value != NULL){
            GKI_freebuf(arg->set_attr_val.value);
        }
    }
        break;

    default:
        LOG_DEBUG("%s Unhandled deep free %d\n", __func__, msg->act);
        break;
    }

}

static void btc_gatts_act_create_attr_tab(esp_gatts_attr_db_t *gatts_attr_db, 
                                                        esp_gatt_if_t gatts_if,
                                                        uint8_t max_nb_attr,
                                                        uint8_t srvc_inst_id)
{
    uint16_t uuid = 0;
    future_t *future_p;
    esp_ble_gatts_cb_param_t param;
    param.add_attr_tab.status = ESP_GATT_OK;
    param.add_attr_tab.num_handle = max_nb_attr;

    if (param.add_attr_tab.status != ESP_GATT_OK) {
        btc_gatts_cb_to_app(ESP_GATTS_CREAT_ATTR_TAB_EVT, gatts_if, &param);
        //reset the env after sent the data to app
        memset(&btc_creat_tab_env, 0, sizeof(esp_btc_creat_tab_t));
        return;
    }

    // Check the attribute table is valid or not
    if ((param.add_attr_tab.status = btc_gatts_check_valid_attr_tab(gatts_attr_db, max_nb_attr)) != ESP_GATT_OK) {
        //sent the callback event to the application
        btc_gatts_cb_to_app(ESP_GATTS_CREAT_ATTR_TAB_EVT, gatts_if, &param);
        return;
    }


    //set the attribute table create service flag to true
    btc_creat_tab_env.is_tab_creat_svc = true;
    btc_creat_tab_env.num_handle = max_nb_attr;
    for(int i = 0; i < max_nb_attr; i++){
        if(gatts_attr_db[i].att_desc.uuid_length == ESP_UUID_LEN_16){
            uuid = (gatts_attr_db[i].att_desc.uuid_p[1] << 8) + (gatts_attr_db[i].att_desc.uuid_p[0]);
        }
        else{
            continue;
        }
        future_p = future_new();
        if (future_p == NULL) {
                LOG_ERROR("%s failed:no mem\n", __func__);
                return ;
            }
        btc_creat_tab_env.complete_future = future_p;
        btc_creat_tab_env.handle_idx = i;
        switch(uuid)
        {
            case ESP_GATT_UUID_PRI_SERVICE:{
                tBTA_GATT_SRVC_ID srvc_id;
                esp_gatt_srvc_id_t        esp_srvc_id;

                esp_srvc_id.id.inst_id = srvc_inst_id;
                btc_gatts_uuid_format_convert(&esp_srvc_id.id.uuid,gatts_attr_db[i].att_desc.length,
                                              gatts_attr_db[i].att_desc.value);
    
                btc_to_bta_srvc_id(&srvc_id, &esp_srvc_id);
                if (btc_creat_tab_env.is_use_svc != true) {
                    BTA_GATTS_CreateService(gatts_if, &srvc_id.id.uuid,
                                            srvc_inst_id, max_nb_attr, true);
                    btc_creat_tab_env.is_use_svc = true;
                } else {
                    LOG_ERROR("Each service table can only created one primary service.");
                    param.add_attr_tab.status = ESP_GATT_ERROR;
                    btc_gatts_cb_to_app(ESP_GATTS_CREAT_ATTR_TAB_EVT, gatts_if, &param);
                    //reset the env after sent the data to app
                    memset(&btc_creat_tab_env, 0, sizeof(esp_btc_creat_tab_t));
                    return;
                }
                
                 if (future_await(future_p) == FUTURE_FAIL) {
                        LOG_ERROR("%s failed\n", __func__);
                        return;
                        }
                    break;
            }
            case ESP_GATT_UUID_SEC_SERVICE:{
                tBTA_GATT_SRVC_ID srvc_id;
                esp_gatt_srvc_id_t        esp_srvc_id;

                esp_srvc_id.id.inst_id = srvc_inst_id;
                btc_gatts_uuid_format_convert(&esp_srvc_id.id.uuid,gatts_attr_db[i].att_desc.uuid_length,
                                              gatts_attr_db[i].att_desc.uuid_p);
                btc_to_bta_srvc_id(&srvc_id, &esp_srvc_id);
                if (btc_creat_tab_env.is_use_svc != true) {
                    BTA_GATTS_CreateService(gatts_if, &srvc_id.id.uuid,
                                            srvc_inst_id, max_nb_attr, false);
                    btc_creat_tab_env.is_use_svc = true;
                } else {
                    LOG_ERROR("Each service table can only created one secondary service.");
                    param.add_attr_tab.status = ESP_GATT_ERROR;
                    btc_gatts_cb_to_app(ESP_GATTS_CREAT_ATTR_TAB_EVT, gatts_if, &param);
                    //reset the env after sent the data to app
                    memset(&btc_creat_tab_env, 0, sizeof(esp_btc_creat_tab_t));
                    return;
                }
                if (future_await(future_p) == FUTURE_FAIL) {
                        LOG_ERROR("%s failed\n", __func__);
                        return;
                        }
                break;
            }
            case ESP_GATT_UUID_INCLUDE_SERVICE:{
                esp_gatts_incl_svc_desc_t *incl_svc_desc = (esp_gatts_incl_svc_desc_t *)gatts_attr_db[i].att_desc.value;
                
                if(incl_svc_desc!= NULL){
                    if(btc_creat_tab_env.svc_start_hdl != 0){
                        BTA_GATTS_AddIncludeService(btc_creat_tab_env.svc_start_hdl, 
                            incl_svc_desc->start_hdl);
                        
                        if (future_await(future_p) == FUTURE_FAIL) {
                                LOG_ERROR("%s failed\n", __func__);
                                return;
                            }
                    }
                }
                break;
            }
            case ESP_GATT_UUID_CHAR_DECLARE:{
                uint16_t svc_hal = 0;
                tBT_UUID bta_char_uuid;
                tGATT_ATTR_VAL attr_val;
                esp_bt_uuid_t uuid_temp;
                tBTA_GATT_PERM perm;
                tBTA_GATTS_ATTR_CONTROL control;
                uint8_t char_property;

                if(btc_creat_tab_env.svc_start_hdl != 0){
                    svc_hal = btc_creat_tab_env.svc_start_hdl;
                    if((gatts_attr_db[i].att_desc.value) == NULL){
                        LOG_ERROR("%s Characteristic declaration should not be NULL\n", __func__);
                    }
                    else{
                        char_property = (uint8_t)(*(uint8_t*)(gatts_attr_db[i].att_desc.value));
                        perm = gatts_attr_db[i+1].att_desc.perm;
                        attr_val.attr_len = gatts_attr_db[i+1].att_desc.length;
                        attr_val.attr_max_len = gatts_attr_db[i+1].att_desc.max_length;
                        btc_gatts_uuid_format_convert(&uuid_temp, gatts_attr_db[i+1].att_desc.uuid_length,gatts_attr_db[i+1].att_desc.uuid_p);
                        btc_to_bta_uuid(&bta_char_uuid, &uuid_temp);
                        attr_val.attr_val = gatts_attr_db[i+1].att_desc.value;
                        control.auto_rsp = gatts_attr_db[i+1].attr_control.auto_rsp;
                        BTA_GATTS_AddCharacteristic (svc_hal, &bta_char_uuid,
                                         perm, char_property, &attr_val, &control);

                        if (future_await(future_p) == FUTURE_FAIL) {
                                LOG_ERROR("%s failed\n", __func__);
                                return;
                                }
                    }
                }
            
                break;
            }
            case ESP_GATT_UUID_CHAR_EXT_PROP:               
            case ESP_GATT_UUID_CHAR_DESCRIPTION:
            case ESP_GATT_UUID_CHAR_CLIENT_CONFIG:
            case ESP_GATT_UUID_CHAR_SRVR_CONFIG:
            case ESP_GATT_UUID_CHAR_PRESENT_FORMAT:
            case ESP_GATT_UUID_CHAR_AGG_FORMAT:
            case ESP_GATT_UUID_CHAR_VALID_RANGE:
            case ESP_GATT_UUID_EXT_RPT_REF_DESCR:
            case ESP_GATT_UUID_RPT_REF_DESCR:{
                uint16_t svc_hal = btc_creat_tab_env.svc_start_hdl;
                tBT_UUID bta_char_uuid;
                esp_bt_uuid_t uuid_temp;
                tGATT_ATTR_VAL attr_val;
                tBTA_GATT_PERM perm = gatts_attr_db[i].att_desc.perm;
                tBTA_GATTS_ATTR_CONTROL control;

                if(svc_hal != 0){
                    attr_val.attr_len = gatts_attr_db[i].att_desc.length;
                    attr_val.attr_max_len = gatts_attr_db[i].att_desc.max_length;
                    attr_val.attr_val = gatts_attr_db[i].att_desc.value;
                    btc_gatts_uuid_format_convert(&uuid_temp, gatts_attr_db[i].att_desc.uuid_length,
                                                  gatts_attr_db[i].att_desc.uuid_p);
                    btc_to_bta_uuid(&bta_char_uuid, &uuid_temp);
                    control.auto_rsp = gatts_attr_db[i].attr_control.auto_rsp;
                    BTA_GATTS_AddCharDescriptor(svc_hal, perm, &bta_char_uuid, &attr_val, &control);
        
                    if (future_await(future_p) == FUTURE_FAIL) {
                        LOG_ERROR("%s failed\n", __func__);
                        return;
                        }
                }
                break;
            }
            default:
                future_free(future_p);
                break;
        }

        
    }

    param.add_attr_tab.handles = btc_creat_tab_env.handles;
    memcpy(&param.add_attr_tab.svc_uuid, &btc_creat_tab_env.svc_uuid, sizeof(esp_bt_uuid_t));

    btc_gatts_cb_to_app(ESP_GATTS_CREAT_ATTR_TAB_EVT, gatts_if, &param);        
    //reset the env after sent the data to app
    memset(&btc_creat_tab_env, 0, sizeof(esp_btc_creat_tab_t));

    //release the flag vaule to false after finish the service created.
    btc_creat_tab_env.is_tab_creat_svc = false;
}

static esp_gatt_status_t btc_gatts_check_valid_attr_tab(esp_gatts_attr_db_t *gatts_attr_db,
                                                                          uint8_t max_nb_attr)
{
    uint8_t svc_num = 0;
    uint16_t uuid = 0;

    for(int i = 0; i < max_nb_attr; i++) {
        if(gatts_attr_db[i].att_desc.uuid_length != ESP_UUID_LEN_16) {
            continue;
        }

        uuid = (gatts_attr_db[i].att_desc.uuid_p[1] << 8) + (gatts_attr_db[i].att_desc.uuid_p[0]);
        switch(uuid) {
            case ESP_GATT_UUID_PRI_SERVICE:
            case ESP_GATT_UUID_SEC_SERVICE:
                if (++svc_num > 1) {
                    LOG_ERROR("Each service table can only created one primary service or secondly service.");
                    return ESP_GATT_ERROR;
                }
                break;
            case ESP_GATT_UUID_INCLUDE_SERVICE: {
                esp_gatts_incl_svc_desc_t *svc_desc = (esp_gatts_incl_svc_desc_t *)gatts_attr_db[i].att_desc.value;
                if(svc_desc == NULL) {
                    LOG_ERROR("%s, The include service attribute should not be NULL.", __func__);
                    return ESP_GATT_INVALID_PDU;
                } else if((svc_desc->start_hdl == 0) || (svc_desc->end_hdl == 0) ||
                          (svc_desc->start_hdl == svc_desc->end_hdl)) {
                    LOG_ERROR("%s, The include service attribute handle is invalid, start_hanlde = %d, end_handle = %d",\
                              __func__, svc_desc->start_hdl, svc_desc->end_hdl);
                    return ESP_GATT_INVALID_HANDLE;
                }
                break;
            }
            case ESP_GATT_UUID_CHAR_DECLARE:
                if((gatts_attr_db[i].att_desc.value) == NULL) {
                    LOG_ERROR("%s, Characteristic declaration should not be NULL.", __func__);
                    return ESP_GATT_INVALID_PDU;
                }

                if(gatts_attr_db[i+1].att_desc.uuid_length != ESP_UUID_LEN_16 &&
                   gatts_attr_db[i+1].att_desc.uuid_length != ESP_UUID_LEN_32 &&
                   gatts_attr_db[i+1].att_desc.uuid_length != ESP_UUID_LEN_128) {
                    LOG_ERROR("%s, The Charateristic uuid length = %d is invalid", __func__,\
                        gatts_attr_db[i+1].att_desc.uuid_length);
                    return ESP_GATT_INVALID_ATTR_LEN;
                }

                if(gatts_attr_db[i+1].att_desc.uuid_length == ESP_UUID_LEN_16) {
                    uuid = (gatts_attr_db[i+1].att_desc.uuid_p[1] << 8) + (gatts_attr_db[i+1].att_desc.uuid_p[0]);
                    if(uuid == ESP_GATT_UUID_CHAR_DECLARE || uuid == ESP_GATT_UUID_CHAR_EXT_PROP ||
                        uuid == ESP_GATT_UUID_CHAR_DESCRIPTION || uuid == ESP_GATT_UUID_CHAR_CLIENT_CONFIG ||
                        uuid == ESP_GATT_UUID_CHAR_SRVR_CONFIG || uuid == ESP_GATT_UUID_CHAR_PRESENT_FORMAT ||
                        uuid == ESP_GATT_UUID_CHAR_AGG_FORMAT || uuid == ESP_GATT_UUID_CHAR_VALID_RANGE ||
                        uuid == ESP_GATT_UUID_EXT_RPT_REF_DESCR || uuid == ESP_GATT_UUID_RPT_REF_DESCR) {
                        LOG_ERROR("%s, The charateristic value uuid = %d is invalid", __func__, uuid);
                        return ESP_GATT_INVALID_PDU;
                    }
                }
                break;
            default:
                break;
        }
    }

    return ESP_GATT_OK;
}

void btc_gatts_get_attr_value(uint16_t attr_handle, uint16_t *length, uint8_t **value)
{
    
    BTA_GetAttributeValue(attr_handle, length, value);
}


static void btc_gatts_cb_param_copy_req(btc_msg_t *msg, void *p_dest, void *p_src)
{
    uint16_t event = msg->act;

    tBTA_GATTS *p_dest_data = (tBTA_GATTS *) p_dest;
    tBTA_GATTS *p_src_data = (tBTA_GATTS *) p_src;

    if (!p_src_data || !p_dest_data) {
        return;
    }

    // Copy basic structure first
    memcpy(p_dest_data, p_src_data, sizeof(tBTA_GATTS));

    // Allocate buffer for request data if necessary
    switch (event) {
    case BTA_GATTS_READ_EVT:
    case BTA_GATTS_WRITE_EVT:
    case BTA_GATTS_EXEC_WRITE_EVT:
    case BTA_GATTS_MTU_EVT:
        p_dest_data->req_data.p_data = GKI_getbuf(sizeof(tBTA_GATTS_REQ_DATA));
        if (p_dest_data->req_data.p_data != NULL) {
            memcpy(p_dest_data->req_data.p_data, p_src_data->req_data.p_data,
                   sizeof(tBTA_GATTS_REQ_DATA));
        }
        break;

    default:
        break;
    }
}

static void btc_gatts_cb_param_copy_free(btc_msg_t *msg, tBTA_GATTS *p_data)
{
    uint16_t event = msg->act;

    switch (event) {
    case BTA_GATTS_READ_EVT:
    case BTA_GATTS_WRITE_EVT:
    case BTA_GATTS_EXEC_WRITE_EVT:
    case BTA_GATTS_MTU_EVT:
        if (p_data && p_data->req_data.p_data) {
            GKI_freebuf(p_data->req_data.p_data);
        }
        break;
    default:
        break;
    }
}


static void btc_gatts_inter_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    bt_status_t status;
    btc_msg_t msg;
    
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GATTS;
    msg.act = event;
    if(btc_creat_tab_env.is_tab_creat_svc && btc_creat_tab_env.complete_future) {
        switch(event) {
            case BTA_GATTS_CREATE_EVT: {
                //save the service handle to the btc module after used 
                //the attribute table method to creat a service
                bta_to_btc_uuid(&btc_creat_tab_env.svc_uuid, &p_data->create.uuid);
                uint8_t index = btc_creat_tab_env.handle_idx;
                btc_creat_tab_env.svc_start_hdl = p_data->create.service_id;
                btc_creat_tab_env.handles[index] = p_data->create.service_id;
                break;
            }
            case BTA_GATTS_ADD_INCL_SRVC_EVT: {
                uint8_t index = btc_creat_tab_env.handle_idx;
                btc_creat_tab_env.handles[index] = p_data->add_result.attr_id;
                break;
            }
            case BTA_GATTS_ADD_CHAR_EVT: {
                uint8_t index = btc_creat_tab_env.handle_idx;
                btc_creat_tab_env.handles[index] = p_data->add_result.attr_id - 1;
                btc_creat_tab_env.handles[index+1] = p_data->add_result.attr_id;
                break;
            }
            case BTA_GATTS_ADD_CHAR_DESCR_EVT: {
                uint8_t index = btc_creat_tab_env.handle_idx;
                btc_creat_tab_env.handles[index] = p_data->add_result.attr_id;
                break;
            }
            default:
                break;
        }

        future_ready(btc_creat_tab_env.complete_future, FUTURE_SUCCESS);
        return;
    }
    status = btc_transfer_context(&msg, p_data,
                                  sizeof(tBTA_GATTS), btc_gatts_cb_param_copy_req);

    if (status != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_gatts_call_handler(btc_msg_t *msg)
{
    btc_ble_gatts_args_t *arg = (btc_ble_gatts_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_GATTS_ACT_APP_REGISTER: {
        tBT_UUID uuid;

        uuid.len = LEN_UUID_16;
        uuid.uu.uuid16 = arg->app_reg.app_id;

        BTA_GATTS_AppRegister(&uuid, btc_gatts_inter_cb);

        break;
    }
    case BTC_GATTS_ACT_APP_UNREGISTER:
        BTA_GATTS_AppDeregister(arg->app_unreg.gatts_if);
        break;
    case BTC_GATTS_ACT_CREATE_SERVICE: {
        tBTA_GATT_SRVC_ID srvc_id;
        btc_to_bta_srvc_id(&srvc_id, &arg->create_srvc.service_id);
        BTA_GATTS_CreateService(arg->create_srvc.gatts_if, &srvc_id.id.uuid,
                                srvc_id.id.inst_id, arg->create_srvc.num_handle,
                                srvc_id.is_primary);
        break;
    }
   case BTC_GATTS_ACT_CREATE_ATTR_TAB:
    btc_gatts_act_create_attr_tab(arg->create_attr_tab.gatts_attr_db,
                                         arg->create_attr_tab.gatts_if,
                                         arg->create_attr_tab.max_nb_attr,
                                         arg->create_attr_tab.srvc_inst_id);
    break;
    case BTC_GATTS_ACT_DELETE_SERVICE:
        BTA_GATTS_DeleteService(arg->delete_srvc.service_handle);
        break;
    case BTC_GATTS_ACT_START_SERVICE:
        BTA_GATTS_StartService(arg->start_srvc.service_handle, BTA_GATT_TRANSPORT_LE);
        break;
    case BTC_GATTS_ACT_STOP_SERVICE:
        BTA_GATTS_StopService(arg->stop_srvc.service_handle);
        break;
    case BTC_GATTS_ACT_ADD_INCLUDE_SERVICE:
        BTA_GATTS_AddIncludeService(arg->add_incl_srvc.service_handle, arg->add_incl_srvc.included_service_handle);
        break;
    case BTC_GATTS_ACT_ADD_CHAR: {
        tBT_UUID uuid;
        btc_to_bta_uuid(&uuid, &arg->add_char.char_uuid);

        BTA_GATTS_AddCharacteristic(arg->add_char.service_handle, &uuid,
                                    arg->add_char.perm, arg->add_char.property, 
                                    (tGATT_ATTR_VAL *)&arg->add_char.char_val,
                                    (tBTA_GATTS_ATTR_CONTROL *)&arg->add_char.attr_control);
        break;
    }
    case BTC_GATTS_ACT_ADD_CHAR_DESCR: {
        tBT_UUID uuid;
        btc_to_bta_uuid(&uuid, &arg->add_descr.descr_uuid);
        BTA_GATTS_AddCharDescriptor(arg->add_descr.service_handle, arg->add_descr.perm, &uuid,
                                   (tBTA_GATT_ATTR_VAL *)&arg->add_descr.descr_val, 
                                   (tBTA_GATTS_ATTR_CONTROL *)&arg->add_descr.attr_control);
        break;
    }
    case BTC_GATTS_ACT_SEND_INDICATE:
        BTA_GATTS_HandleValueIndication(arg->send_ind.conn_id, arg->send_ind.attr_handle,
                                        arg->send_ind.value_len, arg->send_ind.value, arg->send_ind.need_confirm);
        break;
    case BTC_GATTS_ACT_SEND_RESPONSE: {
        esp_ble_gatts_cb_param_t param;
        esp_gatt_rsp_t *p_rsp = arg->send_rsp.rsp;

        if (p_rsp) {
            tBTA_GATTS_RSP rsp_struct;
            btc_to_bta_response(&rsp_struct, p_rsp);
            BTA_GATTS_SendRsp(arg->send_rsp.conn_id, arg->send_rsp.trans_id,
                              arg->send_rsp.status, &rsp_struct);
            param.rsp.handle = rsp_struct.attr_value.handle;
        } else {
            BTA_GATTS_SendRsp(arg->send_rsp.conn_id, arg->send_rsp.trans_id,
                              arg->send_rsp.status, NULL);
        }

        param.rsp.status = 0;
        btc_gatts_cb_to_app(ESP_GATTS_RESPONSE_EVT, BTC_GATT_GET_GATT_IF(arg->send_rsp.conn_id), &param);
        break;
    }
    case BTC_GATTS_ACT_SET_ATTR_VALUE:
        BTA_SetAttributeValue(arg->set_attr_val.handle, arg->set_attr_val.length, 
                              arg->set_attr_val.value);
    break;
    case BTC_GATTS_ACT_OPEN: {
        // Ensure device is in inquiry database
        tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;

        //TODO : implement address type and device type
#if  0
        if (_get_address_type(arg->remote_bda, &addr_type) &&
                btif_get_device_type(arg->remote_bda, &device_type) &&
                device_type != BT_DEVICE_TYPE_BREDR) {
            BTA_DmAddBleDevice(p_cb->bd_addr.address, addr_type, device_type);
        }
#else
        //BTA_DmAddBleDevice(p_cb->bd_addr.address, addr_type, device_type);
#endif
        // Mark background connections
        if (!arg->open.is_direct) {
            BTA_DmBleSetBgConnType(BTM_BLE_CONN_AUTO, NULL);
        }

        transport = BTA_GATT_TRANSPORT_LE;

        // Connect!
        BTA_GATTS_Open(arg->open.gatts_if, arg->open.remote_bda,
                       arg->open.is_direct, transport);
        break;
    }
    case BTC_GATTS_ACT_CLOSE:
        // TODO : implement cancel open
        // Cancel pending foreground/background connections
        //BTA_GATTS_CancelOpen(p_cb->server_if, p_cb->bd_addr.address, TRUE);
        //BTA_GATTS_CancelOpen(p_cb->server_if, p_cb->bd_addr.address, FALSE);

        // Close active connection
        if (arg->close.conn_id != 0) {
            BTA_GATTS_Close(arg->close.conn_id);
        }

        break;
    default:
        break;
    }
    btc_gatts_arg_deep_free(msg);
}

void btc_gatts_cb_handler(btc_msg_t *msg)
{
    esp_ble_gatts_cb_param_t param;
    tBTA_GATTS *p_data = (tBTA_GATTS *)msg->arg;
    esp_gatt_if_t gatts_if;

    switch (msg->act) {
    case BTA_GATTS_REG_EVT: {
        gatts_if = p_data->reg_oper.server_if;
        param.reg.status = p_data->reg_oper.status;
        param.reg.app_id = p_data->reg_oper.uuid.uu.uuid16;

        btc_gatts_cb_to_app(ESP_GATTS_REG_EVT, gatts_if, &param);
        break;
    }
    case BTA_GATTS_DEREG_EVT: {
        gatts_if = p_data->reg_oper.server_if;
        btc_gatts_cb_to_app(ESP_GATTS_UNREG_EVT, gatts_if, NULL);
        break;
    }
    case BTA_GATTS_READ_EVT: {
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id);
        param.read.conn_id = BTC_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        param.read.trans_id = p_data->req_data.trans_id;
        memcpy(param.read.bda, p_data->req_data.remote_bda, ESP_BD_ADDR_LEN);
        param.read.handle = p_data->req_data.p_data->read_req.handle;
        param.read.offset = p_data->req_data.p_data->read_req.offset;
        param.read.is_long = p_data->req_data.p_data->read_req.is_long;

        param.read.need_rsp = p_data->req_data.p_data->read_req.need_rsp; 
        btc_gatts_cb_to_app(ESP_GATTS_READ_EVT, gatts_if, &param);
        break;
    }
    case BTA_GATTS_WRITE_EVT: {
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id);
        param.write.conn_id = BTC_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        param.write.trans_id = p_data->req_data.trans_id;
        memcpy(param.write.bda, p_data->req_data.remote_bda, ESP_BD_ADDR_LEN);
        param.write.handle = p_data->req_data.p_data->write_req.handle;
        param.write.offset = p_data->req_data.p_data->write_req.offset;
        param.write.need_rsp = p_data->req_data.p_data->write_req.need_rsp;
        param.write.is_prep = p_data->req_data.p_data->write_req.is_prep;
        param.write.len = p_data->req_data.p_data->write_req.len;
        param.write.value = p_data->req_data.p_data->write_req.value;

        btc_gatts_cb_to_app(ESP_GATTS_WRITE_EVT, gatts_if, &param);

        break;
    }
    case BTA_GATTS_EXEC_WRITE_EVT: {
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id);
        param.exec_write.conn_id = BTC_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        param.exec_write.trans_id = p_data->req_data.trans_id;
        memcpy(param.exec_write.bda, p_data->req_data.remote_bda, ESP_BD_ADDR_LEN);
        param.exec_write.exec_write_flag = p_data->req_data.p_data->exec_write;

        btc_gatts_cb_to_app(ESP_GATTS_EXEC_WRITE_EVT, gatts_if, &param);
        break;
    }
    case BTA_GATTS_MTU_EVT:
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id);
        param.mtu.conn_id = BTC_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        param.mtu.mtu = p_data->req_data.p_data->mtu;

        btc_gatts_cb_to_app(ESP_GATTS_MTU_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_CONF_EVT:
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->req_data.conn_id);
        param.conf.conn_id = BTC_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        param.conf.status = p_data->req_data.status;

        btc_gatts_cb_to_app(ESP_GATTS_CONF_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_CREATE_EVT:
        gatts_if = p_data->create.server_if;
        param.create.status = p_data->create.status;
        param.create.service_handle = p_data->create.service_id;
        param.create.service_id.is_primary = p_data->create.is_primary;
        param.create.service_id.id.inst_id = p_data->create.svc_instance;
        bta_to_btc_uuid(&param.create.service_id.id.uuid, &p_data->create.uuid);

        btc_gatts_cb_to_app(ESP_GATTS_CREATE_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_ADD_INCL_SRVC_EVT:
        gatts_if = p_data->add_result.server_if;
        param.add_incl_srvc.status = p_data->add_result.status;
        param.add_incl_srvc.attr_handle = p_data->add_result.attr_id;
        param.add_incl_srvc.service_handle = p_data->add_result.service_id;

        btc_gatts_cb_to_app(ESP_GATTS_ADD_INCL_SRVC_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_ADD_CHAR_EVT:
        gatts_if = p_data->add_result.server_if;
        param.add_char.status = p_data->add_result.status;
        param.add_char.attr_handle = p_data->add_result.attr_id;
        param.add_char.service_handle = p_data->add_result.service_id;
        bta_to_btc_uuid(&param.add_char.char_uuid, &p_data->add_result.char_uuid);

        btc_gatts_cb_to_app(ESP_GATTS_ADD_CHAR_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_ADD_CHAR_DESCR_EVT:
        gatts_if = p_data->add_result.server_if;
        param.add_char_descr.status = p_data->add_result.status;
        param.add_char_descr.attr_handle = p_data->add_result.attr_id;
        param.add_char_descr.service_handle = p_data->add_result.service_id;
        bta_to_btc_uuid(&param.add_char_descr.char_uuid, &p_data->add_result.char_uuid);

        btc_gatts_cb_to_app(ESP_GATTS_ADD_CHAR_DESCR_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_DELELTE_EVT:
        gatts_if = p_data->srvc_oper.server_if;
        param.del.status = p_data->srvc_oper.status;
        param.del.service_handle = p_data->srvc_oper.service_id;

        btc_gatts_cb_to_app(ESP_GATTS_DELETE_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_START_EVT:
        gatts_if = p_data->srvc_oper.server_if;
        param.start.status = p_data->srvc_oper.status;
        param.start.service_handle = p_data->srvc_oper.service_id;

        btc_gatts_cb_to_app(ESP_GATTS_START_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_STOP_EVT:
        gatts_if = p_data->srvc_oper.server_if;
        param.stop.status = p_data->srvc_oper.status;
        param.stop.service_handle = p_data->srvc_oper.service_id;

        btc_gatts_cb_to_app(ESP_GATTS_STOP_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_CONNECT_EVT:
        gatts_if = p_data->conn.server_if;
        param.connect.conn_id = BTC_GATT_GET_CONN_ID(p_data->conn.conn_id);
        param.connect.is_connected = true;
        memcpy(param.connect.remote_bda, p_data->conn.remote_bda, ESP_BD_ADDR_LEN);

        btc_gatts_cb_to_app(ESP_GATTS_CONNECT_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_DISCONNECT_EVT:
        gatts_if = p_data->conn.server_if;
        param.disconnect.conn_id = BTC_GATT_GET_CONN_ID(p_data->conn.conn_id);
        param.disconnect.is_connected = false;
        memcpy(param.disconnect.remote_bda, p_data->conn.remote_bda, ESP_BD_ADDR_LEN);

        btc_gatts_cb_to_app(ESP_GATTS_DISCONNECT_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_OPEN_EVT:
        gatts_if = p_data->open.server_if;
        param.open.status = p_data->open.status;

        btc_gatts_cb_to_app(BTA_GATTS_OPEN_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_CANCEL_OPEN_EVT:
        gatts_if = p_data->cancel_open.server_if;
        param.cancel_open.status = p_data->cancel_open.status;

        btc_gatts_cb_to_app(BTA_GATTS_CANCEL_OPEN_EVT, gatts_if, &param);
        break;

    case BTA_GATTS_CLOSE_EVT:
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->close.conn_id);
        param.close.status = p_data->close.status;
        param.close.conn_id = BTC_GATT_GET_CONN_ID(p_data->close.conn_id);

        btc_gatts_cb_to_app(BTA_GATTS_CLOSE_EVT, gatts_if, &param);
        break;

    case BTA_GATTS_LISTEN_EVT:
        // do nothing
        break;
    case BTA_GATTS_CONGEST_EVT:
        gatts_if = BTC_GATT_GET_GATT_IF(p_data->congest.conn_id);
        param.congest.conn_id = BTC_GATT_GET_CONN_ID(p_data->congest.conn_id);
        param.congest.congested = p_data->congest.congested;
        btc_gatts_cb_to_app(ESP_GATTS_CONGEST_EVT, gatts_if, &param);
        break;
    case BTA_GATTS_SET_ATTR_VAL_EVT:
        gatts_if = p_data->attr_val.server_if;
        param.set_attr_val.srvc_handle = p_data->attr_val.service_id;
        param.set_attr_val.attr_handle = p_data->attr_val.attr_id;
        param.set_attr_val.status = p_data->attr_val.status;
        btc_gatts_cb_to_app(ESP_GATTS_SET_ATTR_VAL_EVT, gatts_if, &param);
        break;
    default:
        // do nothing
        break;
    }

    btc_gatts_cb_param_copy_free(msg, p_data);
}

#endif  ///GATTS_INCLUDED