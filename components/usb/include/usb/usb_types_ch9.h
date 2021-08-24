/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Note: This header file contains the types and macros belong/relate to the USB2.0 protocol and are HW implementation
and mode (i.e., Host or Device) agnostic.

- On the USB Host Stack, this header is only meant to be used on the HCD layer and above. For types and macros on the
  Host stack that are HW implementation specific (i.e., HAL layer and below), add them to the "hal/usb_types_private.h"
  header instead.
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define USB_DESC_ATTR                   __attribute__((packed))

// ---------------------------------------------------- Chapter 9 ------------------------------------------------------

/**
 * @brief USB2.0 device states
 */
typedef enum {
    USB_DEVICE_STATE_NOT_ATTACHED,  //Not in the USB spec, but is a catch all state for devices that need to be cleaned up after a sudden disconnection or port error
    USB_DEVICE_STATE_ATTACHED,
    USB_DEVICE_STATE_POWERED,
    USB_DEVICE_STATE_DEFAULT,
    USB_DEVICE_STATE_ADDRESS,
    USB_DEVICE_STATE_CONFIGURED,
    USB_DEVICE_STATE_SUSPENDED,
} usb_device_state_t;

/**
 * @brief Descriptor types from USB2.0 specification table 9.5
 */
#define USB_B_DESCRIPTOR_TYPE_DEVICE                        0x01
#define USB_B_DESCRIPTOR_TYPE_CONFIGURATION                 0x02
#define USB_B_DESCRIPTOR_TYPE_STRING                        0x03
#define USB_B_DESCRIPTOR_TYPE_INTERFACE                     0x04
#define USB_B_DESCRIPTOR_TYPE_ENDPOINT                      0x05
#define USB_B_DESCRIPTOR_TYPE_DEVICE_QUALIFIER              0x06
#define USB_B_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION     0x07
#define USB_B_DESCRIPTOR_TYPE_INTERFACE_POWER               0x08

/**
 * @brief Descriptor types from USB 2.0 ECN
 */
#define USB_B_DESCRIPTOR_TYPE_OTG                           0x09
#define USB_B_DESCRIPTOR_TYPE_DEBUG                         0x0a
#define USB_B_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION         0x0b

/**
 * @brief Descriptor types from Wireless USB spec
 */
#define USB_B_DESCRIPTOR_TYPE_SECURITY                      0x0c
#define USB_B_DESCRIPTOR_TYPE_KEY                           0x0d
#define USB_B_DESCRIPTOR_TYPE_ENCRYPTION_TYPE               0x0e
#define USB_B_DESCRIPTOR_TYPE_BOS                           0x0f
#define USB_B_DESCRIPTOR_TYPE_DEVICE_CAPABILITY             0x10
#define USB_B_DESCRIPTOR_TYPE_WIRELESS_ENDPOINT_COMP        0x11
#define USB_B_DESCRIPTOR_TYPE_WIRE_ADAPTER                  0x21
#define USB_B_DESCRIPTOR_TYPE_RPIPE                         0x22
#define USB_B_DESCRIPTOR_TYPE_CS_RADIO_CONTROL              0x23

/**
 * @brief Descriptor types from UAS specification
 */
#define USB_B_DESCRIPTOR_TYPE_PIPE_USAGE                    0x24

// -------------------- Setup Packet -----------------------

/**
 * @brief Size of a USB control transfer setup packet in bytes
 */
#define USB_SETUP_PACKET_SIZE               8

/**
 * @brief Structure representing a USB control transfer setup packet
 */
typedef union {
    struct {
        uint8_t bmRequestType;
        uint8_t bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    } __attribute__((packed));
    uint8_t val[USB_SETUP_PACKET_SIZE];
} usb_setup_packet_t;
_Static_assert(sizeof(usb_setup_packet_t) == USB_SETUP_PACKET_SIZE, "Size of usb_setup_packet_t incorrect");

/**
 * @brief Bit masks belonging to the bmRequestType field of a setup packet
 */
#define USB_BM_REQUEST_TYPE_DIR_OUT         (0X00 << 7)
#define USB_BM_REQUEST_TYPE_DIR_IN          (0x01 << 7)
#define USB_BM_REQUEST_TYPE_TYPE_STANDARD   (0x00 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_CLASS      (0x01 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_VENDOR     (0x02 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_RESERVED   (0x03 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_MASK       (0x03 << 5)
#define USB_BM_REQUEST_TYPE_RECIP_DEVICE    (0x00 << 0)
#define USB_BM_REQUEST_TYPE_RECIP_INTERFACE (0x01 << 0)
#define USB_BM_REQUEST_TYPE_RECIP_ENDPOINT  (0x02 << 0)
#define USB_BM_REQUEST_TYPE_RECIP_OTHER     (0x03 << 0)
#define USB_BM_REQUEST_TYPE_RECIP_MASK      (0x1f << 0)

