/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
This header contains bare-bone mock implementations of some device classes in order to test various layers of the USB
Host stack.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "usb/usb_types_ch9.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------- MSC SCSI -------------------------------------------------------

extern const char *MSC_CLIENT_TAG;

/*
Note: The mock MSC SCSI tests requires that USB flash drive be connected. The flash drive should...

- Be implement the Mass Storage class supporting BULK only transfers using SCSI commands
- It's configuration 1 should have the following endpoints

Device Descriptor:
    bLength                18
    bDescriptorType         1
    bcdUSB               2.00
    bDeviceClass            0
    bDeviceSubClass         0
    bDeviceProtocol         0
    bMaxPacketSize0        64
    idVendor           0x125f
    idProduct          0xc08a
    bcdDevice            1.00
    iManufacturer           1
    iProduct                2
    iSerial                 3
    bNumConfigurations      1
Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x0020
    bNumInterfaces          1
    bConfigurationValue     1
    iConfiguration          0
    bmAttributes         0x80
        (Bus Powered)
    MaxPower              480mA
    Interface Descriptor:
        bLength                 9
        bDescriptorType         4
        bInterfaceNumber        0
        bAlternateSetting       0
        bNumEndpoints           2
        bInterfaceClass         8 Mass Storage
        bInterfaceSubClass      6 SCSI
        bInterfaceProtocol     80 Bulk-Only
        iInterface              0
        Endpoint Descriptor:
            bLength             7
            bDescriptorType     5
            bEndpointAddress    0x01  EP 1 OUT
            bmAttributes        2
                Transfer Type   Bulk
                Synch Type      None
                Usage Type      Data
            wMaxPacketSize      0x0040  1x 64 bytes
            bInterval           1
        Endpoint Descriptor:
            bLength             7
            bDescriptorType     5
            bEndpointAddress    0x82  EP 2 IN
            bmAttributes        2
                Transfer Type   Bulk
                Synch Type      None
                Usage Type      Data
            wMaxPacketSize      0x0040  1x 64 bytes
            bInterval           1

If you're using a flash driver with different endpoints, modify the endpoint descriptors below.
*/

//Constant descriptors
extern const uint8_t mock_msc_scsi_dev_desc[];
extern const uint8_t mock_msc_scsi_config_desc[];
extern const uint8_t mock_msc_scsi_str_desc_manu[];
extern const uint8_t mock_msc_scsi_str_desc_prod[];
extern const uint8_t mock_msc_scsi_str_desc_ser_num[];
extern const usb_ep_desc_t mock_msc_scsi_bulk_out_ep_desc;
extern const usb_ep_desc_t mock_msc_scsi_bulk_in_ep_desc;

#define MOCK_MSC_SCSI_DEV_ID_VENDOR     0x125F
#define MOCK_MSC_SCSI_DEV_ID_PRODUCT    0xc08A
#define MOCK_MSC_SCSI_DEV_DFLT_EP_MPS   64
#define MOCK_MSC_SCSI_SECTOR_SIZE       512
#define MOCK_MSC_SCSI_LUN               0
#define MOCK_MSC_SCSI_INTF_NUMBER       0
#define MOCK_MSC_SCSI_INTF_ALT_SETTING  0
#define MOCK_MSC_SCSI_BULK_OUT_EP_ADDR  0x01
#define MOCK_MSC_SCSI_BULK_IN_EP_ADDR   0x82
#define MOCK_MSC_SCSI_BULK_EP_MPS       64

#define MOCK_MSC_SCSI_REQ_INIT_RESET(setup_pkt_ptr, intf_num) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIP_INTERFACE; \
    (setup_pkt_ptr)->bRequest = 0xFF;    \
    (setup_pkt_ptr)->wValue = 0; \
    (setup_pkt_ptr)->wIndex = (intf_num);    \
    (setup_pkt_ptr)->wLength = 0;    \
})

typedef struct __attribute__((packed)) {
    uint8_t opcode; //0x28 = read(10), 0x2A=write(10)
    uint8_t flags;
    uint8_t lba_3;
    uint8_t lba_2;
    uint8_t lba_1;
    uint8_t lba_0;
    uint8_t group;
    uint8_t len_1;
    uint8_t len_0;
    uint8_t control;
} mock_scsi_cmd10_t;

typedef struct __attribute__((packed)) {
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    mock_scsi_cmd10_t CBWCB;
    uint8_t padding[6];
} mock_msc_bulk_cbw_t;

// USB Bulk Transfer Command Status Wrapper data
typedef struct __attribute__((packed)) {
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
} mock_msc_bulk_csw_t;

