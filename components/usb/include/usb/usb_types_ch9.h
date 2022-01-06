/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Warning: The USB Host Library API is still a beta version and may be subject to change
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
 *
 * See Table 9-1 of USB2.0 specification for more details
 *
 * @note The USB_DEVICE_STATE_NOT_ATTACHED is not part of the USB2.0 specification, but is a catch all state for devices
 *       that need to be cleaned up after a sudden disconnection or port error.
 */
typedef enum {
    USB_DEVICE_STATE_NOT_ATTACHED,              /**< The device was previously configured or suspended, but is no longer attached (either suddenly disconnected or a port error) */
    USB_DEVICE_STATE_ATTACHED,                  /**< Device is attached to the USB, but is not powered. */
    USB_DEVICE_STATE_POWERED,                   /**< Device is attached to the USB and powered, but has not been reset. */
    USB_DEVICE_STATE_DEFAULT,                   /**< Device is attached to the USB and powered and has been reset, but has not been assigned a unique address. Device responds at the default address. */
    USB_DEVICE_STATE_ADDRESS,                   /**< Device is attached to the USB, powered, has been reset, and a unique device address has been assigned. Device is not configured. */
    USB_DEVICE_STATE_CONFIGURED,                /**< Device is attached to the USB, powered, has been reset, has a unique address, is configured, and is not suspended. The host may now use the function provided by the device. */
    USB_DEVICE_STATE_SUSPENDED,                 /**< Device is, at minimum, attached to the USB and is powered and has not seen bus activity for 3 ms. It may also have a unique address and be configured for use. However, because the device is suspended, the host may not use the device’s function. */
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
 *
 * See Table 9-2 of USB2.0 specification for more details
 */
typedef union {
    struct {
        uint8_t bmRequestType;              /**< Characteristics of request */
        uint8_t bRequest;                   /**< Specific request */
        uint16_t wValue;                    /**< Word-sized field that varies according to request */
        uint16_t wIndex;                    /**< Word-sized field that varies according to request; typically used to pass an index or offset */
        uint16_t wLength;                   /**< Number of bytes to transfer if there is a data stage */
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

/**
 * @brief Initializer for a request to get an string descriptor
 */
#define USB_SETUP_PACKET_INIT_GET_STR_DESC(setup_pkt_ptr, string_index, lang_id, desc_len) ({ \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIP_DEVICE; \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_GET_DESCRIPTOR; \
    (setup_pkt_ptr)->wValue = (USB_W_VALUE_DT_STRING << 8) | ((string_index) & 0xFF); \
    (setup_pkt_ptr)->wIndex = (lang_id); \
    (setup_pkt_ptr)->wLength = (desc_len); \
})

// ---------------- Standard Descriptor --------------------

/**
 * @brief Size of dummy USB standard descriptor
 */
#define USB_STANDARD_DESC_SIZE              2

/**
 * @brief USB standard descriptor
 *
 * All USB standard descriptors start with these two bytes. Use this type when traversing over configuration descriptors
 */
typedef union {
    struct {
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< Descriptor Type */
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
 *
 * See Table 9-8 of USB2.0 specification for more details
 */
typedef union {
    struct {
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< DEVICE Descriptor Type */
        uint16_t bcdUSB;                    /**< USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H) */
        uint8_t bDeviceClass;               /**< Class code (assigned by the USB-IF) */
        uint8_t bDeviceSubClass;            /**< Subclass code (assigned by the USB-IF) */
        uint8_t bDeviceProtocol;            /**< Protocol code (assigned by the USB-IF) */
        uint8_t bMaxPacketSize0;            /**< Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid) */
        uint16_t idVendor;                  /**< Vendor ID (assigned by the USB-IF) */
        uint16_t idProduct;                 /**< Product ID (assigned by the manufacturer) */
        uint16_t bcdDevice;                 /**< Device release number in binary-coded decimal */
        uint8_t iManufacturer;              /**< Index of string descriptor describing manufacturer */
        uint8_t iProduct;                   /**< Index of string descriptor describing product */
        uint8_t iSerialNumber;              /**< Index of string descriptor describing the device’s serial number */
        uint8_t bNumConfigurations;         /**< Number of possible configurations */
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
 * See Table 9-10 of USB2.0 specification for more details
 *
 * @note The full USB configuration includes all the interface and endpoint
 *       descriptors of that configuration.
 */
typedef union {
    struct {
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< CONFIGURATION Descriptor Type */
        uint16_t wTotalLength;              /**< Total length of data returned for this configuration */
        uint8_t bNumInterfaces;             /**< Number of interfaces supported by this configuration */
        uint8_t bConfigurationValue;        /**< Value to use as an argument to the SetConfiguration() request to select this configuration */
        uint8_t iConfiguration;             /**< Index of string descriptor describing this configuration */
        uint8_t bmAttributes;               /**< Configuration characteristics */
        uint8_t bMaxPower;                  /**< Maximum power consumption of the USB device from the bus in this specific configuration when the device is fully operational. */
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
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< INTERFACE ASSOCIATION Descriptor Type */
        uint8_t bFirstInterface;            /**< Interface number of the first interface that is associated with this function */
        uint8_t bInterfaceCount;            /**< Number of contiguous interfaces that are associated with this function */
        uint8_t bFunctionClass;             /**< Class code (assigned by USB-IF) */
        uint8_t bFunctionSubClass;          /**< Subclass code (assigned by USB-IF) */
        uint8_t bFunctionProtocol;          /**< Protocol code (assigned by USB-IF) */
        uint8_t iFunction;                  /**< Index of string descriptor describing this function */
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
 *
 * See Table 9-12 of USB2.0 specification for more details
 */
typedef union {
    struct {
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< INTERFACE Descriptor Type */
        uint8_t bInterfaceNumber;           /**< Number of this interface. */
        uint8_t bAlternateSetting;          /**< Value used to select this alternate setting for the interface identified in the prior field */
        uint8_t bNumEndpoints;              /**< Number of endpoints used by this interface (excluding endpoint zero). */
        uint8_t bInterfaceClass;            /**< Class code (assigned by the USB-IF) */
        uint8_t bInterfaceSubClass;         /**< Subclass code (assigned by the USB-IF) */
        uint8_t bInterfaceProtocol;         /**< Protocol code (assigned by the USB) */
        uint8_t iInterface;                 /**< Index of string descriptor describing this interface */
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
 *
 * See Table 9-13 of USB2.0 specification for more details
 */
typedef union {
    struct {
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< ENDPOINT Descriptor Type */
        uint8_t bEndpointAddress;           /**< The address of the endpoint on the USB device described by this descriptor */
        uint8_t bmAttributes;               /**< This field describes the endpoint’s attributes when it is configured using the bConfigurationValue. */
        uint16_t wMaxPacketSize;            /**< Maximum packet size this endpoint is capable of sending or receiving when this configuration is selected. */
        uint8_t bInterval;                  /**< Interval for polling Isochronous and Interrupt endpoints. Expressed in frames or microframes depending on the device operating speed (1 ms for Low-Speed and Full-Speed or 125 us for USB High-Speed and above). */
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
#define USB_STR_DESC_SIZE       2

/**
 * @brief Structure representing a USB string descriptor
 */
typedef union {
    struct {
        uint8_t bLength;                    /**< Size of the descriptor in bytes */
        uint8_t bDescriptorType;            /**< STRING Descriptor Type */
        uint16_t wData[];                   /**< UTF-16LE encoded */
    } USB_DESC_ATTR;
    uint8_t val[USB_STR_DESC_SIZE];
} usb_str_desc_t;
_Static_assert(sizeof(usb_str_desc_t) == USB_STR_DESC_SIZE, "Size of usb_str_desc_t incorrect");

#ifdef __cplusplus
}
#endif
