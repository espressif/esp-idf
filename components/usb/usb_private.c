/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_heap_caps.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"

urb_t *urb_alloc(size_t data_buffer_size, int num_isoc_packets)
{
    urb_t *urb = heap_caps_calloc(1, sizeof(urb_t) + (sizeof(usb_isoc_packet_desc_t) * num_isoc_packets), MALLOC_CAP_DEFAULT);
    void *data_buffer = heap_caps_malloc(data_buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_CACHE_ALIGNED);
    if (urb == NULL || data_buffer == NULL)     {
        goto err;
    }
    // Cast as dummy transfer so that we can assign to const fields
    usb_transfer_dummy_t *dummy_transfer = (usb_transfer_dummy_t *)&urb->transfer;
    dummy_transfer->data_buffer = data_buffer;
    dummy_transfer->data_buffer_size = heap_caps_get_allocated_size(data_buffer);
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
    heap_caps_free(urb->transfer.data_buffer);
    heap_caps_free(urb);
}
