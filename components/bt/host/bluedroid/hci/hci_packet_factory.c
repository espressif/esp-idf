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

#include "common/bt_defs.h"

#include "osi/allocator.h"
#include "stack/bt_types.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"
#include "hci/hci_internals.h"
#include "hci/hci_layer.h"
#include "hci/hci_packet_factory.h"


static BT_HDR *make_command_no_params(uint16_t opcode);
static BT_HDR *make_command(uint16_t opcode, size_t parameter_size, uint8_t **stream_out);

// Interface functions

static BT_HDR *make_reset(void)
{
    return make_command_no_params(HCI_RESET);
}

static BT_HDR *make_read_buffer_size(void)
{
    return make_command_no_params(HCI_READ_BUFFER_SIZE);
}

static BT_HDR *make_set_c2h_flow_control(uint8_t enable)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1;
    BT_HDR *packet = make_command(HCI_SET_HC_TO_HOST_FLOW_CTRL, parameter_size, &stream);

    UINT8_TO_STREAM(stream, enable);
    return packet;
}

static BT_HDR *make_set_adv_report_flow_control(uint8_t enable, uint16_t num, uint16_t lost_threshold)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1 + 2 + 2;
    BT_HDR *packet = make_command(HCI_VENDOR_BLE_SET_ADV_FLOW_CONTROL, parameter_size, &stream);

    UINT8_TO_STREAM(stream, enable);
    UINT16_TO_STREAM(stream, num);
    UINT16_TO_STREAM(stream, lost_threshold);
    return packet;
}

static BT_HDR *make_host_buffer_size(uint16_t acl_size, uint8_t sco_size, uint16_t acl_count, uint16_t sco_count)
{
    uint8_t *stream;
    const uint8_t parameter_size = 2 + 1 + 2 + 2; // from each of the parameters
    BT_HDR *packet = make_command(HCI_HOST_BUFFER_SIZE, parameter_size, &stream);

    UINT16_TO_STREAM(stream, acl_size);
    UINT8_TO_STREAM(stream, sco_size);
    UINT16_TO_STREAM(stream, acl_count);
    UINT16_TO_STREAM(stream, sco_count);
    return packet;
}

static BT_HDR *make_read_local_version_info(void)
{
    return make_command_no_params(HCI_READ_LOCAL_VERSION_INFO);
}

static BT_HDR *make_read_bd_addr(void)
{
    return make_command_no_params(HCI_READ_BD_ADDR);
}

static BT_HDR *make_read_local_supported_commands(void)
{
    return make_command_no_params(HCI_READ_LOCAL_SUPPORTED_CMDS);
}

static BT_HDR *make_read_local_supported_features(void)
{
    return make_command_no_params(HCI_READ_LOCAL_FEATURES);
}

static BT_HDR *make_read_local_extended_features(uint8_t page_number)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1;
    BT_HDR *packet = make_command(HCI_READ_LOCAL_EXT_FEATURES, parameter_size, &stream);

    UINT8_TO_STREAM(stream, page_number);
    return packet;
}

static BT_HDR *make_write_simple_pairing_mode(uint8_t mode)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1;
    BT_HDR *packet = make_command(HCI_WRITE_SIMPLE_PAIRING_MODE, parameter_size, &stream);

    UINT8_TO_STREAM(stream, mode);
    return packet;
}

static BT_HDR *make_write_secure_connections_host_support(uint8_t mode)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1;
    BT_HDR *packet = make_command(HCI_WRITE_SECURE_CONNS_SUPPORT, parameter_size, &stream);

    UINT8_TO_STREAM(stream, mode);
    return packet;
}

static BT_HDR *make_set_event_mask(const bt_event_mask_t *event_mask)
{
    uint8_t *stream;
    uint8_t parameter_size = sizeof(bt_event_mask_t);
    BT_HDR *packet = make_command(HCI_SET_EVENT_MASK, parameter_size, &stream);

    ARRAY8_TO_STREAM(stream, event_mask->as_array);
    return packet;
}

static BT_HDR *make_ble_write_host_support(uint8_t supported_host, uint8_t simultaneous_host)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1 + 1;
    BT_HDR *packet = make_command(HCI_WRITE_LE_HOST_SUPPORT, parameter_size, &stream);

    UINT8_TO_STREAM(stream, supported_host);
    UINT8_TO_STREAM(stream, simultaneous_host);
    return packet;
}

static BT_HDR *make_ble_read_white_list_size(void)
{
    return make_command_no_params(HCI_BLE_READ_WHITE_LIST_SIZE);
}

