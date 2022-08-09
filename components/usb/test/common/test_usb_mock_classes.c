/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include "usb/usb_types_ch9.h"
#include "test_usb_mock_classes.h"

// ---------------------------------------------------- MSC SCSI -------------------------------------------------------

const char *MSC_CLIENT_TAG = "MSC Client";

const uint8_t mock_msc_scsi_dev_desc[] = {
    0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x5F, 0x12, 0x8A, 0xC0, 0x00, 0x01, 0x01, 0x02, 0x03, 0x01,
};

const uint8_t mock_msc_scsi_config_desc[] = {
    0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 0xF0, 0x09, 0x04, 0x00, 0x00, 0x02, 0x08, 0x06, 0x50, 0x00, 0x07,
    0x05, 0x01, 0x02, 0x40, 0x00, 0x01, 0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x01,
};


const uint8_t mock_msc_scsi_str_desc_manu[] = {
    0x0c, 0x03, 0x41, 0x00, 0x44, 0x00, 0x41, 0x00, 0x54, 0x00, 0x41, 0x00,
};

const uint8_t mock_msc_scsi_str_desc_prod[] = {
    0x2c, 0x03, 0x41, 0x00, 0x44, 0x00, 0x41, 0x00, 0x54, 0x00, 0x41, 0x00, 0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42,
    0x00, 0x20, 0x00, 0x46, 0x00, 0x6c, 0x00, 0x61, 0x00, 0x73, 0x00, 0x68, 0x00, 0x20, 0x00, 0x44, 0x00, 0x72, 0x00,
    0x69, 0x00, 0x76, 0x00, 0x65, 0x00,
};

const uint8_t mock_msc_scsi_str_desc_ser_num[] = {
    0x22, 0x03, 0x31, 0x00, 0x33, 0x00, 0x43, 0x00, 0x32, 0x00, 0x38, 0x00, 0x31, 0x00, 0x36, 0x00, 0x35, 0x00, 0x38,
    0x00, 0x32, 0x00, 0x31, 0x00, 0x38, 0x00, 0x30, 0x00, 0x30, 0x00, 0x38, 0x00, 0x45, 0x00,
};

const usb_ep_desc_t mock_msc_scsi_bulk_out_ep_desc = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x01,       //EP 1 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 64,           //MPS of 64 bytes
    .bInterval = 1,
};

const usb_ep_desc_t mock_msc_scsi_bulk_in_ep_desc = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x82,       //EP 2 IN
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 64,           //MPS of 64 bytes
    .bInterval = 1,
};

void mock_msc_scsi_init_cbw(mock_msc_bulk_cbw_t *cbw, bool is_read, int offset, int num_sectors, uint32_t tag)
{
    cbw->dCBWSignature = 0x43425355;    //Fixed value
    cbw->dCBWTag = tag; //Random value that is echoed back
    cbw->dCBWDataTransferLength = num_sectors * MOCK_MSC_SCSI_SECTOR_SIZE;
    cbw->bmCBWFlags = (is_read) ? (1 << 7) : 0; //If this is a read, set the direction flag
    cbw->bCBWLUN = MOCK_MSC_SCSI_LUN;
    cbw->bCBWCBLength = 10;     //The length of the SCSI command
    //Initialize SCSI CMD as READ10 or WRITE 10
    cbw->CBWCB.opcode = (is_read) ? 0x28 : 0x2A;  //SCSI CMD READ10 or WRITE10
    cbw->CBWCB.flags = 0;
    cbw->CBWCB.lba_3 = (offset >> 24);
    cbw->CBWCB.lba_2 = (offset >> 16);
    cbw->CBWCB.lba_1 = (offset >> 8);
    cbw->CBWCB.lba_0 = (offset >> 0);
    cbw->CBWCB.group = 0;
    cbw->CBWCB.len_1 = (num_sectors >> 8);
    cbw->CBWCB.len_0 = (num_sectors >> 0);
    cbw->CBWCB.control = 0;
}

bool mock_msc_scsi_check_csw(mock_msc_bulk_csw_t *csw, uint32_t tag_expect)
{
    bool no_issues = true;
    if (csw->dCSWSignature != 0x53425355) {
        no_issues = false;
        printf("Warning: csw signature corrupt (0x%"PRIX32")\n", csw->dCSWSignature);
    }
    if (csw->dCSWTag != tag_expect) {
        no_issues = false;
        printf("Warning: csw tag unexpected! Expected %"PRIu32" got %"PRIu32"\n", tag_expect, csw->dCSWTag);
    }
    if (csw->dCSWDataResidue) {
        no_issues = false;
        printf("Warning: csw indicates data residue of %"PRIu32" bytes!\n", csw->dCSWDataResidue);
    }
    if (csw->bCSWStatus) {
        no_issues = false;
        printf("Warning: csw indicates non-good status %d!\n", csw->bCSWStatus);
    }
    return no_issues;
}


// ---------------------------------------------------- HID Mouse ------------------------------------------------------

const usb_ep_desc_t mock_hid_mouse_in_ep_desc = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x81,       //EP 1 IN
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_INT,
    .wMaxPacketSize = 4,            //MPS of 4 bytes
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
