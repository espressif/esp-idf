/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "usb/usb_types_ch9.h"
#include "test_usb_mock_msc.h"

// ---------------------------------------------------- MSC SCSI -------------------------------------------------------

const char *MSC_CLIENT_TAG = "MSC Client";

const usb_device_desc_t mock_msc_scsi_dev_desc = {
    .bLength = USB_DEVICE_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = MOCK_MSC_SCSI_USB_VERSION,
    .bDeviceClass = USB_CLASS_PER_INTERFACE,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = MOCK_MSC_SCSI_DEV_DFLT_EP_MPS,
    .idVendor = MOCK_MSC_SCSI_DEV_ID_VENDOR,
    .idProduct = MOCK_MSC_SCSI_DEV_ID_PRODUCT,
    .bcdDevice = MOCK_MSC_SCSI_DEV_VERSION,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

#define MOCK_MSC_SCSI_WTOTALLENGTH (USB_CONFIG_DESC_SIZE + USB_INTF_DESC_SIZE + 2*USB_EP_DESC_SIZE)
static const usb_config_desc_t mock_msc_config_desc = {
    .bLength = USB_CONFIG_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_CONFIGURATION,
    .wTotalLength = MOCK_MSC_SCSI_WTOTALLENGTH,
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x70, //224mA
};

static const usb_intf_desc_t mock_msc_intf_desc = {
    .bLength = USB_INTF_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = MOCK_MSC_SCSI_INTF_NUMBER,
    .bAlternateSetting = MOCK_MSC_SCSI_INTF_ALT_SETTING,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_MASS_STORAGE,
    .bInterfaceSubClass = 0x06, //SCSI
    .bInterfaceProtocol = 0x50, //Bulk only
    .iInterface = 0,
};

uint8_t mock_msc_scsi_config_desc[255];
uint16_t mock_msc_scsi_str_desc_manu[128];
uint16_t mock_msc_scsi_str_desc_prod[128];
uint16_t mock_msc_scsi_str_desc_ser_num[128];
usb_ep_desc_t mock_msc_scsi_bulk_out_ep_desc;
usb_ep_desc_t mock_msc_scsi_bulk_in_ep_desc;

const usb_ep_desc_t mock_msc_scsi_bulk_out_ep_desc_fs = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_MSC_SCSI_BULK_OUT_EP_ADDR,       //EP 1 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = MOCK_MSC_SCSI_BULK_EP_MPS_FS,           //MPS of 64 bytes
    .bInterval = 0,
};

const usb_ep_desc_t mock_msc_scsi_bulk_out_ep_desc_hs = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_MSC_SCSI_BULK_OUT_EP_ADDR,       //EP 1 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = MOCK_MSC_SCSI_BULK_EP_MPS_HS,           //MPS of 512 bytes
    .bInterval = 0,
};

const usb_ep_desc_t mock_msc_scsi_bulk_in_ep_desc_fs = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_MSC_SCSI_BULK_IN_EP_ADDR,
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = MOCK_MSC_SCSI_BULK_EP_MPS_FS,           //MPS of 64 bytes
    .bInterval = 0,
};

const usb_ep_desc_t mock_msc_scsi_bulk_in_ep_desc_hs = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_MSC_SCSI_BULK_IN_EP_ADDR,
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = MOCK_MSC_SCSI_BULK_EP_MPS_HS,           //MPS of 512 bytes
    .bInterval = 0,
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

void mock_msc_scsi_init_reference_descriptors(void)
{
    // Configuration descriptor
    uint8_t *dest_ptr = mock_msc_scsi_config_desc;
    memcpy(dest_ptr, (void*)&mock_msc_config_desc, sizeof(mock_msc_config_desc));
    dest_ptr += USB_CONFIG_DESC_SIZE;
    memcpy(dest_ptr, (void*)&mock_msc_intf_desc, sizeof(mock_msc_intf_desc));
    dest_ptr += USB_INTF_DESC_SIZE;
    // Set endpoint descriptors with zeroes, FS or HS device has not been connected
    memset(dest_ptr, 0, sizeof(usb_ep_desc_t));
    dest_ptr += USB_EP_DESC_SIZE;
    memset(dest_ptr, 0, sizeof(usb_ep_desc_t));

    // String descriptors
    const char *str = MOCK_MSC_SCSI_STRING_1;
    uint8_t chr_count = strlen(str);
    mock_msc_scsi_str_desc_manu[0] = (USB_B_DESCRIPTOR_TYPE_STRING << 8) | (2 * chr_count + 2);  // first byte is length (including header), second byte is string type
    for (uint8_t i = 0; i < chr_count; i++) {
        mock_msc_scsi_str_desc_manu[1 + i] = str[i];
    }

    str = MOCK_MSC_SCSI_STRING_2;
    chr_count = strlen(str);
    mock_msc_scsi_str_desc_prod[0] = (USB_B_DESCRIPTOR_TYPE_STRING << 8) | (2 * chr_count + 2);  // first byte is length (including header), second byte is string type
    for (uint8_t i = 0; i < chr_count; i++) {
        mock_msc_scsi_str_desc_prod[1 + i] = str[i];
    }

    str = MOCK_MSC_SCSI_STRING_3;
    chr_count = strlen(str);
    mock_msc_scsi_str_desc_ser_num[0] = (USB_B_DESCRIPTOR_TYPE_STRING << 8) | (2 * chr_count + 2);  // first byte is length (including header), second byte is string type
    for (uint8_t i = 0; i < chr_count; i++) {
        mock_msc_scsi_str_desc_ser_num[1 + i] = str[i];
    }
}
