/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "usb/usb_types_ch9.h"
#include "dev_msc.h"

/*
Some tests where the ESP (acting as host) will require that a particular test
device acting as a MSC SCSI flash drive be connected. That test device's
information and descriptors are defined in this file.

If you are connecting a different MSC SCSI flash drive, please update
the descriptor and getter functions accordingly.

------------------------------ Device Descriptor -------------------------------
bLength                     : 0x12      (18 bytes)
bDescriptorType             : 0x01      (Device Descriptor)
bcdUSB                      : 0x0210    (2.10)
bDeviceClass                : 0x00
bDeviceSubClass             : 0x00
bDeviceProtocol             : 0x00
bMaxPacketSize0             : 0x40      (64 bytes)
idVendor                    : 0x0781    (SanDisk Corp)
idProduct                   : 0x5595
bcdDevice                   : 0x0100    (1.00)
iManufacturer               : 1
iProduct                    : 2
iSerial                     : 3
bNumConfigurations          : 1

--------------------------- Configuration Descriptor ---------------------------
bLength                     : 0x09      (9 bytes)
bDescriptorType             : 0x02      (Configuration Descriptor)
wTotalLength                : 0x0020    (32 bytes)
bNumInterfaces              : 0x01      (1 Interface)
bConfigurationValue         : 0x01      (Configuration 1)
iConfiguration              : 0x00      (No String Descriptor)
bmAttributes                : 0x80
 D7: Reserved, set 1            : 0x01
 D6: Self Powered               : 0x00  (no)
 D5: Remote Wakeup              : 0x00  (no)
 D4..0: Reserved, set 0         : 0x00
MaxPower                    : 0x70      (224 mA)

Data (HexDump)              : 09 02 20 00 01 01 00 80 70 09 04 00 00 02 08 06
                              50 00 07 05 81 02 00 02 00 07 05 02 02 00 02 00

----------------------------- Interface Descriptor -----------------------------
bLength                     : 0x09      (9 bytes)
bDescriptorType             : 0x04      (Interface Descriptor)
bInterfaceNumber            : 0x00
bAlternateSetting           : 0x00
bNumEndpoints               : 0x02      (2 Endpoints)
bInterfaceClass             : 0x08      (Mass Storage)
bInterfaceSubClass          : 0x06      (SCSI transparent command set)
bInterfaceProtocol          : 0x50      (Bulk-Only Transport)
iInterface                  : 0x00      (No String Descriptor)

------------------------------ Endpoint Descriptor -----------------------------
bLength                     : 0x07      (7 bytes)
bDescriptorType             : 0x05      (Endpoint Descriptor)
bEndpointAddress            : 0x81      (Direction=IN EndpointID=1)
bmAttributes                : 0x02      (TransferType=Bulk)
wMaxPacketSize              : 0x0040    (max 64 bytes for FS, 512 bytes for HS)
bInterval                   : 0x00      (never NAKs)

------------------------------ Endpoint Descriptor -----------------------------
bLength                     : 0x07      (7 bytes)
bDescriptorType             : 0x05      (Endpoint Descriptor)
bEndpointAddress            : 0x02      (Direction=OUT EndpointID=2)
bmAttributes                : 0x02      (TransferType=Bulk)
wMaxPacketSize              : 0x0040    (max 64 bytes for FS, 512 bytes for HS)
bInterval                   : 0x00      (never NAKs)

---------------------------- String Descriptor Manu ----------------------------
bLength                     : 0x0A      (10 bytes)
bDescriptorType             : 0x03      (String Descriptor)
wData                       : " USB"

---------------------------- String Descriptor Prod ----------------------------
bLength                     : 0x22      (34 bytes)
bDescriptorType             : 0x03      (String Descriptor)
wData                       : " SanDisk 3.2Gen1"

----------------------------- String Descriptor Ser ----------------------------
bLength                     : 0xF2      (242 bytes)
bDescriptorType             : 0x03      (String Descriptor)
wData                       : "0101cdd1e856b427bbb796f870561a4b2b817af9da9872c8d75217cccdd5d5eccb3a0000000000000000000096abe1a3ff83610095558107aea948b4"
*/

// --------------------------- Device Information ------------------------------

