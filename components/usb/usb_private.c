/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_heap_caps.h"
#include "esp_dma_utils.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"

urb_t *urb_alloc(size_t data_buffer_size, int num_isoc_packets)
{
    urb_t *urb = heap_caps_calloc(1, sizeof(urb_t) + (sizeof(usb_isoc_packet_desc_t) * num_isoc_packets), MALLOC_CAP_DEFAULT);
    void *data_buffer;
    size_t real_size;
    esp_dma_mem_info_t dma_mem_info = {
        .dma_alignment_bytes = 4,
    };
    //TODO: IDF-9639
    esp_dma_capable_malloc(data_buffer_size, &dma_mem_info, &data_buffer, &real_size);
    if (urb == NULL || data_buffer == NULL)     {
        goto err;
    }
    // Cast as dummy transfer so that we can assign to const fields
    usb_transfer_dummy_t *dummy_transfer = (usb_transfer_dummy_t *)&urb->transfer;
    dummy_transfer->data_buffer = data_buffer;
    dummy_transfer->data_buffer_size = real_size;
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
