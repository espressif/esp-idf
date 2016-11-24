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

#include "esp_sdp_api.h"


esp_err_t esp_bt_sdp_enable(bt_sdp_cb_t *cback)
{
    tBTA_SDP_STATUS status = BTA_SdpEnable((tBTA_SDP_DM_CBACK *)cback);

    return (status == BTA_SDP_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_sdp_search(esp_bd_addr_t bd_addr, esp_bt_uuid_t *uuid)
{
    tBTA_SDP_STATUS status = BTA_SdpSearch(bd_addr, (tSDP_UUID *)uuid);
    return (status == BTA_SDP_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_sdp_create_record_by_user(void *user_data)
{
    tBTA_SDP_STATUS status = BTA_SdpCreateRecordByUser(user_data);
    return (status == BTA_SDP_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_sdp_remove_record_by_user(void *user_data)
{
    tBTA_SDP_STATUS status = BTA_SdpRemoveRecordByUser(user_data);
    return (status == BTA_SDP_SUCCESS) ? ESP_OK : ESP_FAIL;
}


/**********************************************************************************************/
/**********************************************************************************************/
/* API into SDP for local service database updates */
/* these APIs are indended to be called in callback function in the context of stack task,
 * to handle BT_SDP_CREATE_RECORD_USER_EVT and BT_SDP_REMOVE_RECORD_USER_EVT
 */
uint32_t esp_bt_sdp_create_record(void)
{
    return SDP_CreateRecord();
}

bool esp_bt_sdp_delete_record(uint32_t handle)
{
    return SDP_DeleteRecord(handle);
}

int32_t esp_bt_sdp_read_record(uint32_t handle, uint8_t *data, int32_t *data_len)
{
    return SDP_ReadRecord(handle, data, data_len);
}

bool esp_bt_sdp_add_attribute (uint32_t handle, uint16_t attr_id,
                               uint8_t attr_type, uint32_t attr_len,
                               uint8_t *p_val)
{
    return SDP_AddAttribute(handle, attr_id, attr_type, attr_len, p_val);
}

bool esp_bt_sdp_add_sequence (uint32_t handle,  uint16_t attr_id,
                              uint16_t num_elem, uint8_t type[],
                              uint8_t len[], uint8_t *p_val[])
{
    return SDP_AddSequence(handle, attr_id, num_elem, type, len, p_val);
}

bool esp_bt_sdp_add_uuid_sequence (uint32_t handle,  uint16_t attr_id,
                                   uint16_t num_uuids, uint16_t *p_uuids)
{
    return SDP_AddUuidSequence(handle, attr_id, num_uuids, p_uuids);
}


bool esp_bt_sdp_add_protocol_list (uint32_t handle, uint16_t num_elem,
                                   sdp_proto_elem_t *p_elem_list)
{
    return SDP_AddProtocolList(handle, num_elem, (tSDP_PROTOCOL_ELEM *)p_elem_list);
}

bool esp_bt_sdp_add_addition_protocol_lists(uint32_t handle, uint16_t num_elem,
        sdp_proto_list_elem_t *p_proto_list)
{
    return SDP_AddAdditionProtoLists(handle, num_elem, (tSDP_PROTO_LIST_ELEM *)p_proto_list);
}

bool esp_bt_sdp_add_profile_dscp_list (uint32_t handle,
                                       uint16_t profile_uuid,
                                       uint16_t version)
{
    return SDP_AddProfileDescriptorList(handle, profile_uuid, version);
}

bool esp_bt_sdp_add_lang_base_attr_id_list(uint32_t handle,
        uint16_t lang, uint16_t char_enc,
        uint16_t base_id)
{
    return SDP_AddLanguageBaseAttrIDList(handle, lang, char_enc, base_id);
}

bool esp_bt_sdp_add_service_class_id_list(uint32_t handle,
        uint16_t num_services,
        uint16_t *p_service_uuids)
{
    return SDP_AddServiceClassIdList(handle, num_services, p_service_uuids);
}

bool esp_bt_sdp_delete_attribute(uint32_t handle, uint16_t attr_id)
{
    return SDP_DeleteAttribute(handle, attr_id);
}

/**********************************************************************************************/
/**********************************************************************************************/
