// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/queue.h>
#include "usb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *data_buffer;
    size_t data_buffer_size;
    int num_bytes;
    int actual_num_bytes;
    uint32_t flags;
    usb_transfer_status_t status;
    uint32_t timeout;
    usb_transfer_cb_t callback;
    void *context;
    int num_isoc_packets;
    usb_isoc_packet_desc_t isoc_packet_desc[0];
} usb_transfer_dummy_t;
_Static_assert(sizeof(usb_transfer_dummy_t) == sizeof(usb_transfer_t), "usb_transfer_dummy_t does not match usb_transfer_t");

struct urb_obj{
    TAILQ_ENTRY(urb_obj) tailq_entry;
    //HCD context pointer and variables. Must be initialized to NULL and 0 respectively
    void *hcd_ptr;
    uint32_t hcd_var;
    //Host Driver layer will add its fields here.
    //Public transfer structure. Must be last due to variable length array
    usb_transfer_t transfer;
};

typedef struct urb_obj urb_t;

#ifdef __cplusplus
}
#endif
