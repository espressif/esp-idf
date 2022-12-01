/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "usb/usb_types_ch9.h"
#include "test_usb_mock_hid.h"

// ---------------------------------------------------- HID Mouse ------------------------------------------------------

const usb_ep_desc_t mock_hid_mouse_in_ep_desc = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_HID_MOUSE_INTR_IN_EP_ADDR,       //EP 1 IN
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_INT,
    .wMaxPacketSize = MOCK_HID_MOUSE_INTR_IN_MPS,
    .bInterval = 10,                //Interval of 10ms
};

void mock_hid_process_report(mock_hid_mouse_report_t *report, int iter)
{
    static int x_pos = 0;
    static int y_pos = 0;
    //Update X position
    if (report->x_movement & 0x80) {    //Positive movement
        x_pos += report->x_movement & 0x7F;
    } else {    //Negative movement
        x_pos -= report->x_movement & 0x7F;
    }
    //Update Y position
    if (report->y_movement & 0x80) {    //Positive movement
        y_pos += report->y_movement & 0x7F;
    } else {    //Negative movement
        y_pos -= report->y_movement & 0x7F;
    }
    printf("\rX:%d\tY:%d\tIter: %d\n", x_pos, y_pos, iter);
}
