/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _HCI_PACKET_PARSER_H_
#define _HCI_PACKET_PARSER_H_

#include <stdint.h>

#include "osi/allocator.h"
#include "device/bdaddr.h"
#include "stack/bt_types.h"
#include "device/device_features.h"
//#include "features.h"
#include "device/version.h"

typedef struct {
    void (*parse_generic_command_complete)(BT_HDR *response);

    void (*parse_read_buffer_size_response)(
        BT_HDR *response,
        uint16_t *acl_data_size_ptr,
        uint16_t *acl_buffer_count_ptr,
        uint8_t *sco_data_size_ptr,
        uint16_t *sco_buffer_count_ptr
    );

    void (*parse_read_local_version_info_response)(
        BT_HDR *response,
        bt_version_t *bt_version_ptr
    );

    void (*parse_read_bd_addr_response)(
        BT_HDR *response,
        bt_bdaddr_t *address_ptr
    );

    void (*parse_read_local_supported_commands_response)(
        BT_HDR *response,
        uint8_t *supported_commands_ptr,
        size_t supported_commands_length
    );

    void (*parse_read_local_supported_features_response)(
        BT_HDR *response,
        bt_device_features_t *feature_pages
    );

    void (*parse_read_local_extended_features_response)(
        BT_HDR *response,
        uint8_t *page_number_ptr,
        uint8_t *max_page_number_ptr,
        bt_device_features_t *feature_pages,
        size_t feature_pages_count
    );

    void (*parse_ble_read_white_list_size_response)(
        BT_HDR *response,
        uint8_t *white_list_size_ptr
    );

    void (*parse_ble_read_buffer_size_response)(
        BT_HDR *response,
        uint16_t *data_size_ptr,
        uint8_t *acl_buffer_count_ptr
    );

    void (*parse_ble_read_supported_states_response)(
        BT_HDR *response,
        uint8_t *supported_states,
        size_t supported_states_size
    );

    void (*parse_ble_read_local_supported_features_response)(
        BT_HDR *response,
        bt_device_features_t *supported_features
    );

    void (*parse_ble_read_resolving_list_size_response) (
        BT_HDR *response,
        uint8_t *resolving_list_size_ptr
    );
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    void (*parse_ble_read_adv_max_len_response) (
        BT_HDR *respone,
        uint16_t *ble_ext_adv_data_max_len_ptr
    );
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    void (*parse_ble_read_suggested_default_data_length_response)(
        BT_HDR *response,
        uint16_t *ble_default_packet_length_ptr,
        uint16_t *ble_default_packet_txtime_ptr
    );
} hci_packet_parser_t;

const hci_packet_parser_t *hci_packet_parser_get_interface(void);

#endif /*_HCI_PACKET_PARSER_H_*/