static const dev_msc_info_t dev_info = {
    .bInterfaceNumber = 0x00,
    .bAlternateSetting = 0x00,
    .in_ep_addr = 0x81,
    .out_up_addr = 0x02,
    .scsi_sector_size = 512,
};

// ------------------------------- Descriptors ---------------------------------

static const usb_device_desc_t dev_desc = {
    .bLength = USB_DEVICE_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0210,       // 2.10
    .bDeviceClass = USB_CLASS_PER_INTERFACE,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x0781,     // SanDisk Corp
    .idProduct = 0x5595,
    .bcdDevice = 0x0100,    // 1.00
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

static const usb_config_desc_t config_desc = {
    .bLength = USB_CONFIG_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_CONFIGURATION,
    .wTotalLength = 0x0020,     // 32 bytes
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x70,          // 224 mA
};

static const usb_intf_desc_t intf_desc = {
    .bLength = USB_INTF_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_MASS_STORAGE,
    .bInterfaceSubClass = 0x06, //SCSI
    .bInterfaceProtocol = 0x50, //Bulk only
    .iInterface = 0,
};

static const usb_ep_desc_t in_ep_desc_fs = {
    .bLength = USB_EP_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x81,       // EP 1 IN
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 64,
    .bInterval = 0,
};

static const usb_ep_desc_t in_ep_desc_hs = {
    .bLength = USB_EP_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x81,       // EP 1 IN
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 512,
    .bInterval = 0,
};

static const usb_ep_desc_t out_ep_desc_fs = {
    .bLength = USB_EP_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x02,       // EP 2 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 64,
    .bInterval = 0,
};

static const usb_ep_desc_t out_ep_desc_hs = {
    .bLength = USB_EP_DESC_SIZE,
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x02,       // EP 2 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 512,
    .bInterval = 0,
};

/*
String descriptors are dynamically initialized due to issues with static
initialization of variable length array members. See IDF-9886.
*/

static const usb_str_desc_t str_desc_manu_base = {
    .bLength = sizeof(usb_str_desc_t) + (4 * sizeof(uint16_t)),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_STRING,
};
static const uint16_t str_desc_manu_data[] = {
    0x0020,         // ' '
    0x0055,         // 'U'
    0x0053,         // 'S'
    0x0042,         // 'B'
};
static uint8_t *str_desc_manu[sizeof(str_desc_manu_base) + sizeof(str_desc_manu_data)];

static const usb_str_desc_t str_desc_prod_base = {
    .bLength = sizeof(usb_str_desc_t) + (16 * sizeof(uint16_t)),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_STRING,
};
static const uint16_t str_desc_prod_data[] = {
    0x0020,         // ' '
    0x0053,         // 'S'
    0x0061,         // 'a'
    0x006e,         // 'n'
    0x0044,         // 'D'
    0x0069,         // 'i'
    0x0073,         // 's'
    0x006b,         // 'k'
    0x0020,         // ' '
    0x0033,         // '3'
    0x002e,         // '.'
    0x0032,         // '2'
    0x0047,         // 'G'
    0x0065,         // 'e'
    0x006e,         // 'n'
    0x0031,         // '1'
};
static uint8_t *str_desc_prod[sizeof(str_desc_prod_base) + sizeof(str_desc_prod_data)];

static const usb_str_desc_t str_desc_ser_base = {
    .bLength = sizeof(usb_str_desc_t) + (120 * sizeof(uint16_t)),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_STRING,
};
static const uint16_t str_desc_ser_data[] = {
    /*
    The following string encoded in UTF-16LE

    "0101cdd1e856b427bbb796f870561a4b2b817af9da9872c8d75217cccdd5d5eccb3a0000000
    000000000000096abe1a3ff83610095558107aea948b4"
    */
    0x0030, 0x0031, 0x0030, 0x0031, 0x0063, 0x0064, 0x0064, 0x0031, 0x0065,
    0x0038, 0x0035, 0x0036, 0x0062, 0x0034, 0x0032, 0x0037, 0x0062, 0x0062,
    0x0062, 0x0037, 0x0039, 0x0036, 0x0066, 0x0038, 0x0037, 0x0030, 0x0035,
    0x0036, 0x0031, 0x0061, 0x0034, 0x0062, 0x0032, 0x0062, 0x0038, 0x0031,
    0x0037, 0x0061, 0x0066, 0x0039, 0x0064, 0x0061, 0x0039, 0x0038, 0x0037,
    0x0032, 0x0063, 0x0038, 0x0064, 0x0037, 0x0035, 0x0032, 0x0031, 0x0037,
    0x0063, 0x0063, 0x0063, 0x0064, 0x0064, 0x0035, 0x0064, 0x0035, 0x0065,
    0x0063, 0x0063, 0x0062, 0x0033, 0x0061, 0x0030, 0x0030, 0x0030, 0x0030,
    0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030,
    0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0039, 0x0036,
    0x0061, 0x0062, 0x0065, 0x0031, 0x0061, 0x0033, 0x0066, 0x0066, 0x0038,
    0x0033, 0x0036, 0x0031, 0x0030, 0x0030, 0x0039, 0x0035, 0x0035, 0x0035,
    0x0038, 0x0031, 0x0030, 0x0037, 0x0061, 0x0065, 0x0061, 0x0039, 0x0034,
    0x0038, 0x0062, 0x0034,
};
static uint8_t *str_desc_ser[sizeof(str_desc_ser_base) + sizeof(str_desc_ser_data)];

// -------------------------------- Functions ----------------------------------

void dev_msc_init(void)
{
    // Dynamically initialize string descriptors due to compiler limitations (see IDF-9886)
    uint8_t *ptr;

    // Initialize manufacturer string descriptor
    ptr = (uint8_t *)str_desc_manu;
    memcpy(ptr, &str_desc_manu_base, sizeof(str_desc_manu_base));
    ptr += sizeof(str_desc_manu_base);
    memcpy(ptr, &str_desc_manu_data, sizeof(str_desc_manu_data));

    // Initialize product string descriptor
    ptr = (uint8_t *)str_desc_prod;
    memcpy(ptr, &str_desc_prod_base, sizeof(str_desc_prod_base));
    ptr += sizeof(str_desc_prod_base);
    memcpy(ptr, &str_desc_prod_data, sizeof(str_desc_prod_data));

    // Initialize serial string descriptor
    ptr = (uint8_t *)str_desc_ser;
    memcpy(ptr, &str_desc_ser_base, sizeof(str_desc_ser_base));
    ptr += sizeof(str_desc_ser_base);
    memcpy(ptr, &str_desc_ser_data, sizeof(str_desc_ser_data));
}

const dev_msc_info_t *dev_msc_get_info(void)
{
    return &dev_info;
}

const usb_device_desc_t *dev_msc_get_dev_desc(usb_speed_t speed)
{
    return &dev_desc;
}

const usb_config_desc_t *dev_msc_get_config_desc(usb_speed_t speed)
{
    return &config_desc;
}

const usb_intf_desc_t *dev_msc_get_intf_desc(usb_speed_t speed)
{
    return &intf_desc;
}

const usb_ep_desc_t *dev_msc_get_in_ep_desc(usb_speed_t speed)
{
    const usb_ep_desc_t *ret;

    // EP descriptor differs by speed due to MPS
    switch (speed) {
    case USB_SPEED_FULL:
        ret = &in_ep_desc_fs;
        break;
    case USB_SPEED_HIGH:
        ret = &in_ep_desc_hs;
        break;
    default:
        ret = NULL;
        abort();    // Should never occur
        break;
    }

    return ret;
}

const usb_ep_desc_t *dev_msc_get_out_ep_desc(usb_speed_t speed)
{
    const usb_ep_desc_t *ret;

    // EP descriptor differs by speed due to MPS
    switch (speed) {
    case USB_SPEED_FULL:
        ret = &out_ep_desc_fs;
        break;
    case USB_SPEED_HIGH:
        ret = &out_ep_desc_hs;
        break;
    default:
        ret = NULL;
        abort();    // Should never occur
        break;
    }

    return ret;
}

const usb_str_desc_t *dev_msc_get_str_desc_manu(void)
{
    return (const usb_str_desc_t *)str_desc_manu;
}

const usb_str_desc_t *dev_msc_get_str_desc_prod(void)
{
    return (const usb_str_desc_t *)str_desc_prod;
}

const usb_str_desc_t *dev_msc_get_str_desc_ser(void)
{
    return (const usb_str_desc_t *)str_desc_ser;
}
