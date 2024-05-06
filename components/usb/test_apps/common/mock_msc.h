/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_assert.h"
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

    ------------------ Configuration Descriptor -------------------
bLength                  : 0x09 (9 bytes)
bDescriptorType          : 0x02 (Configuration Descriptor)
wTotalLength             : 0x0020 (32 bytes)
bNumInterfaces           : 0x01 (1 Interface)
bConfigurationValue      : 0x01 (Configuration 1)
iConfiguration           : 0x00 (No String Descriptor)
bmAttributes             : 0x80
 D7: Reserved, set 1     : 0x01
 D6: Self Powered        : 0x00 (no)
 D5: Remote Wakeup       : 0x00 (no)
 D4..0: Reserved, set 0  : 0x00
MaxPower                 : 0x70 (224 mA)
Data (HexDump)           : 09 02 20 00 01 01 00 80 70 09 04 00 00 02 08 06
                           50 00 07 05 81 02 00 02 00 07 05 02 02 00 02 00

        ---------------- Interface Descriptor -----------------
bLength                  : 0x09 (9 bytes)
bDescriptorType          : 0x04 (Interface Descriptor)
bInterfaceNumber         : 0x00
bAlternateSetting        : 0x00
bNumEndpoints            : 0x02 (2 Endpoints)
bInterfaceClass          : 0x08 (Mass Storage)
bInterfaceSubClass       : 0x06 (SCSI transparent command set)
bInterfaceProtocol       : 0x50 (Bulk-Only Transport)
iInterface               : 0x00 (No String Descriptor)
Data (HexDump)           : 09 04 00 00 02 08 06 50 00

        ----------------- Endpoint Descriptor -----------------
bLength                  : 0x07 (7 bytes)
bDescriptorType          : 0x05 (Endpoint Descriptor)
bEndpointAddress         : 0x81 (Direction=IN EndpointID=1)
bmAttributes             : 0x02 (TransferType=Bulk)
wMaxPacketSize           : 0x0040 (max 64 bytes)
bInterval                : 0x00 (never NAKs)
Data (HexDump)           : 07 05 81 02 40 00 00

        ----------------- Endpoint Descriptor -----------------
bLength                  : 0x07 (7 bytes)
bDescriptorType          : 0x05 (Endpoint Descriptor)
bEndpointAddress         : 0x02 (Direction=OUT EndpointID=2)
bmAttributes             : 0x02 (TransferType=Bulk)
wMaxPacketSize           : 0x0040 (max 64 bytes)
bInterval                : 0x00 (never NAKs)
Data (HexDump)           : 07 05 02 02 40 00 00

If you're using a flash driver with different endpoints, modify the endpoint descriptors below.
*/

//Constant descriptors
extern const usb_device_desc_t mock_msc_scsi_dev_desc;
extern uint8_t mock_msc_scsi_config_desc[255];
extern uint16_t mock_msc_scsi_str_desc_manu[128];
extern uint16_t mock_msc_scsi_str_desc_prod[128];
extern uint16_t mock_msc_scsi_str_desc_ser_num[128];
extern const usb_ep_desc_t mock_msc_scsi_bulk_out_ep_desc;
extern const usb_ep_desc_t mock_msc_scsi_bulk_in_ep_desc;

#define MOCK_MSC_SCSI_DEV_ID_VENDOR     0x0781 // Western Digital, Sandisk
#define MOCK_MSC_SCSI_DEV_ID_PRODUCT    0x5595
#define MOCK_MSC_SCSI_DEV_VERSION       0x0100 //1.00
#define MOCK_MSC_SCSI_USB_VERSION       0x0210 //2.10
#define MOCK_MSC_SCSI_DEV_DFLT_EP_MPS   64
#define MOCK_MSC_SCSI_SECTOR_SIZE       512
#define MOCK_MSC_SCSI_LUN               0
#define MOCK_MSC_SCSI_INTF_NUMBER       0
#define MOCK_MSC_SCSI_INTF_ALT_SETTING  0
#define MOCK_MSC_SCSI_BULK_OUT_EP_ADDR  0x02
#define MOCK_MSC_SCSI_BULK_IN_EP_ADDR   0x81
#define MOCK_MSC_SCSI_BULK_EP_MPS       64
#define MOCK_MSC_SCSI_STRING_1          (" USB")
#define MOCK_MSC_SCSI_STRING_2          (" SanDisk 3.2Gen1")
#define MOCK_MSC_SCSI_STRING_3          ("0101cdd1e856b427bbb796f870561a4b2b817af9da9872c8d75217cccdd5d5eccb3a0000000000000000000096abe1a3ff83610095558107aea948b4") // This string is NOT checked by the enum test

#define MOCK_MSC_SCSI_REQ_INIT_RESET(setup_pkt_ptr, intf_num) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIP_INTERFACE; \
    (setup_pkt_ptr)->bRequest = 0xFF;    \
    (setup_pkt_ptr)->wValue = 0; \
    (setup_pkt_ptr)->wIndex = (intf_num);    \
    (setup_pkt_ptr)->wLength = 0;    \
})

typedef struct __attribute__((packed))
{
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

typedef struct __attribute__((packed))
{
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
typedef struct __attribute__((packed))
{
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

/**
 * @brief Construct configuration and string descriptors
 */
void mock_msc_scsi_init_reference_descriptors(void);

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
