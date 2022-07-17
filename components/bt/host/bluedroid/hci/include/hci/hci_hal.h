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

#ifndef _HCI_HAL_H_
#define _HCI_HAL_H_

#include <stdbool.h>
#include <stdint.h>
#include "osi/pkt_queue.h"
#include "stack/bt_types.h"
#if SOC_ESP_NIMBLE_CONTROLLER
#include "os/os_mbuf.h"
#endif
typedef enum {
    DATA_TYPE_COMMAND = 1,
    DATA_TYPE_ACL     = 2,
    DATA_TYPE_SCO     = 3,
    DATA_TYPE_EVENT   = 4
} serial_data_type_t;

typedef void (*packet_ready_cb)(BT_HDR *packet);
typedef void (*adv_rpt_ready_cb)(pkt_linked_item_t *linked_pkt);

typedef struct {
    // Called when the HAL detects inbound data.
    // Data |type| may be ACL, SCO, or EVENT.
    // Executes in the context of the thread supplied to |init|.
    packet_ready_cb packet_ready;
    adv_rpt_ready_cb adv_rpt_ready;

    /*
    // Called when the HAL detects inbound astronauts named Dave.
    // HAL will deny all requests to open the pod bay doors after this.
    dave_ready_cb dave_ready;
    */
} hci_hal_callbacks_t;

typedef struct hci_hal_t {
    // Initialize the HAL, with |upper_callbacks| and |upper_thread| to run in the context of.
    //bool (*init)(const hci_hal_callbacks_t *upper_callbacks);

    // Connect to the underlying hardware, and let data start flowing.
    bool (*open)(const hci_hal_callbacks_t *upper_callbacks,  void *task_thread);
    // Disconnect from the underlying hardware, and close the HAL.
    // "Daisy, Daisy..."
    void (*close)(void);

    // Retrieve up to |max_size| bytes for ACL, SCO, or EVENT data packets into
    // |buffer|, blocking until max_size bytes read if |block| is true.
    // Only guaranteed to be correct in the context of a data_ready callback
    // of the corresponding type.
    //size_t (*read_data)(serial_data_type_t type, uint8_t *buffer, size_t max_size);
    // The upper layer must call this to notify the HAL that it has finished
    // reading a packet of the specified |type|. Underlying implementations that
    // use shared channels for multiple data types depend on this to know when
    // to reinterpret the data stream.
    //void (*packet_finished)(serial_data_type_t type);
    // Transmit COMMAND, ACL, or SCO data packets.
    // |data| may not be NULL. |length| must be greater than zero.
    //
    // IMPORTANT NOTE:
    // Depending on the underlying implementation, the byte right
    // before the beginning of |data| may be borrowed during this call
    // and then restored to its original value.
    // This is safe in the bluetooth context, because there is always a buffer
    // header that prefixes data you're sending.
    uint16_t (*transmit_data)(serial_data_type_t type, uint8_t *data, uint16_t length);
} hci_hal_t;


// Gets the correct hal implementation, as compiled for.
const hci_hal_t *hci_hal_h4_get_interface(void);
#if SOC_ESP_NIMBLE_CONTROLLER
int ble_hs_hci_rx_evt(uint8_t *hci_ev, void *arg);

int ble_hs_rx_data(struct os_mbuf *om, void *arg);
#endif


#endif /* _HCI_HAL_H */
