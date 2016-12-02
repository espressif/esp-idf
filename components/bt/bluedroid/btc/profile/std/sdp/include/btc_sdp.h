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

#ifndef __ESP_SDP_API_H__
#define __ESP_SDP_API_H__

#include <stdint.h>
#include "esp_err.h"
#include "esp_bt_common.h"
#include "bta_sdp_api.h"
#include "bt_sdp.h"

#define BT_SDP_STAT_SUCCESS                      BTA_SDP_SUCCESS
#define BT_SDP_STAT_FAILURE                      BTA_SDP_FAILURE
#define BT_SDP_STAT_BUSY                         BTA_SDP_BUSY

#define BT_SDP_ENABLE_EVT                        BTA_SDP_ENABLE_EVT
#define BT_SDP_SEARCH_EVT                        BTA_SDP_SEARCH_EVT
#define BT_SDP_SEARCH_COMP_EVT                   BTA_SDP_SEARCH_COMP_EVT
#define BT_SDP_CREATE_RECORD_USER_EVT            BTA_SDP_CREATE_RECORD_USER_EVT
#define BT_SDP_REMOVE_RECORD_USER_EVT            BTA_SDP_REMOVE_RECORD_USER_EVT
#define BT_SDP_MAX_EVT                           BTA_SDP_MAX_EVT

#define BT_SDP_MAX_RECORDS                       BTA_SDP_MAX_RECORDS

typedef tBTA_SDP_STATUS bt_sdp_status_t;

typedef tBTA_SDP_EVT bt_sdp_evt_t;

typedef bluetooth_sdp_record bt_sdp_record_t;

/* tBTA_SEARCH_COMP, bta_sdp_api.h */
typedef struct {
    bt_sdp_status_t status;
    esp_bd_addr_t remote_addr;
    esp_bt_uuid_t uuid;
    int record_count;
    bt_sdp_record_t records[BT_SDP_MAX_RECORDS];
} bt_sdp_search_comp_t;

/* tBTA_SDP, bta_sdp_api.h */
typedef union {
    bt_sdp_status_t status;
    bt_sdp_search_comp_t sdp_search_comp;
} bt_sdp_t;

typedef void (bt_sdp_cb_t)(bt_sdp_evt_t event, bt_sdp_t *p_data, void *user_data);

esp_err_t esp_bt_sdp_enable(bt_sdp_cb_t *cback);

esp_err_t esp_bt_sdp_search(esp_bd_addr_t bd_addr, esp_bt_uuid_t *uuid);

esp_err_t esp_bt_sdp_create_record_by_user(void *user_data);

esp_err_t esp_bt_sdp_remove_record_by_user(void *user_data);


/**********************************************************************************************/
/**********************************************************************************************/
/* API into SDP for local service database updates
 * these APIs are indended to be called in callback function in the context of stack task,
 * to handle BT_SDP_CREATE_RECORD_USER_EVT and BT_SDP_REMOVE_RECORD_USER_EVT
 */

/* This structure is used to add protocol lists and find protocol elements */
#define  ESP_BT_SDP_MAX_PROTOCOL_PARAMS         SDP_MAX_PROTOCOL_PARAMS     // bt_target.h
typedef struct {
    uint16_t      protocol_uuid;
    uint16_t      num_params;
    uint16_t      params[ESP_BT_SDP_MAX_PROTOCOL_PARAMS];
} sdp_proto_elem_t; // tSDP_PROTOCOL_ELEM, sdp_api.h

#define ESP_BT_SDP_MAX_LIST_ELEMS               SDP_MAX_LIST_ELEMS // sdp_api.h
typedef struct {
    uint16_t              num_elems;
    sdp_proto_elem_t      list_elem[ESP_BT_SDP_MAX_LIST_ELEMS];
} sdp_proto_list_elem_t; // tSDP_PROTO_LIST_ELEM, sdp_api.h


uint32_t esp_bt_sdp_create_record(void);

bool esp_bt_sdp_delete_record(uint32_t handle);

int32_t esp_bt_sdp_read_record(uint32_t handle, uint8_t *data, int32_t *data_len);

bool esp_bt_sdp_add_attribute (uint32_t handle, uint16_t attr_id,
                               uint8_t attr_type, uint32_t attr_len,
                               uint8_t *p_val);

bool esp_bt_sdp_add_sequence (uint32_t handle,  uint16_t attr_id,
                              uint16_t num_elem, uint8_t type[],
                              uint8_t len[], uint8_t *p_val[]);

bool esp_bt_sdp_add_uuid_sequence (uint32_t handle,  uint16_t attr_id,
                                   uint16_t num_uuids, uint16_t *p_uuids);


bool esp_bt_sdp_add_protocol_list (uint32_t handle, uint16_t num_elem,
                                   sdp_proto_elem_t *p_elem_list);

bool esp_bt_sdp_add_addition_protocol_lists(uint32_t handle, uint16_t num_elem,
        sdp_proto_list_elem_t *p_proto_list);

bool esp_bt_sdp_add_profile_dscp_list (uint32_t handle,
                                       uint16_t profile_uuid,
                                       uint16_t version);

bool esp_bt_sdp_add_lang_base_attr_id_list(uint32_t handle,
        uint16_t lang, uint16_t char_enc,
        uint16_t base_id);

bool esp_bt_sdp_add_service_class_id_list(uint32_t handle,
        uint16_t num_services,
        uint16_t *p_service_uuids);

bool esp_bt_sdp_delete_attribute(uint32_t handle, uint16_t attr_id);

#endif /* __ESP_SDP_API_H__ */
