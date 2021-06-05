// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
Note: This header file contains the types and macros belong/relate to the USB2.0 protocol and are HW implementation
agnostic. In other words, this header is only meant to be used in the HCD layer and above of the USB Host stack. For
types and macros that are HW implementation specific (i.e., HAL layer and below), add them to the "usb_types.h" header
instead.
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <sys/queue.h>

#define USB_CTRL_REQ_ATTR       __attribute__((packed))
#define USB_DESC_ATTR           __attribute__((packed))

// ------------------------------------------------ Common USB Types ---------------------------------------------------

// --------------------- Bus Related -----------------------

/**
 * @brief USB Standard Speeds
 */
typedef enum {
    USB_SPEED_LOW = 0,                  /**< USB Low Speed (1.5 Mbit/s) */
    USB_SPEED_FULL,                     /**< USB Full Speed (12 Mbit/s) */
} usb_speed_t;

// ------------------ Transfer Related ---------------------

/**
 * @brief The type of USB transfer
 *
 * @note The enum values need to match the bmAttributes field of an EP descriptor
 */
typedef enum {
    USB_XFER_TYPE_CTRL = 0,
    USB_XFER_TYPE_ISOCHRONOUS,
    USB_XFER_TYPE_BULK,
    USB_XFER_TYPE_INTR,
} usb_transfer_type_t;

/**
 * @brief The status of a particular transfer
 */
typedef enum {
    USB_TRANSFER_STATUS_COMPLETED,      /**< The transfer was successful (but may be short) */
    USB_TRANSFER_STATUS_ERROR,          /**< The transfer failed because due to excessive errors (e.g. no response or CRC error) */
    USB_TRANSFER_STATUS_TIMED_OUT,      /**< The transfer failed due to a time out */
    USB_TRANSFER_STATUS_CANCELLED,      /**< The transfer was canceled */
    USB_TRANSFER_STATUS_STALL,          /**< The transfer was stalled */
    USB_TRANSFER_STATUS_NO_DEVICE,      /**< The transfer failed because the device is no longer valid (e.g., disconnected */
    USB_TRANSFER_STATUS_OVERFLOW,       /**< The transfer as more data was sent than was requested */
} usb_transfer_status_t;

/**
 * @brief Isochronous packet descriptor
 *
 * If the number of bytes in an IRP (I/O Request Packet, see USB2.0 Spec) is
 * larger than the MPS of the endpoint, the IRP is split over multiple packets
 * (one packet per bInterval of the endpoint). An array of Isochronous packet
 * descriptors describes how an IRP should be split over multiple packets.
 */
typedef struct {
    int length;                         /**< Number of bytes to transmit/receive in the packet */
    int actual_length;                  /**< Actual number of bytes transmitted/received in the packet */
    usb_transfer_status_t status;       /**< Status of the packet */
} usb_iso_packet_desc_t;

/**
 * @brief USB IRP (I/O Request Packet). See USB2.0 Spec
 *
 * An IRP is used to represent data transfer request form a software client to and endpoint over the USB bus. The same
 * IRP object type is used at each layer of the USB stack. This minimizes copying/conversion across the different layers
 * of the stack as each layer will pass a pointer to this type of object.
 *
 * See 10.5.3.1 os USB2.0 specification
 * Bulk: Represents a single bulk transfer which a pipe will transparently split into multiple MPS transactions (until
 *       the last)
 * Control: Represents a single control transfer with the setup packet at the first 8 bytes of the buffer.
 * Interrupt: Represents a single interrupt transaction
 * Isochronous: Represents a buffer of a stream of bytes which the pipe will transparently transfer the stream of bytes
 *              one or more service periods
 *
 * @note The tailq_entry and reserved variables are used by the USB Host stack internally. Users should not modify those fields.
 * @note Once an IRP is submitted, users should not modify the IRP as the Host stack takes ownership of the IRP.
 */
