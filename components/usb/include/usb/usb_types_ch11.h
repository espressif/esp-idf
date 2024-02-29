/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USB Hub request types
 */

#define USB_BM_REQUEST_TYPE_HUB         (USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIP_DEVICE)
#define USB_BM_REQUEST_TYPE_PORT        (USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIP_OTHER)

/**
 * @brief USB Hub descriptor type
 */
#define USB_CLASS_DESCRIPTOR_TYPE_HUB   0x29

/**
 * @brief USB Hub Class bRequest codes
 *
 * See USB 2.0 spec Table 11-16
 */
typedef enum {
    USB_B_REQUEST_HUB_GET_PORT_STATUS     = 0x00,   /**< Get port status.  */
    USB_B_REQUEST_HUB_CLEAR_FEATURE       = 0x01,   /**< Clearing a feature disables that feature or starts a process associated with the feature. */
    USB_B_REQUEST_HUB_GET_STATE           = 0x02,   /**< Outdated. Used in previous specifications for GET_STATE */
    USB_B_REQUEST_HUB_SET_PORT_FEATURE    = 0x03,   /**< Set port feature. */
    USB_B_REQUEST_HUB_GET_DESCRIPTOR      = 0x06,   /**< Get HUB descriptor. */
    USB_B_REQUEST_HUB_SET_DESCRIPTOR      = 0x07,   /**< Set HUB descriptor. */
    USB_B_REQUEST_HUB_CLEAR_TT_BUFFER     = 0x08,   /**< This request clears the state of a Transaction Translator(TT) bulk/control buffer after it has been left in a busy state due to high-speed errors. */
    USB_B_REQUEST_HUB_RESET_TT            = 0x09,   /**< Reset TT. */
    USB_B_REQUEST_HUB_GET_TT_STATE        = 0x0A,   /**< Get TT state. */
    USB_B_REQUEST_HUB_STOP_TT             = 0x0B,   /**< Stop TT. */
} usb_hub_class_request_t ;

/**
 * @brief USB Hub Port feature selector codes
 *
 * See USB 2.0 spec Table 11-17
 */
typedef enum {
    USB_FEATURE_PORT_CONNECTION         = 0x00,
    USB_FEATURE_PORT_ENABLE             = 0x01,
    USB_FEATURE_PORT_SUSPEND            = 0x02,
    USB_FEATURE_PORT_OVER_CURRENT       = 0x03,
    USB_FEATURE_PORT_RESET              = 0x04,
    USB_FEATURE_PORT_POWER              = 0x08,
    USB_FEATURE_PORT_LOWSPEED           = 0x09,
    USB_FEATURE_C_PORT_CONNECTION       = 0x10,
    USB_FEATURE_C_PORT_ENABLE           = 0x11,
    USB_FEATURE_C_PORT_SUSPEND          = 0x12,
    USB_FEATURE_C_PORT_OVER_CURRENT     = 0x13,
    USB_FEATURE_C_PORT_RESET            = 0x14,
    USB_FEATURE_PORT_TEST               = 0x15,
    USB_FEATURE_PORT_INDICATOR          = 0x16,
} usb_hub_port_feature_t;

/**
 * @brief Size of a USB Hub Port Status and Hub Change results
 */
#define USB_PORT_STATUS_SIZE            4

/**
 * @brief USB Hub Port Status and Hub Change results
 *
 * See USB 2.0 spec Table 11-19 and Table 11-20
 */
