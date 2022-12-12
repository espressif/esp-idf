/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/queue.h>
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------------------------------ Types --------------------------------------------------------

typedef struct {
    uint8_t *data_buffer;
    size_t data_buffer_size;
    int num_bytes;
    int actual_num_bytes;
    uint32_t flags;
    usb_device_handle_t device_handle;
    uint8_t bEndpointAddress;
    usb_transfer_status_t status;
    uint32_t timeout;
    usb_transfer_cb_t callback;
    void *context;
    int num_isoc_packets;
    usb_isoc_packet_desc_t isoc_packet_desc[0];
} usb_transfer_dummy_t;
_Static_assert(sizeof(usb_transfer_dummy_t) == sizeof(usb_transfer_t), "usb_transfer_dummy_t does not match usb_transfer_t");

struct urb_s{
    TAILQ_ENTRY(urb_s) tailq_entry;
    //HCD Layer: Handler pointer and variables. Must be initialized to NULL and 0 respectively
    void *hcd_ptr;
    uint32_t hcd_var;
    //Host Lib Layer:
    void *usb_host_client;  //Currently only used when submitted to shared pipes (i.e., Device default pipes)
    size_t usb_host_header_size; //USB Host may need the data buffer to have a transparent header
    bool usb_host_inflight; //Debugging variable, used to prevent re-submitting URBs already inflight
    //Public transfer structure. Must be last due to variable length array
    usb_transfer_t transfer;
};
typedef struct urb_s urb_t;

typedef enum {
    USB_NOTIF_SOURCE_USBH = 0x01,
    USB_NOTIF_SOURCE_HUB = 0x02,
} usb_notif_source_t;

typedef bool (*usb_notif_cb_t)(usb_notif_source_t source, bool in_isr, void *context);

// --------------------------------------------------- Allocation ------------------------------------------------------

/**
 * @brief Allocate a URB
 *
 * - Data buffer is allocated in DMA capable memory
 * - The constant fields of the URB are also set
 * - The data_buffer field of the URB is set to point to start of the allocated data buffer AFTER the header. To access
 *   the header, users need a negative offset from data_buffer.
 *
 * @param data_buffer_size Size of the URB's data buffer
 * @param header_size Size of header to put in front of URB's data buffer
 * @param num_isoc_packets Number of isochronous packet descriptors
 * @return urb_t* URB object
 */
urb_t *urb_alloc(size_t data_buffer_size, size_t header_size, int num_isoc_packets);

/**
 * @brief Free a URB
 *
 * @param urb URB object
 */
void urb_free(urb_t *urb);

#ifdef __cplusplus
}
#endif
