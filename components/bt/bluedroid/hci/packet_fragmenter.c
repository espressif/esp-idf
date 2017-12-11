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
#include <string.h>
#include "bt_trace.h"
#include "bt_defs.h"
#include "controller.h"
#include "buffer_allocator.h"
#include "hci_internals.h"
#include "hci_layer.h"
#include "packet_fragmenter.h"

#include "hash_map.h"
#include "hash_functions.h"
#include "bt_trace.h"


#define APPLY_CONTINUATION_FLAG(handle) (((handle) & 0xCFFF) | 0x1000)
#define APPLY_START_FLAG(handle) (((handle) & 0xCFFF) | 0x2000)
#define SUB_EVENT(event) ((event) & MSG_SUB_EVT_MASK)
#define GET_BOUNDARY_FLAG(handle) (((handle) >> 12) & 0x0003)

#define HANDLE_MASK 0x0FFF
#define START_PACKET_BOUNDARY 2
#define CONTINUATION_PACKET_BOUNDARY 1
#define L2CAP_HEADER_SIZE       4

// TODO(zachoverflow): find good value for this
#define NUMBER_OF_BUCKETS 42

// Our interface and callbacks
static const packet_fragmenter_t interface;
static const allocator_t *buffer_allocator;
static const controller_t *controller;
static const packet_fragmenter_callbacks_t *callbacks;
static hash_map_t *partial_packets;
static BT_HDR *current_fragment_packet;

static void init(const packet_fragmenter_callbacks_t *result_callbacks)
{
    current_fragment_packet = NULL;
    callbacks = result_callbacks;
    partial_packets = hash_map_new(NUMBER_OF_BUCKETS, hash_function_naive, NULL, NULL, NULL);
}

static void cleanup()
{
    if (partial_packets) {
        hash_map_free(partial_packets);
    }
}

static BT_HDR *fragment_get_current_packet()
{
    return current_fragment_packet;
}

static void fragment_and_dispatch(BT_HDR *packet)
{
    uint16_t continuation_handle;
    uint16_t max_data_size, max_packet_size, remaining_length;
    uint16_t event = packet->event & MSG_EVT_MASK;
    uint8_t *stream = packet->data + packet->offset;

    assert(packet != NULL);

    // We only fragment ACL packets
    if (event != MSG_STACK_TO_HC_HCI_ACL) {
        callbacks->fragmented(packet, true);
        return;
    }

    max_data_size =
        SUB_EVENT(packet->event) == LOCAL_BR_EDR_CONTROLLER_ID ?
        controller->get_acl_data_size_classic() :
        controller->get_acl_data_size_ble();

    max_packet_size = max_data_size + HCI_ACL_PREAMBLE_SIZE;
    remaining_length = packet->len;
    STREAM_TO_UINT16(continuation_handle, stream);
    continuation_handle = APPLY_CONTINUATION_FLAG(continuation_handle);
    if (remaining_length > max_packet_size) {
        current_fragment_packet = packet;
        UINT16_TO_STREAM(stream, max_data_size);
        packet->len = max_packet_size;
        callbacks->fragmented(packet, false);
        packet->offset += max_data_size;
        remaining_length -= max_data_size;
        packet->len = remaining_length;

        // Write the ACL header for the next fragment
        stream = packet->data + packet->offset;
        UINT16_TO_STREAM(stream, continuation_handle);
        UINT16_TO_STREAM(stream, remaining_length - HCI_ACL_PREAMBLE_SIZE);
        // Apparently L2CAP can set layer_specific to a max number of segments to transmit
        if (packet->layer_specific) {
            packet->layer_specific--;
            if (packet->layer_specific == 0) {
                packet->event = MSG_HC_TO_STACK_L2C_SEG_XMIT;

                /* The remain packet will send back to the l2cap layer when controller buffer is not enough
                   current_fragment_packet must be NULL, otherwise hci_host_thread_handler() will
                   connitue handle the remain packet. then the remain packet will be freed.
                */

                current_fragment_packet = NULL;
                callbacks->transmit_finished(packet, false);
                return;
            }
        }
    } else {
        current_fragment_packet = NULL;
        callbacks->fragmented(packet, true);
    }
}