typedef struct {
    union {
        struct {
            uint8_t     PORT_CONNECTION      : 1;   /**< 0 = No device is present. 1 = A device is present on this port.*/
            uint8_t     PORT_ENABLE          : 1;   /**< 0 = Port is disabled. 1 = Port is enabled.*/
            uint8_t     PORT_SUSPEND         : 1;   /**< 0 = Not suspended. 1 = Suspended or resuming. */
            uint8_t     PORT_OVER_CURRENT    : 1;   /**< 0 = All no over-current condition exists on this port. 1 = An over-current condition exists on this port. */
            uint8_t     PORT_RESET           : 1;   /**< 0 = Reset signaling not asserted. 1 = Reset signaling asserted. */
            uint8_t     RESERVED_1           : 3;   /**< Reserved field */
            uint8_t     PORT_POWER           : 1;   /**< 0 = This port is in the Powered-off state. 1 = This port is not in the Powered-off state. */
            uint8_t     PORT_LOW_SPEED       : 1;   /**< 0 = Full-speed or High-speed device attached to this port (determined by bit 10). 1 = Low-speed device attached to this port.*/
            uint8_t     PORT_HIGH_SPEED      : 1;   /**< 0 = Full-speed device attached to this port. 1 = High-speed device attached to this port. */
            uint8_t     PORT_TEST            : 1;   /**< 0 = This port is not in the Port Test Mode. 1 = This port is in Port Test Mode. */
            uint8_t     PORT_INDICATOR       : 1;   /**< 0 = Port indicator displays default colors. 1 = Port indicator displays software controlled color. */
            uint8_t     RESERVED_2           : 3;   /**< Reserved field */
        };
        uint16_t val;                               /**< Port status value */
    }   wPortStatus;

    union {
        struct {
            uint8_t     C_PORT_CONNECTION    : 1;   /**< 0 = No change has occurred to Current Connect status. 1 = Current Connect status has changed. */
            uint8_t     C_PORT_ENABLE        : 1;   /**< This field is set to one when a port is disabled because of a Port_Error condition */
            uint8_t     C_PORT_SUSPEND       : 1;   /**< 0 = No change. 1 = Resume complete. */
            uint8_t     C_PORT_OVER_CURRENT  : 1;   /**< 0 = No change has occurred to Over-Current Indicator. 1 = Over-Current Indicator has changed. */
            uint8_t     C_PORT_RESET         : 1;   /**< This field is set when reset processing on this port is complete. 0 = No change. 1 = Reset complete.*/
            uint16_t    RESERVED             : 11;  /**< Reserved field */
        };
        uint16_t val;                               /**< Port change value */
    }   wPortChange;
} __attribute__((packed)) usb_port_status_t;
ESP_STATIC_ASSERT(sizeof(usb_port_status_t) == USB_PORT_STATUS_SIZE, "Size of usb_port_status_t incorrect");

/**
 * @brief Size of a USB Hub Status
 */
#define USB_HUB_STATUS_SIZE             4

/**
 * @brief USB Hub Status
 */
typedef struct {
    union {
        struct {
            uint8_t HUB_LOCAL_POWER     : 1;    /**< 0 = Local power supply good. 1 = Local power supply lost (inactive)*/
            uint8_t HUB_OVER_CURRENT    : 1;    /**< 0 = No over-current condition currently exists. 1 = A hub over-current condition exists.*/
            uint16_t RESERVED           : 14;   /**< Reserved fields */
        };
        uint16_t val;                           /**< Hub status value */
    } wHubStatus;
    union {
        struct {
            uint8_t C_HUB_LOCAL_POWER   : 1;    /**< 0 = No change has occurred to Local Power Status. 1 = Local Power Status has changed.*/
            uint8_t C_HUB_OVER_CURRENT  : 1;    /**< 0 = No change has occurred to the Over-Current Status. 1 = Over-Current Status has changed.*/
            uint16_t RESERVED           : 14;   /**< Reserved fields */
        };
        uint16_t val;                           /**< Hub change value */
    } wHubChange;
} __attribute__((packed)) usb_hub_status_t;
ESP_STATIC_ASSERT(sizeof(usb_hub_status_t) == USB_HUB_STATUS_SIZE, "Size of usb_hub_status_t incorrect");

/**
 * @brief Size of a USB Hub Device descriptor
 */