/**
 * @brief Bit masks belonging to the bRequest field of a setup packet
 */
#define USB_B_REQUEST_GET_STATUS            0x00
#define USB_B_REQUEST_CLEAR_FEATURE         0x01
#define USB_B_REQUEST_SET_FEATURE           0x03
#define USB_B_REQUEST_SET_ADDRESS           0x05
#define USB_B_REQUEST_GET_DESCRIPTOR        0x06
#define USB_B_REQUEST_SET_DESCRIPTOR        0x07
#define USB_B_REQUEST_GET_CONFIGURATION     0x08
#define USB_B_REQUEST_SET_CONFIGURATION     0x09
#define USB_B_REQUEST_GET_INTERFACE         0x0A
#define USB_B_REQUEST_SET_INTERFACE         0x0B
#define USB_B_REQUEST_SYNCH_FRAME           0x0C

/**
 * @brief Bit masks belonging to the wValue field of a setup packet
 */
#define USB_W_VALUE_DT_DEVICE               0x01
#define USB_W_VALUE_DT_CONFIG               0x02
#define USB_W_VALUE_DT_STRING               0x03
#define USB_W_VALUE_DT_INTERFACE            0x04
#define USB_W_VALUE_DT_ENDPOINT             0x05
#define USB_W_VALUE_DT_DEVICE_QUALIFIER     0x06
#define USB_W_VALUE_DT_OTHER_SPEED_CONFIG   0x07
#define USB_W_VALUE_DT_INTERFACE_POWER      0x08

/**
 * @brief Initializer for a SET_ADDRESS request
 *
 * Sets the address of a connected device
 */
#define USB_SETUP_PACKET_INIT_SET_ADDR(setup_pkt_ptr, addr) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_TYPE_STANDARD |USB_BM_REQUEST_TYPE_RECIP_DEVICE;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_SET_ADDRESS;  \
    (setup_pkt_ptr)->wValue = (addr);   \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = 0;   \
})

/**
 * @brief Initializer for a request to get a device's device descriptor
 */
#define USB_SETUP_PACKET_INIT_GET_DEVICE_DESC(setup_pkt_ptr) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIP_DEVICE;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_GET_DESCRIPTOR;   \
    (setup_pkt_ptr)->wValue = (USB_W_VALUE_DT_DEVICE << 8); \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = 18;  \
})

/**
 * @brief Initializer for a request to get a device's current configuration number
 */
#define USB_SETUP_PACKET_INIT_GET_CONFIG(setup_pkt_ptr) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIP_DEVICE;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_GET_CONFIGURATION;    \
    (setup_pkt_ptr)->wValue = 0;    \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = 1;   \
})

/**
 * @brief Initializer for a request to get one of the device's current configuration descriptor
 *
 * - desc_index indicates the configuration's index number
 * - Number of bytes of the configuration descriptor to get
 */
#define USB_SETUP_PACKET_INIT_GET_CONFIG_DESC(setup_pkt_ptr, desc_index, desc_len) ({    \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIP_DEVICE;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_GET_DESCRIPTOR;   \
    (setup_pkt_ptr)->wValue = (USB_W_VALUE_DT_CONFIG << 8) | ((desc_index) & 0xFF); \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = (desc_len);  \
})

/**
 * @brief Initializer for a request to set a device's current configuration number
 */
#define USB_SETUP_PACKET_INIT_SET_CONFIG(setup_pkt_ptr, config_num) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIP_DEVICE;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_SET_CONFIGURATION;    \
    (setup_pkt_ptr)->wValue = (config_num); \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = 0;   \
})

/**
 * @brief Initializer for a request to set an interface's alternate setting
 */
#define USB_SETUP_PACKET_INIT_SET_INTERFACE(setup_pkt_ptr, intf_num, alt_setting_num) ({    \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIP_INTERFACE;  \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_SET_INTERFACE; \
    (setup_pkt_ptr)->wValue = (alt_setting_num); \
    (setup_pkt_ptr)->wIndex = (intf_num);  \
    (setup_pkt_ptr)->wLength = 0;   \
})

