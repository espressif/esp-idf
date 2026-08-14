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

#ifndef _PACKET_FRAGMENTER_H_
#define _PACKET_FRAGMENTER_H_

#include "osi/allocator.h"
#include "stack/bt_types.h"
#include "hci/hci_layer.h"

typedef void (*transmit_finished_cb)(BT_HDR *packet, bool all_fragments_sent);
typedef void (*packet_reassembled_cb)(BT_HDR *packet);
typedef void (*packet_fragmented_cb)(BT_HDR *packet, bool send_transmit_finished);
#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
typedef void (*packet_c2h_credit_cb)(uint16_t handle, uint16_t num);
#endif

typedef struct {
    // Called for every packet fragment.
    packet_fragmented_cb fragmented;

    // Called for every completely reassembled packet.
    packet_reassembled_cb reassembled;

    // Called when the fragmenter finishes sending all requested fragments,
    // but the packet has not been entirely sent.
    transmit_finished_cb transmit_finished;

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)
    // Called when an inbound ACL fragment is absorbed or dropped without
    // being delivered via reassembled.
    packet_c2h_credit_cb c2h_credit;
#endif
} packet_fragmenter_callbacks_t;

typedef struct packet_fragmenter_t {
    // Initialize the fragmenter, specifying the |result_callbacks|.
    void (*init)(const packet_fragmenter_callbacks_t *result_callbacks);

    // Release all resources associated with the fragmenter.
    void (*cleanup)(void);

    // Check if Current fragmenter is ongoing
    BT_HDR *(*fragment_current_packet)(void);

    // Fragments |packet| if necessary and hands off everything to the fragmented callback.
    void (*fragment_and_dispatch)(BT_HDR *packet);
    // If |packet| is a complete packet, forwards to the reassembled callback. Otherwise
    // holds onto it until all fragments arrive, at which point the reassembled callback is called
    // with the reassembled data.
    void (*reassemble_and_dispatch)(BT_HDR *packet);

    // Drop any in-progress inbound ACL reassembly for |handle| and return its
    // C2H START credit. Must run on the HCI thread; no-op if none is held.
    void (*flush_reassembly)(uint16_t handle);
} packet_fragmenter_t;

const packet_fragmenter_t *packet_fragmenter_get_interface(void);

#if defined(BT_BLUEDROID_UNIT_TEST)
const packet_fragmenter_t *packet_fragmenter_get_test_interface(void);
#endif

#endif /* _PACKET_FRAGMENTER_H_ */
