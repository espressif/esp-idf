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
types and macros that are HW implementation specific (i.e., HAL layer and below), add them to the
"hal/usb_types_private.h" header instead.
*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
    USB_TRANSFER_TYPE_CTRL = 0,
    USB_TRANSFER_TYPE_ISOCHRONOUS,
    USB_TRANSFER_TYPE_BULK,
    USB_TRANSFER_TYPE_INTR,
} usb_transfer_type_t;

/**
 * @brief The status of a particular transfer
 */
typedef enum {
    USB_TRANSFER_STATUS_COMPLETED,      /**< The transfer was successful (but may be short) */
    USB_TRANSFER_STATUS_ERROR,          /**< The transfer failed because due to excessive errors (e.g. no response or CRC error) */
    USB_TRANSFER_STATUS_TIMED_OUT,      /**< The transfer failed due to a time out */
    USB_TRANSFER_STATUS_CANCELED,       /**< The transfer was canceled */
    USB_TRANSFER_STATUS_STALL,          /**< The transfer was stalled */
    USB_TRANSFER_STATUS_NO_DEVICE,      /**< The transfer failed because the device is no longer valid (e.g., disconnected */
    USB_TRANSFER_STATUS_OVERFLOW,       /**< The transfer as more data was sent than was requested */
    USB_TRANSFER_STATUS_SKIPPED,        /**< ISOC packets only. The packet was skipped due to system latency or bus overload */
} usb_transfer_status_t;


#define USB_TRANSFER_FLAG_ZERO_PACK  0x01   /**< (For bulk OUT only). Indicates that a bulk OUT transfers should always terminate with a short packet, even if it means adding an extra zero length packet */

/**
 * @brief Isochronous packet descriptor
 *
 * If the number of bytes in an Isochronous transfer is larger than the MPS of the endpoint, the transfer is split
 * into multiple packets transmitted at the endpoint's specified interval. An array of Isochronous packet descriptors
 * describes how an Isochronous transfer should be split into multiple packets.
 */
typedef struct {
    int num_bytes;                                  /**< Number of bytes to transmit/receive in the packet */
    int actual_num_bytes;                           /**< Actual number of bytes transmitted/received in the packet */
    usb_transfer_status_t status;                   /**< Status of the packet */
} usb_isoc_packet_desc_t;

/**
 * @brief USB transfer structure
 *
 * This structure is used to represent a transfer from a software client to an endopint over the USB bus. Some of the
 * fields are made const on purpose as they are fixed on allocation. Users should call the appropriate USB Host Driver
 * function to allocate a USB transfer structure instead of allocating this structure themselves.
 *
 * The transfer type is inferred from the endpoint this transfer is sent to. Depending on the transfer type, users
 * should note the following:
 *
 * - Bulk: This structure represents a single bulk transfer. If the number of bytes exceeds the endpoint's MPS, the
 *         transfer will be split into multiple MPS sized packets followed by a short packet.
 * - Control: This structure represents a single control transfer. This first 8 bytes of the data_buffer must be filled
 *            with the setup packet. The num_bytes field should exclude the size of the setup packet (i.e., set to 0 if
 *            the control transfer has no data stage).
 * - Interrupt: Represents an interrupt transfer. If num_bytes exceeds the MPS of the endpoint, the transfer will be
 *              split into multiple packets, and each packet is transferred at the endpoint's specified interval.
 * - Isochronous: Represents a stream of bytes that should be transferred to an endpoint at a fixed rate. The transfer
 *                is split into packets according to the each isoc_packet_desc. A packet is transferred at each interval
 *                of the endpoint.
 *
 * @note For Bulk/Control/Interrupt IN transfers, the num_bytes must be a integer multiple of the endpoint's MPS
 * @note This structure should be allocated via __insert_func_name__()
 * @note Once the transfer has be submitted, users should not modify the structure until the transfer has completed
 */
typedef struct usb_transfer_obj usb_transfer_t;

/**
 * @brief USB transfer completion callback
 */
typedef void (*usb_transfer_cb_t)(usb_transfer_t *transfer);

struct usb_transfer_obj{
    uint8_t *const data_buffer;                     /**< Pointer to data buffer */
    const size_t data_buffer_size;                  /**< Size of the data buffer in bytes */
    int num_bytes;                                  /**< Number of bytes to transfer. Control transfers should exclude size of setup packet. IN transfers should be integer multiple of MPS */
    int actual_num_bytes;                           /**< Actual number of bytes transferred */
    uint32_t flags;                                 /**< Transfer flags */
    usb_transfer_status_t status;                   /**< Status of the transfer */
    uint32_t timeout;                               /**< Timeout (in milliseconds) of the packet (currently not supported yet) */
    usb_transfer_cb_t callback;                     /**< Transfer callback */
    void *context;                                  /**< Context variable for transfer to associate transfer with something */
    const int num_isoc_packets;                     /**< Only relevant to Isochronous. Number of service periods (i.e., intervals) to transfer data buffer over. */
    usb_isoc_packet_desc_t isoc_packet_desc[0];     /**< Descriptors for each Isochronous packet */
};


// ---------------------------------------------------- Chapter 9 ------------------------------------------------------

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
#define USB_CTRL_REQ_INIT_GET_DEVICE_DESC(ctrl_req_ptr) ({  \
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
#define USB_CTRL_REQ_INIT_GET_CONFIG_DESC(ctrl_req_ptr, desc_index, desc_len) ({    \
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

/**
 * @brief Initializer for a request to set an interface's alternate setting
 */
#define USB_CTRL_REQ_INIT_SET_INTERFACE(ctrl_req_ptr, intf_num, alt_setting_num) ({    \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_OUT | USB_B_REQUEST_TYPE_TYPE_STANDARD | USB_B_REQUEST_TYPE_RECIP_INTERFACE;  \
    (ctrl_req_ptr)->bRequest = USB_B_REQUEST_SET_INTERFACE; \
    (ctrl_req_ptr)->wValue = (alt_setting_num); \
    (ctrl_req_ptr)->wIndex = (intf_num);  \
    (ctrl_req_ptr)->wLength = 0;   \
})

// ---------------- Standard Descriptor --------------------

/**
 * @brief Size of dummy USB standard descriptor
 */
#define USB_DESC_STANDARD_SIZE      2

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
    uint8_t val[USB_DESC_STANDARD_SIZE];
} usb_desc_standard_t;
_Static_assert(sizeof(usb_desc_standard_t) == USB_DESC_STANDARD_SIZE, "Size of usb_desc_standard_t incorrect");

// ------------------ Device Descriptor --------------------

/**
 * @brief Size of a USB device descriptor in bytes
 */
#define USB_DESC_DEVICE_SIZE            18

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
    uint8_t val[USB_DESC_DEVICE_SIZE];
} usb_desc_device_t;
_Static_assert(sizeof(usb_desc_device_t) == USB_DESC_DEVICE_SIZE, "Size of usb_desc_device_t incorrect");

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
#define USB_DESC_CONFIG_SIZE        9

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
    uint8_t val[USB_DESC_CONFIG_SIZE];
} usb_desc_config_t;
_Static_assert(sizeof(usb_desc_config_t) == USB_DESC_CONFIG_SIZE, "Size of usb_desc_config_t incorrect");

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
#define USB_DESC_INTF_ASSOC_SIZE    9

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
    uint8_t val[USB_DESC_INTF_ASSOC_SIZE];
} usb_desc_iad_t;
_Static_assert(sizeof(usb_desc_iad_t) == USB_DESC_INTF_ASSOC_SIZE, "Size of usb_desc_iad_t incorrect");

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
