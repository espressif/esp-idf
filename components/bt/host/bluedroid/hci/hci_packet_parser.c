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

#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "hci/hci_layer.h"
#include "hci/hci_packet_parser.h"

static const command_opcode_t NO_OPCODE_CHECKING = 0;

static uint8_t *read_command_complete_header(
    BT_HDR *response,
    command_opcode_t expected_opcode,
    size_t minimum_bytes_after);

static void parse_generic_command_complete(BT_HDR *response)
{
    read_command_complete_header(response, NO_OPCODE_CHECKING, 0 /* bytes after */);

    osi_free(response);
}

static void parse_read_buffer_size_response(
    BT_HDR *response,
    uint16_t *acl_data_size_ptr,
    uint16_t *acl_buffer_count_ptr,
    uint8_t *sco_data_size_ptr,
    uint16_t *sco_buffer_count_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_READ_BUFFER_SIZE, 7 /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_UINT16(*acl_data_size_ptr, stream);
    STREAM_TO_UINT8(*sco_data_size_ptr, stream);
    STREAM_TO_UINT16(*acl_buffer_count_ptr, stream);
    STREAM_TO_UINT16(*sco_buffer_count_ptr, stream);
    osi_free(response);
}

static void parse_read_local_version_info_response(
    BT_HDR *response,
    bt_version_t *bt_version)
{

    uint8_t *stream = read_command_complete_header(response, HCI_READ_LOCAL_VERSION_INFO, 8 /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_UINT8(bt_version->hci_version, stream);
    STREAM_TO_UINT16(bt_version->hci_revision, stream);
    STREAM_TO_UINT8(bt_version->lmp_version, stream);
    STREAM_TO_UINT16(bt_version->manufacturer, stream);
    STREAM_TO_UINT16(bt_version->lmp_subversion, stream);

    osi_free(response);
}

static void parse_read_bd_addr_response(
    BT_HDR *response,
    bt_bdaddr_t *address_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_READ_BD_ADDR, sizeof(bt_bdaddr_t) /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_BDADDR(address_ptr->address, stream);

    osi_free(response);
}

static void parse_read_local_supported_commands_response(
    BT_HDR *response,
    uint8_t *supported_commands_ptr,
    size_t supported_commands_length)
{

    uint8_t *stream = read_command_complete_header(response, HCI_READ_LOCAL_SUPPORTED_CMDS, supported_commands_length /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_ARRAY(supported_commands_ptr, stream, (int)supported_commands_length);

    osi_free(response);
}

static void parse_read_local_supported_features_response(
    BT_HDR *response,
    bt_device_features_t *feature_pages)
{
    uint8_t *stream = read_command_complete_header(response, HCI_READ_LOCAL_FEATURES, sizeof(bt_device_features_t) /* bytes after */);
    if (stream != NULL) {
        STREAM_TO_ARRAY(feature_pages->as_array, stream, (int)sizeof(bt_device_features_t));
    }
    osi_free(response);
}

static void parse_read_local_extended_features_response(
    BT_HDR *response,
    uint8_t *page_number_ptr,
    uint8_t *max_page_number_ptr,
    bt_device_features_t *feature_pages,
    size_t feature_pages_count)
{

    uint8_t *stream = read_command_complete_header(response, HCI_READ_LOCAL_EXT_FEATURES, 2 + sizeof(bt_device_features_t) /* bytes after */);
    if (stream != NULL) {
        STREAM_TO_UINT8(*page_number_ptr, stream);
        STREAM_TO_UINT8(*max_page_number_ptr, stream);

        assert(*page_number_ptr < feature_pages_count);
        STREAM_TO_ARRAY(feature_pages[*page_number_ptr].as_array, stream, (int)sizeof(bt_device_features_t));
    } else {
        HCI_TRACE_ERROR("%s() - WARNING: READING EXTENDED FEATURES FAILED. "
                  "THIS MAY INDICATE A FIRMWARE/CONTROLLER ISSUE.", __func__);
    }

    osi_free(response);
}

static void parse_ble_read_white_list_size_response(
    BT_HDR *response,
    uint8_t *white_list_size_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_READ_WHITE_LIST_SIZE, 1 /* byte after */);
    assert(stream != NULL);
    STREAM_TO_UINT8(*white_list_size_ptr, stream);

    osi_free(response);
}

static void parse_ble_read_buffer_size_response(
    BT_HDR *response,
    uint16_t *data_size_ptr,
    uint8_t *acl_buffer_count_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_READ_BUFFER_SIZE, 3 /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_UINT16(*data_size_ptr, stream);
    STREAM_TO_UINT8(*acl_buffer_count_ptr, stream);

    osi_free(response);
}

static void parse_ble_read_supported_states_response(
    BT_HDR *response,
    uint8_t *supported_states,
    size_t supported_states_size)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_READ_SUPPORTED_STATES, supported_states_size /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_ARRAY(supported_states, stream, (int)supported_states_size);

    osi_free(response);
}