struct usb_irp_obj {
    //Internal members
    TAILQ_ENTRY(usb_irp_obj) tailq_entry;   /**< TAILQ entry that allows this object to be added to linked lists. Users should NOT modify this field */
    void *reserved_ptr;                     /**< Reserved pointer variable for internal use in the stack. Users should set this to NULL on allocation and NOT modify this afterwards */
    uint32_t reserved_flags;                   /**< Reserved variable for flags used internally in the stack. Users should set this to 0 on allocation and NOT modify this afterwards */
    //Public members
    uint8_t *data_buffer;                   /**< Pointer to data buffer. Must be DMA capable memory */
    int num_bytes;                          /**< Number of bytes in IRP. Control should exclude size of setup. IN should be integer multiple of MPS */
    int actual_num_bytes;                   /**< Actual number of bytes transmitted/receives in the IRP */
    usb_transfer_status_t status;           /**< Status of the transfer */
    uint32_t timeout;                       /**< Timeout (in milliseconds) of the packet */
    void *context;                          /**< Context variable used to associate the IRP object with another object */
    int num_iso_packets;                    /**< Only relevant to Isochronous. Number of service periods to transfer data buffer over. Set to 0 for non-iso transfers */
    usb_iso_packet_desc_t iso_packet_desc[0];   /**< Descriptors for each ISO packet */
};

typedef struct usb_irp_obj usb_irp_t;

// ---------------------------------------------------- Chapter 9 ------------------------------------------------------

// ------------------- Control Request ---------------------

/**
 * @brief Size of a USB control transfer setup packet in bytes
 */
#define USB_CTRL_REQ_SIZE       8

/**
 * @brief Structure representing a USB control transfer setup packet
 */
typedef union {
    struct {
        uint8_t bRequestType;
        uint8_t bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    } USB_CTRL_REQ_ATTR;
    uint8_t val[USB_CTRL_REQ_SIZE];
} usb_ctrl_req_t;
_Static_assert(sizeof(usb_ctrl_req_t) == USB_CTRL_REQ_SIZE, "Size of usb_ctrl_req_t incorrect");

/**
 * @brief Bit masks belonging to the bRequestType field of a setup packet
 */
#define USB_B_REQUEST_TYPE_DIR_OUT          (0X00 << 7)
#define USB_B_REQUEST_TYPE_DIR_IN           (0x01 << 7)
#define USB_B_REQUEST_TYPE_TYPE_STANDARD    (0x00 << 5)
#define USB_B_REQUEST_TYPE_TYPE_CLASS       (0x01 << 5)
#define USB_B_REQUEST_TYPE_TYPE_VENDOR      (0x02 << 5)
#define USB_B_REQUEST_TYPE_TYPE_RESERVED    (0x03 << 5)
#define USB_B_REQUEST_TYPE_TYPE_MASK        (0x03 << 5)
#define USB_B_REQUEST_TYPE_RECIP_DEVICE     (0x00 << 0)
#define USB_B_REQUEST_TYPE_RECIP_INTERFACE  (0x01 << 0)
#define USB_B_REQUEST_TYPE_RECIP_ENDPOINT   (0x02 << 0)
#define USB_B_REQUEST_TYPE_RECIP_OTHER      (0x03 << 0)
#define USB_B_REQUEST_TYPE_RECIP_MASK       (0x1f << 0)

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
#define USB_CTRL_REQ_INIT_SET_ADDR(ctrl_req_ptr, addr) ({  \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_OUT | USB_B_REQUEST_TYPE_TYPE_STANDARD |USB_B_REQUEST_TYPE_RECIP_DEVICE;   \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_SET_ADDRESS;  \
    (ctrl_req_ptr)->wValue = (addr);   \
    (ctrl_req_ptr)->wIndex = 0;    \
    (ctrl_req_ptr)->wLength = 0;   \
})

/**
 * @brief Initializer for a request to get a device's device descriptor
 */
#define USB_CTRL_REQ_INIT_GET_DEVC_DESC(ctrl_req_ptr) ({ \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_IN | USB_B_REQUEST_TYPE_TYPE_STANDARD | USB_B_REQUEST_TYPE_RECIP_DEVICE;   \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_GET_DESCRIPTOR;   \
    (ctrl_req_ptr)->wValue = (USB_W_VALUE_DT_DEVICE << 8); \
    (ctrl_req_ptr)->wIndex = 0;    \
    (ctrl_req_ptr)->wLength = 18;  \
})

/**
 * @brief Initializer for a request to get a device's current configuration number
 */
#define USB_CTRL_REQ_INIT_GET_CONFIG(ctrl_req_ptr) ({  \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_IN | USB_B_REQUEST_TYPE_TYPE_STANDARD | USB_B_REQUEST_TYPE_RECIP_DEVICE;   \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_GET_CONFIGURATION;    \
    (ctrl_req_ptr)->wValue = 0;    \
    (ctrl_req_ptr)->wIndex = 0;    \
    (ctrl_req_ptr)->wLength = 1;   \
})