#define USB_HUB_DESCRIPTOR_SIZE         (7)

/**
 * @brief USB Hub Device descriptor
 */
typedef struct {
    uint8_t  bDescLength;                   /**< Number of bytes in this descriptor, including this byte */
    uint8_t  bDescriptorType;               /**< Descriptor Type, value: 29H for Hub descriptor */
    uint8_t  bNbrPorts;                     /**< Number of downstream facing ports that this Hub supports */
    uint16_t wHubCharacteristics;           /**< Logical Power Switching Mode, Compound Device, Over-current Protection Mode, TT Think Time, Port Indicators Supported */
    uint8_t  bPwrOn2PwrGood;                /**< Time (in 2 ms intervals) from the time the power-on sequence begins on a port until power is good on that port */
    uint8_t  bHubContrCurrent;              /**< Maximum current requirements of the Hub Controller electronics in mA. */
} __attribute__((packed)) usb_hub_descriptor_t;
ESP_STATIC_ASSERT(sizeof(usb_hub_descriptor_t) == USB_HUB_DESCRIPTOR_SIZE, "Size of usb_hub_descriptor_t incorrect");

/**
 * @brief Initializer for a request to get HUB descriptor
 */
#define USB_SETUP_PACKET_INIT_GET_HUB_DESCRIPTOR(setup_pkt_ptr) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_HUB;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_HUB_GET_DESCRIPTOR;   \
    (setup_pkt_ptr)->wValue = (USB_CLASS_DESCRIPTOR_TYPE_HUB << 8); \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = sizeof(usb_hub_descriptor_t);  \
})

/**
 * @brief Initializer for a request to get HUB status
 */
#define USB_SETUP_PACKET_INIT_GET_HUB_STATUS(setup_pkt_ptr) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_HUB;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_HUB_GET_PORT_STATUS;   \
    (setup_pkt_ptr)->wValue = 0; \
    (setup_pkt_ptr)->wIndex = 0;    \
    (setup_pkt_ptr)->wLength = sizeof(usb_hub_status_t);  \
})

/**
 * @brief Initializer for a request to get port status
 */
#define USB_SETUP_PACKET_INIT_GET_PORT_STATUS(setup_pkt_ptr, port) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN | USB_BM_REQUEST_TYPE_PORT;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_HUB_GET_PORT_STATUS;   \
    (setup_pkt_ptr)->wValue = 0; \
    (setup_pkt_ptr)->wIndex = (port);    \
    (setup_pkt_ptr)->wLength = sizeof(usb_port_status_t);  \
})

/**
 * @brief Initializer for a set port feature
 */
#define USB_SETUP_PACKET_INIT_SET_PORT_FEATURE(setup_pkt_ptr, port, feature) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_PORT;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_HUB_SET_PORT_FEATURE;   \
    (setup_pkt_ptr)->wValue = (feature); \
    (setup_pkt_ptr)->wIndex = (port);    \
    (setup_pkt_ptr)->wLength = 0;  \
})

/**
 * @brief Initializer for a clear port feature
 */
#define USB_SETUP_PACKET_INIT_CLEAR_PORT_FEATURE(setup_pkt_ptr, port, feature) ({  \
    (setup_pkt_ptr)->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT | USB_BM_REQUEST_TYPE_PORT;   \
    (setup_pkt_ptr)->bRequest = USB_B_REQUEST_HUB_CLEAR_FEATURE;   \
    (setup_pkt_ptr)->wValue = (feature); \
    (setup_pkt_ptr)->wIndex = (port);    \
    (setup_pkt_ptr)->wLength = 0;  \
})

/**
 * @brief Get Port Number from a setup packet
 */
#define USB_SETUP_PACKET_GET_PORT(setup_pkt_ptr) ({  \
    (setup_pkt_ptr)->wIndex;    \
})

#ifdef __cplusplus
}
#endif //__cplusplus
