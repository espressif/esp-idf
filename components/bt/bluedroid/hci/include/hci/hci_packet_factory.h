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

#ifndef _HCI_PACKET_FACTORY_H_
#define _HCI_PACKET_FACTORY_H_

#include "stack/bt_types.h"
#include "device/event_mask.h"

typedef struct {
    BT_HDR *(*make_reset)(void);
    BT_HDR *(*make_read_buffer_size)(void);
    BT_HDR *(*make_set_c2h_flow_control)(uint8_t enable);
    BT_HDR *(*make_host_buffer_size)(uint16_t acl_size, uint8_t sco_size, uint16_t acl_count, uint16_t sco_count);
    BT_HDR *(*make_read_local_version_info)(void);
    BT_HDR *(*make_read_bd_addr)(void);
    BT_HDR *(*make_read_local_supported_commands)(void);
    BT_HDR *(*make_read_local_extended_features)(uint8_t page_number);
    BT_HDR *(*make_write_simple_pairing_mode)(uint8_t mode);
    BT_HDR *(*make_write_secure_connections_host_support)(uint8_t mode);
    BT_HDR *(*make_set_event_mask)(const bt_event_mask_t *event_mask);
    BT_HDR *(*make_ble_write_host_support)(uint8_t supported_host, uint8_t simultaneous_host);
    BT_HDR *(*make_ble_read_white_list_size)(void);
    BT_HDR *(*make_ble_read_buffer_size)(void);
    BT_HDR *(*make_ble_read_supported_states)(void);
    BT_HDR *(*make_ble_read_local_supported_features)(void);
    BT_HDR *(*make_ble_read_resolving_list_size)(void);
    BT_HDR *(*make_ble_read_suggested_default_data_length)(void);
    BT_HDR *(*make_ble_write_suggested_default_data_length)(uint16_t SuggestedMaxTxOctets, uint16_t SuggestedMaxTxTime);
    BT_HDR *(*make_ble_set_event_mask)(const bt_event_mask_t *event_mask);
    BT_HDR *(*make_write_sync_flow_control_enable)(uint8_t enable);
    BT_HDR *(*make_write_default_erroneous_data_report)(uint8_t enable);
} hci_packet_factory_t;

const hci_packet_factory_t *hci_packet_factory_get_interface();

#endif /*_HCI_PACKET_FACTORY_H_*/