static void parse_ble_read_local_supported_features_response(
    BT_HDR *response,
    bt_device_features_t *supported_features)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_READ_LOCAL_SPT_FEAT, sizeof(bt_device_features_t) /* bytes after */);
    assert(stream != NULL);
    STREAM_TO_ARRAY(supported_features->as_array, stream, (int)sizeof(bt_device_features_t));

    osi_free(response);
}

static void parse_ble_read_resolving_list_size_response(
    BT_HDR *response,
    uint8_t *resolving_list_size_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_READ_RESOLVING_LIST_SIZE, 1 /* bytes after */);
    STREAM_TO_UINT8(*resolving_list_size_ptr, stream);

    osi_free(response);
}

static void parse_ble_read_suggested_default_data_length_response(
    BT_HDR *response,
    uint16_t *ble_default_packet_length_ptr,
    uint16_t *ble_default_packet_txtime_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_READ_DEFAULT_DATA_LENGTH, 2 /* bytes after */);
    STREAM_TO_UINT16(*ble_default_packet_length_ptr, stream);
    STREAM_TO_UINT16(*ble_default_packet_txtime_ptr, stream);
    osi_free(response);
}
#if (BLE_50_FEATURE_SUPPORT == TRUE)
static void parse_ble_read_adv_max_len_response(
    BT_HDR *response,
    uint16_t *adv_max_len_ptr)
{

    uint8_t *stream = read_command_complete_header(response, HCI_BLE_RD_MAX_ADV_DATA_LEN, 1 /* bytes after */);
    STREAM_TO_UINT8(*adv_max_len_ptr, stream);

    osi_free(response);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)


// Internal functions

static uint8_t *read_command_complete_header(
    BT_HDR *response,
    command_opcode_t expected_opcode,
    size_t minimum_bytes_after)
{

    uint8_t *stream = response->data + response->offset;

    // Read the event header
    uint8_t event_code;
    uint8_t parameter_length;
    STREAM_TO_UINT8(event_code, stream);
    STREAM_TO_UINT8(parameter_length, stream);

    const size_t parameter_bytes_we_read_here = 4;

    // Check the event header values against what we expect
    assert(event_code == HCI_COMMAND_COMPLETE_EVT);
    assert(parameter_length >= (parameter_bytes_we_read_here + minimum_bytes_after));

    // Read the command complete header
    command_opcode_t opcode;
    uint8_t status;
    STREAM_SKIP_UINT8(stream); // skip the number of hci command packets field
    STREAM_TO_UINT16(opcode, stream);

    // Check the command complete header values against what we expect
    if (expected_opcode != NO_OPCODE_CHECKING) {
        assert(opcode == expected_opcode);
    }

    // Assume the next field is the status field
    STREAM_TO_UINT8(status, stream);

    if (status != HCI_SUCCESS) {
        return NULL;
    }

    return stream;
}

static const hci_packet_parser_t interface = {
    parse_generic_command_complete,
    parse_read_buffer_size_response,
    parse_read_local_version_info_response,
    parse_read_bd_addr_response,
    parse_read_local_supported_commands_response,
    parse_read_local_supported_features_response,
    parse_read_local_extended_features_response,
    parse_ble_read_white_list_size_response,
    parse_ble_read_buffer_size_response,
    parse_ble_read_supported_states_response,
    parse_ble_read_local_supported_features_response,
    parse_ble_read_resolving_list_size_response,
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    parse_ble_read_adv_max_len_response,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    parse_ble_read_suggested_default_data_length_response
};

const hci_packet_parser_t *hci_packet_parser_get_interface(void)
{
    return &interface;
}