static void reassemble_and_dispatch(BT_HDR *packet)
{
    LOG_DEBUG("reassemble_and_dispatch\n");

    if ((packet->event & MSG_EVT_MASK) == MSG_HC_TO_STACK_HCI_ACL) {
        uint8_t *stream = packet->data + packet->offset;
        uint16_t handle;
        uint16_t l2cap_length;
        uint16_t acl_length;

        STREAM_TO_UINT16(handle, stream);
        STREAM_TO_UINT16(acl_length, stream);
        STREAM_TO_UINT16(l2cap_length, stream);

        assert(acl_length == packet->len - HCI_ACL_PREAMBLE_SIZE);

        uint8_t boundary_flag = GET_BOUNDARY_FLAG(handle);
        handle = handle & HANDLE_MASK;

        BT_HDR *partial_packet = (BT_HDR *)hash_map_get(partial_packets, (void *)(uintptr_t)handle);

        if (boundary_flag == START_PACKET_BOUNDARY) {
            if (partial_packet) {
                LOG_WARN("%s found unfinished packet for handle with start packet. Dropping old.\n", __func__);
                hash_map_erase(partial_packets, (void *)(uintptr_t)handle);
                buffer_allocator->free(partial_packet);
            }

            uint16_t full_length = l2cap_length + L2CAP_HEADER_SIZE + HCI_ACL_PREAMBLE_SIZE;
            if (full_length <= packet->len) {
                if (full_length < packet->len) {
                    LOG_WARN("%s found l2cap full length %d less than the hci length %d.\n", __func__, l2cap_length, packet->len);
                }

                callbacks->reassembled(packet);
                return;
            }

            partial_packet = (BT_HDR *)buffer_allocator->alloc(full_length + sizeof(BT_HDR));
            partial_packet->event = packet->event;
            partial_packet->len = full_length;
            partial_packet->offset = packet->len;

            memcpy(partial_packet->data, packet->data + packet->offset, packet->len);

            // Update the ACL data size to indicate the full expected length
            stream = partial_packet->data;
            STREAM_SKIP_UINT16(stream); // skip the handle
            UINT16_TO_STREAM(stream, full_length - HCI_ACL_PREAMBLE_SIZE);

            hash_map_set(partial_packets, (void *)(uintptr_t)handle, partial_packet);
            // Free the old packet buffer, since we don't need it anymore
            buffer_allocator->free(packet);
        } else {
            if (!partial_packet) {
                LOG_ERROR("%s got continuation for unknown packet. Dropping it.\n", __func__);
                buffer_allocator->free(packet);
                return;
            }

            packet->offset += HCI_ACL_PREAMBLE_SIZE; // skip ACL preamble
            packet->len -= HCI_ACL_PREAMBLE_SIZE;
            uint16_t projected_offset = partial_packet->offset + packet->len;
            if (projected_offset > partial_packet->len) { // len stores the expected length
                LOG_ERROR("%s got packet which would exceed expected length of %d. Truncating.\n", __func__, partial_packet->len);
                packet->len = partial_packet->len - partial_packet->offset;
                projected_offset = partial_packet->len;
            }

            memcpy(
                partial_packet->data + partial_packet->offset,
                packet->data + packet->offset,
                packet->len
            );

            // Free the old packet buffer, since we don't need it anymore
            buffer_allocator->free(packet);
            partial_packet->offset = projected_offset;

            if (partial_packet->offset == partial_packet->len) {
                hash_map_erase(partial_packets, (void *)(uintptr_t)handle);
                partial_packet->offset = 0;
                callbacks->reassembled(partial_packet);
            }
        }
    } else {
        callbacks->reassembled(packet);
    }
}

static const packet_fragmenter_t interface = {
    init,
    cleanup,

    fragment_get_current_packet,
    fragment_and_dispatch,
    reassemble_and_dispatch
};

const packet_fragmenter_t *packet_fragmenter_get_interface()
{
    controller = controller_get_interface();
    buffer_allocator = buffer_allocator_get_interface();
    return &interface;
}