/**
 * @brief Initializer for a request to get one of the device's current configuration descriptor
 *
 * - desc_index indicates the configuration's index number
 * - Number of bytes of the configuration descriptor to get
 */
#define USB_CTRL_REQ_INIT_GET_CFG_DESC(ctrl_req_ptr, desc_index, desc_len) ({  \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_IN | USB_B_REQUEST_TYPE_TYPE_STANDARD | USB_B_REQUEST_TYPE_RECIP_DEVICE;   \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_GET_DESCRIPTOR;   \
    (ctrl_req_ptr)->wValue = (USB_W_VALUE_DT_CONFIG << 8) | ((desc_index) & 0xFF); \
    (ctrl_req_ptr)->wIndex = 0;    \
    (ctrl_req_ptr)->wLength = (desc_len);  \
})

/**
 * @brief Initializer for a request to set a device's current configuration number
 */
#define USB_CTRL_REQ_INIT_SET_CONFIG(ctrl_req_ptr, config_num) ({  \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_OUT | USB_B_REQUEST_TYPE_TYPE_STANDARD | USB_B_REQUEST_TYPE_RECIP_DEVICE;   \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_SET_CONFIGURATION;    \
    (ctrl_req_ptr)->wValue = (config_num); \
    (ctrl_req_ptr)->wIndex = 0;    \
    (ctrl_req_ptr)->wLength = 0;   \
})

// ------------------ Device Descriptor --------------------

/**
 * @brief Size of a USB device descriptor in bytes
 */
#define USB_DESC_DEV_SIZE       18

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
    uint8_t val[USB_DESC_DEV_SIZE];
} usb_desc_devc_t;
_Static_assert(sizeof(usb_desc_devc_t) == USB_DESC_DEV_SIZE, "Size of usb_desc_devc_t incorrect");

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
#define USB_DESC_CFG_SIZE       9

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
    uint8_t val[USB_DESC_CFG_SIZE];
} usb_desc_cfg_t;
_Static_assert(sizeof(usb_desc_cfg_t) == USB_DESC_CFG_SIZE, "Size of usb_desc_cfg_t incorrect");

/**
 * @brief Bit masks belonging to the bmAttributes field of a configuration descriptor
 */
#define USB_BM_ATTRIBUTES_ONE               (1 << 7)    //Must be set
#define USB_BM_ATTRIBUTES_SELFPOWER         (1 << 6)    //Self powered
#define USB_BM_ATTRIBUTES_WAKEUP            (1 << 5)    //Can wake-up
#define USB_BM_ATTRIBUTES_BATTERY           (1 << 4)    //Battery powered

// ---------------- Interface Descriptor -------------------

/**
 * @brief Size of a USB interface descriptor in bytes
 */
#define USB_DESC_INTF_SIZE      9

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
    uint8_t val[USB_DESC_INTF_SIZE];
} usb_desc_intf_t;
_Static_assert(sizeof(usb_desc_intf_t) == USB_DESC_INTF_SIZE, "Size of usb_desc_intf_t incorrect");

// ----------------- Endpoint Descriptor -------------------

/**
 * @brief Size of a USB endpoint descriptor in bytes
 */
#define USB_DESC_EP_SIZE        7

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
    uint8_t val[USB_DESC_EP_SIZE];
} usb_desc_ep_t;
_Static_assert(sizeof(usb_desc_ep_t) == USB_DESC_EP_SIZE, "Size of usb_desc_ep_t incorrect");

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
#define USB_DESC_EP_GET_XFERTYPE(desc_ptr) ((usb_transfer_type_t) ((desc_ptr)->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK))
#define USB_DESC_EP_GET_EP_NUM(desc_ptr) ((desc_ptr)->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK)
#define USB_DESC_EP_GET_EP_DIR(desc_ptr) (((desc_ptr)->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) ? 1 : 0)
#define USB_DESC_EP_GET_MPS(desc_ptr) ((desc_ptr)->wMaxPacketSize & 0x7FF)

// ------------------ String Descriptor --------------------

/**
 * @brief Size of a short USB string descriptor in bytes
 */
#define USB_DESC_STR_SIZE       4

/**
 * @brief Structure representing a USB string descriptor
 */
typedef union {
    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wData[1];        /* UTF-16LE encoded */
    } USB_DESC_ATTR;
    uint8_t val[USB_DESC_STR_SIZE];
} usb_desc_str_t;
_Static_assert(sizeof(usb_desc_str_t) == USB_DESC_STR_SIZE, "Size of usb_desc_str_t incorrect");

#ifdef __cplusplus
}
#endif
