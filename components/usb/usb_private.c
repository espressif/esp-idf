/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"

// ----------------------------------------------------- Macros --------------------------------------------------------

#if !CONFIG_IDF_TARGET_LINUX
#include "esp_private/esp_cache_private.h"
#define ALIGN_UP(num, align)    ((align) == 0 ? (num) : (((num) + ((align) - 1)) & ~((align) - 1)))
#endif

// ----------------------- Configs -------------------------

#ifdef CONFIG_USB_HOST_DWC_DMA_CAP_MEMORY_IN_PSRAM      // In esp32p4, the USB-DWC internal DMA can access external RAM
#define DATA_BUFFER_CAPS                     (MALLOC_CAP_DMA | MALLOC_CAP_CACHE_ALIGNED | MALLOC_CAP_SPIRAM)
#else
#define DATA_BUFFER_CAPS                     (MALLOC_CAP_DMA | MALLOC_CAP_CACHE_ALIGNED | MALLOC_CAP_INTERNAL)
#endif

urb_t *urb_alloc(size_t data_buffer_size, int num_isoc_packets)
{
    urb_t *urb = heap_caps_calloc(1, sizeof(urb_t) + (sizeof(usb_isoc_packet_desc_t) * num_isoc_packets), MALLOC_CAP_DEFAULT);

#if !CONFIG_IDF_TARGET_LINUX
    // Note for developers: We do not use heap_caps_get_allocated_size() because it is broken with HEAP_POISONING=COMPREHENSIVE
    size_t cache_align = 0;
    esp_cache_get_alignment(DATA_BUFFER_CAPS, &cache_align);
    data_buffer_size = ALIGN_UP(data_buffer_size, cache_align);
#endif

    void *data_buffer = heap_caps_malloc(data_buffer_size, DATA_BUFFER_CAPS);
    if (urb == NULL || data_buffer == NULL)     {
        goto err;
    }

    // Cast as dummy transfer so that we can assign to const fields
    usb_transfer_dummy_t *dummy_transfer = (usb_transfer_dummy_t *)&urb->transfer;
    dummy_transfer->data_buffer = data_buffer;
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
    heap_caps_free(urb->transfer.data_buffer);
    heap_caps_free(urb);
}
