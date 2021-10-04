/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_heap_caps.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"

urb_t *urb_alloc(size_t data_buffer_size, size_t header_size, int num_isoc_packets)
{
    urb_t *urb = heap_caps_calloc(1, sizeof(urb_t) + (sizeof(usb_isoc_packet_desc_t) * num_isoc_packets), MALLOC_CAP_DEFAULT);
    uint8_t *data_buffer = heap_caps_malloc(data_buffer_size + header_size, MALLOC_CAP_DMA);
    if (urb == NULL || data_buffer == NULL)     {
        goto err;
    }
    urb->usb_host_header_size = header_size;    //Indicate that this URB's data_buffer has a header in front of it.
    //Case as dummy transfer to write to initialize const fields
    usb_transfer_dummy_t *dummy_transfer = (usb_transfer_dummy_t *)&urb->transfer;
    dummy_transfer->data_buffer = (uint8_t *)(data_buffer + header_size);
    dummy_transfer->data_buffer_size = data_buffer_size;
    dummy_transfer->num_isoc_packets = num_isoc_packets;
    return urb;
err:
    heap_caps_free(urb);
    heap_caps_free(data_buffer);
    return NULL;
}

void urb_free(urb_t *urb)
{
    if (urb == NULL) {
        return;
    }
    heap_caps_free((uint8_t *)(urb->transfer.data_buffer - urb->usb_host_header_size));
    heap_caps_free(urb);
}