// ---------------- Standard Descriptor --------------------

/**
 * @brief Size of dummy USB standard descriptor
 */
#define USB_STANDARD_DESC_SIZE              2

/**
 * @brief Dummy USB standard descriptor
 *
 * All USB standard descriptors start with these two bytes. Use this type traversing over descriptors
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
    } USB_DESC_ATTR;
    uint8_t val[USB_STANDARD_DESC_SIZE];
} usb_standard_desc_t;
_Static_assert(sizeof(usb_standard_desc_t) == USB_STANDARD_DESC_SIZE, "Size of usb_standard_desc_t incorrect");

// ------------------ Device Descriptor --------------------

/**
 * @brief Size of a USB device descriptor in bytes
 */
#define USB_DEVICE_DESC_SIZE                18

/**
 * @brief Structure representing a USB device descriptor
 */
typedef union {
    struct {
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
    } USB_DESC_ATTR;
    uint8_t val[USB_DEVICE_DESC_SIZE];
} usb_device_desc_t;
_Static_assert(sizeof(usb_device_desc_t) == USB_DEVICE_DESC_SIZE, "Size of usb_device_desc_t incorrect");

/**
 * @brief Possible base class values of the bDeviceClass field of a USB device descriptor
 */
#define USB_CLASS_PER_INTERFACE             0x00
#define USB_CLASS_AUDIO                     0x01
#define USB_CLASS_COMM                      0x02
#define USB_CLASS_HID                       0x03
#define USB_CLASS_PHYSICAL                  0x05
#define USB_CLASS_STILL_IMAGE               0x06
#define USB_CLASS_PRINTER                   0x07
#define USB_CLASS_MASS_STORAGE              0x08
#define USB_CLASS_HUB                       0x09
#define USB_CLASS_CDC_DATA                  0x0a
#define USB_CLASS_CSCID                     0x0b
#define USB_CLASS_CONTENT_SEC               0x0d
#define USB_CLASS_VIDEO                     0x0e
#define USB_CLASS_WIRELESS_CONTROLLER       0xe0
#define USB_CLASS_PERSONAL_HEALTHCARE       0x0f
#define USB_CLASS_AUDIO_VIDEO               0x10
#define USB_CLASS_BILLBOARD                 0x11
#define USB_CLASS_USB_TYPE_C_BRIDGE         0x12
#define USB_CLASS_MISC                      0xef
#define USB_CLASS_APP_SPEC                  0xfe
#define USB_CLASS_VENDOR_SPEC               0xff

/**
 * @brief Vendor specific subclass code
 */
#define USB_SUBCLASS_VENDOR_SPEC            0xff

// -------------- Configuration Descriptor -----------------

/**
 * @brief Size of a short USB configuration descriptor in bytes
 *
 * @note The size of a full USB configuration includes all the interface and endpoint
 *       descriptors of that configuration.
 */
#define USB_CONFIG_DESC_SIZE        9

/**
 * @brief Structure representing a short USB configuration descriptor
 *
 * @note The full USB configuration includes all the interface and endpoint
 *       descriptors of that configuration.
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wTotalLength;
        uint8_t bNumInterfaces;
        uint8_t bConfigurationValue;
        uint8_t iConfiguration;
        uint8_t bmAttributes;
        uint8_t bMaxPower;
    } USB_DESC_ATTR;
    uint8_t val[USB_CONFIG_DESC_SIZE];
} usb_config_desc_t;
_Static_assert(sizeof(usb_config_desc_t) == USB_CONFIG_DESC_SIZE, "Size of usb_config_desc_t incorrect");

/**
 * @brief Bit masks belonging to the bmAttributes field of a configuration descriptor
 */
#define USB_BM_ATTRIBUTES_ONE               (1 << 7)    //Must be set
#define USB_BM_ATTRIBUTES_SELFPOWER         (1 << 6)    //Self powered
#define USB_BM_ATTRIBUTES_WAKEUP            (1 << 5)    //Can wake-up
#define USB_BM_ATTRIBUTES_BATTERY           (1 << 4)    //Battery powered

// ---------- Interface Association Descriptor -------------

/**
 * @brief Size of a USB interface association descriptor in bytes
 */
#define USB_IAD_DESC_SIZE    9

