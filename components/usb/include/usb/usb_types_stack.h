/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Warning: The USB Host Library API is still a beta version and may be subject to change
*/

#pragma once

#include "usb/usb_types_ch9.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------ Protocol Standard --------------------------------------------------

/**
 * @brief USB Standard Speeds
 */
typedef enum {
    USB_SPEED_LOW = 0,                  /**< USB Low Speed (1.5 Mbit/s) */
    USB_SPEED_FULL,                     /**< USB Full Speed (12 Mbit/s) */
} usb_speed_t;

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

// ------------------------------------------------- Device Related ----------------------------------------------------

/**
 * @brief Handle of a USB Device connected to a USB Host
 */
typedef struct usb_device_handle_s * usb_device_handle_t;

/**
 * @brief Basic information of an enumerated device
 */
typedef struct {
    usb_speed_t speed;                  /**< Device's speed */
    uint8_t dev_addr;                   /**< Device's address */
    uint8_t bMaxPacketSize0;            /**< The maximum packet size of the device's default endpoint */
    uint8_t bConfigurationValue;        /**< Device's current configuration number */
} usb_device_info_t;

// ------------------------------------------------ Transfer Related ---------------------------------------------------

/**
 * @brief The status of a particular transfer
 */
typedef enum {
    USB_TRANSFER_STATUS_COMPLETED,      /**< The transfer was successful (but may be short) */
    USB_TRANSFER_STATUS_ERROR,          /**< The transfer failed because due to excessive errors (e.g. no response or CRC error) */
    USB_TRANSFER_STATUS_TIMED_OUT,      /**< The transfer failed due to a time out */
    USB_TRANSFER_STATUS_CANCELED,       /**< The transfer was canceled */
    USB_TRANSFER_STATUS_STALL,          /**< The transfer was stalled */
    USB_TRANSFER_STATUS_OVERFLOW,       /**< The transfer as more data was sent than was requested */
    USB_TRANSFER_STATUS_SKIPPED,        /**< ISOC packets only. The packet was skipped due to system latency or bus overload */
    USB_TRANSFER_STATUS_NO_DEVICE,      /**< The transfer failed because the target device is gone */
} usb_transfer_status_t;

/**
 * @brief Isochronous packet descriptor
 *
 * If the number of bytes in an Isochronous transfer is larger than the MPS of the endpoint, the transfer is split
 * into multiple packets transmitted at the endpoint's specified interval. An array of Isochronous packet descriptors
 * describes how an Isochronous transfer should be split into multiple packets.
 */
typedef struct {
    int num_bytes;                                  /**< Number of bytes to transmit/receive in the packet. IN packets should be integer multiple of MPS */
    int actual_num_bytes;                           /**< Actual number of bytes transmitted/received in the packet */
    usb_transfer_status_t status;                   /**< Status of the packet */
} usb_isoc_packet_desc_t;

/**
 * @brief USB transfer structure
 *
 * This structure is used to represent a transfer from a software client to an endpoint over the USB bus. Some of the
 * fields are made const on purpose as they are fixed on allocation. Users should call the appropriate USB Host Library
 * function to allocate a USB transfer structure instead of allocating this structure themselves.
 *
 * The transfer type is inferred from the endpoint this transfer is sent to. Depending on the transfer type, users
 * should note the following:
 *
 * - Bulk: This structure represents a single bulk transfer. If the number of bytes exceeds the endpoint's MPS, the
 *         transfer will be split into multiple MPS sized packets followed by a short packet.
 * - Control: This structure represents a single control transfer. This first 8 bytes of the data_buffer must be filled
 *            with the setup packet (see usb_setup_packet_t). The num_bytes field should be the total size of the
 *            transfer (i.e., size of setup packet + wLength).
 * - Interrupt: Represents an interrupt transfer. If num_bytes exceeds the MPS of the endpoint, the transfer will be
 *              split into multiple packets, and each packet is transferred at the endpoint's specified interval.
 * - Isochronous: Represents a stream of bytes that should be transferred to an endpoint at a fixed rate. The transfer
 *                is split into packets according to the each isoc_packet_desc. A packet is transferred at each interval
 *                of the endpoint. If an entire ISOC URB was transferred without error (skipped packets do not count as
 *                errors), the URB's overall status and the status of each packet descriptor will be updated, and the
 *                actual_num_bytes reflects the total bytes transferred over all packets. If the ISOC URB encounters an
 *                error, the entire URB is considered erroneous so only the overall status will updated.
 *
 * @note For Bulk/Control/Interrupt IN transfers, the num_bytes must be a integer multiple of the endpoint's MPS
 * @note This structure should be allocated via usb_host_transfer_alloc()
 * @note Once the transfer has be submitted, users should not modify the structure until the transfer has completed
 */
typedef struct usb_transfer_s usb_transfer_t;

/**
 * @brief USB transfer completion callback
 */
typedef void (*usb_transfer_cb_t)(usb_transfer_t *transfer);

struct usb_transfer_s{
    uint8_t *const data_buffer;                     /**< Pointer to data buffer */
    const size_t data_buffer_size;                  /**< Size of the data buffer in bytes */
    int num_bytes;                                  /**< Number of bytes to transfer.
                                                         Control transfers should include the size of the setup packet.
                                                         Isochronous transfer should be the total transfer size of all packets.
                                                         For non-control IN transfers, num_bytes should be an integer multiple of MPS. */
    int actual_num_bytes;                           /**< Actual number of bytes transferred */
    uint32_t flags;                                 /**< Transfer flags */
    usb_device_handle_t device_handle;              /**< Device handle */
    uint8_t bEndpointAddress;                       /**< Endpoint Address */
    usb_transfer_status_t status;                   /**< Status of the transfer */
    uint32_t timeout_ms;                            /**< Timeout (in milliseconds) of the packet (currently not supported yet) */
    usb_transfer_cb_t callback;                     /**< Transfer callback */
    void *context;                                  /**< Context variable for transfer to associate transfer with something */
    const int num_isoc_packets;                     /**< Only relevant to Isochronous. Number of service periods (i.e., intervals) to transfer data buffer over. */
    usb_isoc_packet_desc_t isoc_packet_desc[0];     /**< Descriptors for each Isochronous packet */
};

#define USB_TRANSFER_FLAG_ZERO_PACK  0x01           /**< (For bulk OUT only). Indicates that a bulk OUT transfers should always terminate with a short packet, even if it means adding an extra zero length packet */

#ifdef __cplusplus
}
#endif
