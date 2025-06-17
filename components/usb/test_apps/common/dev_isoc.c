/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usb/usb_types_ch9.h"
#include "usb/usb_types_stack.h"
#include "dev_isoc.h"

// ------------------------------- Descriptors ---------------------------------

static const usb_ep_desc_t isoc_out_ep_desc = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x02,   // EP 2 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_ISOC,
    .wMaxPacketSize = 128,
    .bInterval = 1,     // Isoc interval is (2 ^ (bInterval - 1)) which means an interval of 1ms
};

// -------------------------------- Functions ----------------------------------

const usb_ep_desc_t *dev_isoc_get_out_ep_desc(usb_speed_t speed)
{
    return &isoc_out_ep_desc;
}