/**
 * @brief Initialize a MSC Command Block Wrapper (CBW) as an SCSI command
 *
 * @param cbw CBW structure
 * @param is_read Is a read command
 * @param offset Block offset
 * @param num_sectors Number of sectors to read
 * @param tag Tag (this is simply echoed back
 */
void mock_msc_scsi_init_cbw(mock_msc_bulk_cbw_t *cbw, bool is_read, int offset, int num_sectors, uint32_t tag);

/**
 * @brief Check that returned Command Status Wrapper (CSW) is valid
 *
 * @param csw CSW structure
 * @param tag_expect Expected tag
 * @return true CSW is valid
 * @return false CSW is not valid
 */
bool mock_msc_scsi_check_csw(mock_msc_bulk_csw_t *csw, uint32_t tag_expect);

// ---------------------------------------------------- HID Mouse ------------------------------------------------------


/*
Note: The mock HID mouse tests require that USB low speed mouse be connected. The mouse should...

- Be implement the HID with standard report format used by mice
- It's configuration 1 should have the following endpoint

Device Descriptor:
    bLength                18
    bDescriptorType         1
    bcdUSB               2.00
    bDeviceClass            0
    bDeviceSubClass         0
    bDeviceProtocol         0
    bMaxPacketSize0         8
    idVendor           0x413c Dell Computer Corp.
    idProduct          0x301a
    bcdDevice            1.00
    iManufacturer           1
    iProduct                2
    iSerial                 0
    bNumConfigurations      1
    Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength       0x0022
        bNumInterfaces          1
        bConfigurationValue     1
        iConfiguration          0
        bmAttributes         0xa0
        (Bus Powered)
        Remote Wakeup
        MaxPower              100mA
        Interface Descriptor:
            bLength                 9
            bDescriptorType         4
            bInterfaceNumber        0
            bAlternateSetting       0
            bNumEndpoints           1
            bInterfaceClass         3 Human Interface Device
            bInterfaceSubClass      1 Boot Interface Subclass
            bInterfaceProtocol      2 Mouse
            iInterface              0
                HID Device Descriptor:
                bLength                 9
                bDescriptorType        33
                bcdHID               1.11
                bCountryCode            0 Not supported
                bNumDescriptors         1
                bDescriptorType        34 Report
                wDescriptorLength      46
                Report Descriptors:
                ** UNAVAILABLE **
            Endpoint Descriptor:
                bLength                 7
                bDescriptorType         5
                bEndpointAddress     0x81  EP 1 IN
                bmAttributes            3
                Transfer Type            Interrupt
                Synch Type               None
                Usage Type               Data
                wMaxPacketSize     0x0004  1x 4 bytes
                bInterval              10

If you're using another mice with different endpoints, modify the endpoint descriptor below
*/

extern  const usb_ep_desc_t mock_hid_mouse_in_ep_desc;

#define MOCK_HID_MOUSE_DEV_ID_VENDOR        0x413C
#define MOCK_HID_MOUSE_DEV_ID_PRODUCT       0x301A
#define MOCK_HID_MOUSE_DEV_DFLT_EP_MPS      8
#define MOCK_HID_MOUSE_INTF_NUMBER          0
#define MOCK_HID_MOUSE_INTF_ALT_SETTING     0
#define MOCK_HID_MOUSE_INTR_IN_EP_ADDR      0x81
#define MOCK_HID_MOUSE_INTR_IN_MPS          0x04

typedef union {
    struct {
        uint32_t left_button: 1;
        uint32_t right_button: 1;
        uint32_t middle_button: 1;
        uint32_t reserved5: 5;
        uint8_t x_movement;
        uint8_t y_movement;
    } __attribute__((packed));
    uint8_t val[3];
} mock_hid_mouse_report_t;
_Static_assert(sizeof(mock_hid_mouse_report_t) == 3, "Size of HID mouse report incorrect");

void mock_hid_process_report(mock_hid_mouse_report_t *report, int iter);

// ---------------------------------------------------- Mock ISOC ------------------------------------------------------

/*
Note: ISOC test rely on communicating with a non existent endpoint using ISOC OUT transfers. Since no ACK is given for
ISOC, transferring to a non-existent endpoint should work. The non-existent endpoint descriptor is described below:
*/

#define MOCK_ISOC_EP_NUM        2
#define MOCK_ISOC_EP_MPS        512


static const usb_ep_desc_t mock_isoc_out_ep_desc = {
    .bLength = sizeof(usb_ep_desc_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_ISOC_EP_NUM,
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_ISOC,
    .wMaxPacketSize = MOCK_ISOC_EP_MPS,     //MPS of 512 bytes
    .bInterval = 1,     //Isoc interval is (2 ^ (bInterval - 1)) which means an interval of 1ms
};


#ifdef __cplusplus
}
#endif
