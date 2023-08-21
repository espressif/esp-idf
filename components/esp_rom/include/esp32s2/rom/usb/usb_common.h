/*
 * SPDX-FileCopyrightText: 2015,2016 Intel Corporation
 * SPDX-FileContributor: 2017 PHYTEC Messtechnik GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief useful constants and macros for the USB application
 *
 * This file contains useful constants and macros for the USB applications.
 */

#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BCD(x) ((((x) / 10) << 4) | ((x) / 10))

/* Descriptor size in bytes */
#define USB_DEVICE_DESC_SIZE        18
#define USB_CONFIGURATION_DESC_SIZE 9
#define USB_INTERFACE_DESC_SIZE     9
#define USB_ENDPOINT_DESC_SIZE      7
#define USB_STRING_DESC_SIZE        4
#define USB_HID_DESC_SIZE       9
#define USB_DFU_DESC_SIZE       9
#define USB_DEVICE_QUAL_DESC_SIZE   10
#define USB_INTERFACE_ASSOC_DESC_SIZE   8

/* Descriptor type */
#define USB_DEVICE_DESC         0x01
#define USB_CONFIGURATION_DESC      0x02
#define USB_STRING_DESC         0x03
#define USB_INTERFACE_DESC      0x04
#define USB_ENDPOINT_DESC       0x05
#define USB_DEVICE_QUAL_DESC        0x06
#define USB_INTERFACE_ASSOC_DESC    0x0B
#define USB_DEVICE_CAPABILITY_DESC  0x10
#define USB_HID_DESC            0x21
#define USB_HID_REPORT_DESC     0x22
#define USB_DFU_FUNCTIONAL_DESC     0x21
#define USB_ASSOCIATION_DESC        0x0B
#define USB_BINARY_OBJECT_STORE_DESC    0x0F

/* Useful define */
#define USB_1_1             0x0110
#define USB_2_0             0x0200
/* Set USB version to 2.1 so that the host will request the BOS descriptor */
#define USB_2_1             0x0210

#define BCDDEVICE_RELNUM        (BCD(KERNEL_VERSION_MAJOR) << 8 | \
                    BCD(KERNEL_VERSION_MINOR))

/* 100mA max power, per 2mA units */
/* USB 1.1 spec indicates 100mA(max) per unit load, up to 5 loads */
#define MAX_LOW_POWER           0x32
#define MAX_HIGH_POWER          0xFA

/* bmAttributes:
 * D7:Reserved, always 1,
 * D6:Self-Powered -> 1,
 * D5:Remote Wakeup -> 0,
 * D4...0:Reserved -> 0
 */
#define USB_CONFIGURATION_ATTRIBUTES    0xC0

/* Classes */
#define COMMUNICATION_DEVICE_CLASS  0x02
#define COMMUNICATION_DEVICE_CLASS_DATA 0x0A
#define HID_CLASS           0x03
#define MASS_STORAGE_CLASS      0x08
#define WIRELESS_DEVICE_CLASS       0xE0
#define MISC_CLASS          0xEF
#define CUSTOM_CLASS            0xFF
#define DFU_DEVICE_CLASS        0xFE

/* Sub-classes */
#define CDC_NCM_SUBCLASS        0x0d
#define BOOT_INTERFACE_SUBCLASS     0x01
#define SCSI_TRANSPARENT_SUBCLASS   0x06
#define DFU_INTERFACE_SUBCLASS      0x01
#define RF_SUBCLASS         0x01
#define CUSTOM_SUBCLASS         0xFF
#define COMMON_SUBCLASS         0x02
/* Misc subclasses */
#define MISC_RNDIS_SUBCLASS     0x04
#define CDC_ABSTRACT_CONTROL_MODEL 0x02

/* Protocols */
#define V25TER_PROTOCOL         0x01
#define MOUSE_PROTOCOL          0x02
#define BULK_ONLY_PROTOCOL      0x50
#define DFU_RUNTIME_PROTOCOL        0x01
#define DFU_MODE_PROTOCOL       0x02
#define BLUETOOTH_PROTOCOL      0x01
/* CDC ACM protocols */
#define ACM_VENDOR_PROTOCOL     0xFF
/* Misc protocols */
#define MISC_ETHERNET_PROTOCOL      0x01
#define IAD_PROTOCOL            0x01

/** Standard Device Descriptor */
struct usb_device_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} __packed;

/** Unicode (UTF16LE) String Descriptor */
struct usb_string_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bString;
} __packed;

/** Association Descriptor */
struct usb_association_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} __packed;

/** Standard Configuration Descriptor */
struct usb_cfg_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} __packed;

/** Standard Interface Descriptor */
struct usb_if_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} __packed;

/** Standard Endpoint Descriptor */
struct usb_ep_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} __packed;

struct string_descriptor_zero {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wBcdLang[];
} __packed;

struct string_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bString[];
} __packed;

#define ROM_MAX_CFG_DESC_CNT 1

struct rom_usb_descriptors {
    const struct usb_device_descriptor *device_descr;
    const void *config_descr[ROM_MAX_CFG_DESC_CNT];
    int string_count; // including string_descriptor_zero
    const struct string_descriptor_zero *string0_descr;
    const struct string_descriptor *string_descrs[];
};

/* Descriptors defined in the ROM */
extern struct usb_device_descriptor general_device_descr;
extern const void* acm_config_descr;
extern const void* dfu_config_descr;
extern const struct string_descriptor str_manu_descr;
extern const struct string_descriptor str_prod_descr;
extern const struct string_descriptor_zero string0_descr;
extern const struct rom_usb_descriptors acm_usb_descriptors;
extern const struct rom_usb_descriptors dfu_usb_descriptors;
extern const struct rom_usb_descriptors *rom_usb_curr_desc;

/* ROM patch: set the ACM descriptor with the correct serial number.
 * Only needed on ESP32-S2, on later chips the ROM descriptor is correct.
 */
void rom_usb_cdc_set_descriptor_patch(void);


#ifdef __cplusplus
}
#endif
