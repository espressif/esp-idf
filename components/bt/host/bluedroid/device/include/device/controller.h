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

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdbool.h>
#include <stdint.h>

#include "common/bt_target.h"
#include "device/bdaddr.h"
#include "device/device_features.h"
#include "hci/hci_layer.h"
#include "hci/hci_packet_factory.h"
#include "hci/hci_packet_parser.h"

typedef struct controller_t {
    void (*start_up)(void);
    void (*shut_down)(void);
    bool (*get_is_ready)(void);

    const bt_bdaddr_t *(*get_address)(void);
    const bt_version_t *(*get_bt_version)(void);

    const bt_device_features_t *(*get_features_classic)(int index);

    uint8_t (*get_last_features_classic_index)(void);

    const bt_device_features_t *(*get_features_ble)(void);
    const uint8_t *(*get_ble_supported_states)(void);

    bool (*supports_simple_pairing)(void);
    bool (*supports_secure_connections)(void);
    bool (*supports_simultaneous_le_bredr)(void);
    bool (*supports_reading_remote_extended_features)(void);
    bool (*supports_interlaced_inquiry_scan)(void);
    bool (*supports_rssi_with_inquiry_results)(void);
    bool (*supports_extended_inquiry_response)(void);
    bool (*supports_master_slave_role_switch)(void);

    bool (*supports_ble)(void);
    bool (*supports_ble_packet_extension)(void);
    bool (*supports_ble_connection_parameters_request)(void);
    bool (*supports_ble_privacy)(void);

    // Get the cached acl data sizes for the controller.
    uint16_t (*get_acl_data_size_classic)(void);
    uint16_t (*get_acl_data_size_ble)(void);

    // Get the cached acl packet sizes for the controller.
    // This is a convenience function for the respective
    // acl data size + size of the acl header.
    uint16_t (*get_acl_packet_size_classic)(void);
    uint16_t (*get_acl_packet_size_ble)(void);

    uint16_t (*get_ble_default_data_packet_length)(void);
    uint16_t (*get_ble_default_data_packet_txtime)(void);

    // Get the number of acl packets the controller can buffer.
    uint16_t (*get_acl_buffer_count_classic)(void);
    uint8_t (*get_acl_buffer_count_ble)(void);

    uint8_t (*get_ble_white_list_size)(void);

    uint8_t (*get_ble_resolving_list_max_size)(void);
    void (*set_ble_resolving_list_max_size)(int resolving_list_max_size);

#if (BLE_50_FEATURE_SUPPORT == TRUE)
    uint16_t (*ble_get_ext_adv_data_max_len)(void);
#endif // BLE_50_FEATURE_SUPPORT

#if (BTM_SCO_HCI_INCLUDED == TRUE)
    // Get the number of sco packets the controller can buffer
    uint8_t (*get_sco_data_size)(void);
    uint8_t (*get_sco_buffer_count)(void);
#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */
} controller_t;

const controller_t *controller_get_interface(void);

#endif /*_CONTROLLER_H_*/