static BT_HDR *make_ble_read_buffer_size(void)
{
    return make_command_no_params(HCI_BLE_READ_BUFFER_SIZE);
}

static BT_HDR *make_ble_read_supported_states(void)
{
    return make_command_no_params(HCI_BLE_READ_SUPPORTED_STATES);
}

static BT_HDR *make_ble_read_local_supported_features(void)
{
    return make_command_no_params(HCI_BLE_READ_LOCAL_SPT_FEAT);
}

static BT_HDR *make_ble_read_resolving_list_size(void)
{
    return make_command_no_params(HCI_BLE_READ_RESOLVING_LIST_SIZE);
}

static BT_HDR *make_ble_read_suggested_default_data_length(void)
{
    return make_command_no_params(HCI_BLE_READ_DEFAULT_DATA_LENGTH);
}

static BT_HDR *make_ble_write_suggested_default_data_length(uint16_t SuggestedMaxTxOctets, uint16_t SuggestedMaxTxTime)
{
    uint8_t *stream;
    uint8_t parameter_size = sizeof(uint16_t) + sizeof(uint16_t);
    BT_HDR *packet = make_command(HCI_BLE_WRITE_DEFAULT_DATA_LENGTH, parameter_size, &stream);

    UINT16_TO_STREAM(stream, SuggestedMaxTxOctets);
    UINT16_TO_STREAM(stream, SuggestedMaxTxTime);
    return packet;
}

static BT_HDR *make_ble_set_event_mask(const bt_event_mask_t *event_mask)
{
    uint8_t *stream;
    uint8_t parameter_size = sizeof(bt_event_mask_t);
    BT_HDR *packet = make_command(HCI_BLE_SET_EVENT_MASK, parameter_size, &stream);

    ARRAY8_TO_STREAM(stream, event_mask->as_array);
    return packet;
}

static BT_HDR *make_write_sync_flow_control_enable(uint8_t enable)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1;
    BT_HDR *packet = make_command(HCI_WRITE_SCO_FLOW_CTRL_ENABLE, parameter_size, &stream);

    UINT8_TO_STREAM(stream, enable);
    return packet;
}

static BT_HDR *make_write_default_erroneous_data_report(uint8_t enable)
{
    uint8_t *stream;
    const uint8_t parameter_size = 1;
    BT_HDR *packet = make_command(HCI_WRITE_ERRONEOUS_DATA_RPT, parameter_size, &stream);

    UINT8_TO_STREAM(stream, enable);
    return packet;
}
#if (BLE_50_FEATURE_SUPPORT == TRUE)
static BT_HDR *make_read_max_adv_data_len(void)
{
    return make_command_no_params(HCI_BLE_RD_MAX_ADV_DATA_LEN);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
// Internal functions

static BT_HDR *make_command_no_params(uint16_t opcode)
{
    return make_command(opcode, 0, NULL);
}

static BT_HDR *make_command(uint16_t opcode, size_t parameter_size, uint8_t **stream_out)
{
    BT_HDR *packet = HCI_GET_CMD_BUF(parameter_size);
    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(packet);
    metadata->opcode = opcode;

    uint8_t *stream = packet->data;
    UINT16_TO_STREAM(stream, opcode);
    UINT8_TO_STREAM(stream, parameter_size);

    if (stream_out != NULL) {
        *stream_out = stream;
    }

    return packet;
}

static const hci_packet_factory_t interface = {
    make_reset,
    make_read_buffer_size,
    make_set_c2h_flow_control,
    make_set_adv_report_flow_control,
    make_host_buffer_size,
    make_read_local_version_info,
    make_read_bd_addr,
    make_read_local_supported_commands,
    make_read_local_supported_features,
    make_read_local_extended_features,
    make_write_simple_pairing_mode,
    make_write_secure_connections_host_support,
    make_set_event_mask,
    make_ble_write_host_support,
    make_ble_read_white_list_size,
    make_ble_read_buffer_size,
    make_ble_read_supported_states,
    make_ble_read_local_supported_features,
    make_ble_read_resolving_list_size,
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    make_read_max_adv_data_len,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    make_ble_read_suggested_default_data_length,
    make_ble_write_suggested_default_data_length,
    make_ble_set_event_mask,
    make_write_sync_flow_control_enable,
    make_write_default_erroneous_data_report,
};

const hci_packet_factory_t *hci_packet_factory_get_interface(void)
{
    return &interface;
}