/**
 * @brief Structure representing a USB interface association descriptor
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bFirstInterface;
        uint8_t bInterfaceCount;
        uint8_t bFunctionClass;
        uint8_t bFunctionSubClass;
        uint8_t bFunctionProtocol;
        uint8_t iFunction;
    } USB_DESC_ATTR;
    uint8_t val[USB_IAD_DESC_SIZE];
} usb_iad_desc_t;
_Static_assert(sizeof(usb_iad_desc_t) == USB_IAD_DESC_SIZE, "Size of usb_iad_desc_t incorrect");

// ---------------- Interface Descriptor -------------------

/**
 * @brief Size of a USB interface descriptor in bytes
 */
#define USB_INTF_DESC_SIZE      9

/**
 * @brief Structure representing a USB interface descriptor
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bInterfaceNumber;
        uint8_t bAlternateSetting;
        uint8_t bNumEndpoints;
        uint8_t bInterfaceClass;
        uint8_t bInterfaceSubClass;
        uint8_t bInterfaceProtocol;
        uint8_t iInterface;
    } USB_DESC_ATTR;
    uint8_t val[USB_INTF_DESC_SIZE];
} usb_intf_desc_t;
_Static_assert(sizeof(usb_intf_desc_t) == USB_INTF_DESC_SIZE, "Size of usb_intf_desc_t incorrect");

// ----------------- Endpoint Descriptor -------------------

/**
 * @brief Size of a USB endpoint descriptor in bytes
 */
#define USB_EP_DESC_SIZE        7

/**
 * @brief Structure representing a USB endpoint descriptor
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bEndpointAddress;
        uint8_t bmAttributes;
        uint16_t wMaxPacketSize;
        uint8_t bInterval;
    } USB_DESC_ATTR;
    uint8_t val[USB_EP_DESC_SIZE];
} usb_ep_desc_t;
_Static_assert(sizeof(usb_ep_desc_t) == USB_EP_DESC_SIZE, "Size of usb_ep_desc_t incorrect");

/**
 * @brief Bit masks belonging to the bEndpointAddress field of an endpoint descriptor
 */
#define USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK              0x0f
#define USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK              0x80

/**
 * @brief Bit masks belonging to the bmAttributes field of an endpoint descriptor
 */
#define USB_BM_ATTRIBUTES_XFERTYPE_MASK                 0x03
#define USB_BM_ATTRIBUTES_XFER_CONTROL                  (0 << 0)
#define USB_BM_ATTRIBUTES_XFER_ISOC                     (1 << 0)
#define USB_BM_ATTRIBUTES_XFER_BULK                     (2 << 0)
#define USB_BM_ATTRIBUTES_XFER_INT                      (3 << 0)
#define USB_BM_ATTRIBUTES_SYNCTYPE_MASK                 0x0C    /* in bmAttributes */
#define USB_BM_ATTRIBUTES_SYNC_NONE                     (0 << 2)
#define USB_BM_ATTRIBUTES_SYNC_ASYNC                    (1 << 2)
#define USB_BM_ATTRIBUTES_SYNC_ADAPTIVE                 (2 << 2)
#define USB_BM_ATTRIBUTES_SYNC_SYNC                     (3 << 2)
#define USB_BM_ATTRIBUTES_USAGETYPE_MASK                0x30
#define USB_BM_ATTRIBUTES_USAGE_DATA                    (0 << 4)
#define USB_BM_ATTRIBUTES_USAGE_FEEDBACK                (1 << 4)
#define USB_BM_ATTRIBUTES_USAGE_IMPLICIT_FB             (2 << 4)

/**
 * @brief Macro helpers to get information about an endpoint from its descriptor
 */
#define USB_EP_DESC_GET_XFERTYPE(desc_ptr) ((usb_transfer_type_t) ((desc_ptr)->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK))
#define USB_EP_DESC_GET_EP_NUM(desc_ptr) ((desc_ptr)->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK)
#define USB_EP_DESC_GET_EP_DIR(desc_ptr) (((desc_ptr)->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) ? 1 : 0)
#define USB_EP_DESC_GET_MPS(desc_ptr) ((desc_ptr)->wMaxPacketSize & 0x7FF)

// ------------------ String Descriptor --------------------

/**
 * @brief Size of a short USB string descriptor in bytes
 */
#define USB_STR_DESC_SIZE       4

/**
 * @brief Structure representing a USB string descriptor
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wData[1];        /* UTF-16LE encoded */
    } USB_DESC_ATTR;
    uint8_t val[USB_STR_DESC_SIZE];
} usb_str_desc_t;
_Static_assert(sizeof(usb_str_desc_t) == USB_STR_DESC_SIZE, "Size of usb_str_desc_t incorrect");

#ifdef __cplusplus
}
#endif
