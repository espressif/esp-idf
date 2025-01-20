/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "driver/bitscrambler.h"
#include "bitscrambler_private.h"
#include "bitscrambler_loopback_private.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "hal/cache_ll.h"
#include "hal/gdma_ll.h"
#include "bitscrambler_soc_specific.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_check.h"
#include "soc/ahb_dma_struct.h"
#include "esp_heap_caps.h"
#include "esp_cache.h"
#include "esp_dma_utils.h"

const static char *TAG = "bs_loop";

//Note: given that the first member is a bitscrambler_t, this can be safely passed to
//any of the non-loopback bitscrambler functions.
typedef struct {
    bitscrambler_t bs;
    dma_descriptor_t *tx_desc_link; // descriptor link list, the length of the link is determined by the copy buffer size
    dma_descriptor_t *rx_desc_link; // descriptor link list, the length of the link is determined by the copy buffer size
    gdma_channel_handle_t tx_channel; // GDMA TX channel handle
    gdma_channel_handle_t rx_channel; // GDMA RX channel handle
    SemaphoreHandle_t sema_done;
    size_t max_transfer_sz_bytes;
} bitscrambler_loopback_t;

static esp_err_t new_dma_channel(const gdma_channel_alloc_config_t *cfg, gdma_channel_handle_t *handle, int bus)
{
    esp_err_t ret = ESP_OK;
    //Note that there are chips that do not have SOC_GDMA_BUS_* defined, but those chips also do
    //not have a BitScrambler.
#ifdef SOC_GDMA_BUS_AHB
    if (bus == SOC_GDMA_BUS_AHB || bus == SOC_GDMA_BUS_ANY) {
        ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(cfg, handle), TAG, "alloc AHB DMA channel failed");
    }
#endif
#ifdef SOC_GDMA_BUS_AXI
    if (bus == SOC_GDMA_BUS_AXI) {
        ESP_RETURN_ON_ERROR(gdma_new_axi_channel(cfg, handle), TAG, "alloc AXI DMA channel failed");
    }
#endif
    return ret;
}

static IRAM_ATTR bool trans_done_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    BaseType_t higher_prio_task_awoken = pdFALSE;
    bitscrambler_loopback_t *bs = (bitscrambler_loopback_t*)user_data;
    xSemaphoreGiveFromISR(bs->sema_done, &higher_prio_task_awoken);
    return higher_prio_task_awoken;
}

esp_err_t bitscrambler_loopback_create(bitscrambler_handle_t *handle, int attach_to, size_t max_transfer_sz_bytes)
{
    ///make sure bs is indeed the first member of bitscrambler_loopback_t so we can cast it to a bitscrambler_t
    _Static_assert(offsetof(bitscrambler_loopback_t, bs) == 0, "bs needs to be 1st member of bitscrambler_loopback_t");
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    if (attach_to < 0 || attach_to > SOC_BITSCRAMBLER_ATTACH_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    bitscrambler_loopback_t *bs = calloc(1, sizeof(bitscrambler_loopback_t));
    if (!bs) {
        return ESP_ERR_NO_MEM;
    }

    //Create the underlying BitScrambler object
    bitscrambler_config_t cfg = {
        .dir = BITSCRAMBLER_DIR_TX,
        .attach_to = attach_to
    };
    ESP_GOTO_ON_ERROR(bitscrambler_init_loopback(&bs->bs, &cfg), err, TAG, "failed bitscrambler init for loopback");

    bs->sema_done = xSemaphoreCreateBinary();
    if (!bs->sema_done) {
        goto err;
    }

    bs->max_transfer_sz_bytes = max_transfer_sz_bytes;
    int desc_ct = (max_transfer_sz_bytes + DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
    int bus = g_bitscrambler_periph_desc[attach_to].bus;
    uint32_t caps = (bus == SOC_GDMA_BUS_AXI) ? MALLOC_CAP_DMA_DESC_AXI : MALLOC_CAP_DMA_DESC_AHB;
    size_t align = (bus == SOC_GDMA_BUS_AXI) ? 8 : 4;
    bs->rx_desc_link = heap_caps_aligned_calloc(align, desc_ct, sizeof(dma_descriptor_t), caps);
    bs->tx_desc_link = heap_caps_aligned_calloc(align, desc_ct, sizeof(dma_descriptor_t), caps);
    if (!bs->rx_desc_link || !bs->tx_desc_link) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    // create TX channel and RX channel, they should reside in the same DMA pair
    gdma_channel_alloc_config_t tx_alloc_config = {
        .flags.reserve_sibling = 1,
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_GOTO_ON_ERROR(new_dma_channel(&tx_alloc_config, &bs->tx_channel, bus), err, TAG, "failed to create GDMA TX channel");
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = bs->tx_channel,
    };
    ESP_GOTO_ON_ERROR(new_dma_channel(&rx_alloc_config, &bs->rx_channel, bus), err, TAG, "failed to create GDMA RX channel");

    gdma_connect(bs->rx_channel, g_bitscrambler_periph_desc[attach_to].dma_trigger);
    gdma_connect(bs->tx_channel, g_bitscrambler_periph_desc[attach_to].dma_trigger);
    gdma_strategy_config_t gdma_strategy_conf = {
        .auto_update_desc = true,
        .owner_check = false,
    };
    gdma_apply_strategy(bs->rx_channel, &gdma_strategy_conf);
    gdma_apply_strategy(bs->tx_channel, &gdma_strategy_conf);

    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = trans_done_cb,
    };
    gdma_register_rx_event_callbacks(bs->rx_channel, &rx_cbs, bs);

    *handle = (bitscrambler_handle_t)bs;
    return ESP_OK;

err:
    bitscrambler_loopback_free(&bs->bs);
    free(bs);
    return ret;
}

//note this is never called directly; bitscrambler_free calls this to clear
//the loopback-specific things of a loopback bitscrambler.
//bitscrambler_loopback_create also calls this in an error situation, so
//we should only delete not-NULL members.
void bitscrambler_loopback_free(bitscrambler_handle_t bs)
{
    bitscrambler_loopback_t *bsl = (bitscrambler_loopback_t*)bs;
    if (bsl->rx_channel) {
        gdma_disconnect(bsl->rx_channel);
        gdma_del_channel(bsl->rx_channel);
    }
    if (bsl->tx_channel) {
        gdma_disconnect(bsl->tx_channel);
        gdma_del_channel(bsl->tx_channel);
    }
    if (bsl->sema_done) {
        vSemaphoreDelete(bsl->sema_done);
    }
    free(bsl->rx_desc_link);
    free(bsl->tx_desc_link);
}

static int fill_dma_links(dma_descriptor_t *link, void *buffer, size_t len_bytes, int set_eof)
{
    uint8_t *buffer_p = (uint8_t*)buffer;
    int link_ct = 0;
    for (int p = 0; p < len_bytes; p += DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED) {
        int seg_len = len_bytes - p;
        if (seg_len > DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED) {
            seg_len = DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
        }
        link[link_ct].dw0.size = seg_len;
        link[link_ct].dw0.length = seg_len;
        link[link_ct].dw0.err_eof = 0;
        link[link_ct].dw0.suc_eof = 0;
        link[link_ct].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        link[link_ct].buffer = &buffer_p[p];
        link[link_ct].next = &link[link_ct + 1];
        link_ct++;
    }
    link[link_ct - 1].next = NULL; //fix last entry to end transaction
    if (set_eof) {
        link[link_ct - 1].dw0.suc_eof = 1;
    }
    return link_ct;
}

/*
A BitScrambler program could theoretically take a bunch of time to run, e.g. when transferring from PSRAM to PSRAM.
However, given that this is a memory copy, it feels stupid to have a 'soft' parameter as a timeout; we do need a timeout,
however, as the BitScrambler program may be buggy and e.g. never read or write anything.

As an upper limit for a timeout, we can assume the backing memory is quad psram @ 20MHz, meaning we have a throughput
of around 10MByte/second. Any BitScrambler program is going to be lots faster than that, simply because it doesn't
have the instructions to delay writing by much. Just for safety, we can add an extra factor of 10 to that, making
us assume a minimum throughput of 1MByte/sec.
*/
#define BS_MIN_BYTES_PER_SEC 1000000

/*
We'll also add a few FreeRTOS ticks to the delay, so tiny data transfers won't have an impossibly short timeout.
*/
#define BS_TIMEOUT_BASE_TICKS 3

esp_err_t bitscrambler_loopback_run(bitscrambler_handle_t bs, void *buffer_in, size_t length_bytes_in, void *buffer_out, size_t length_bytes_out, size_t *bytes_written)
{
    //Note that buffer_in and buffer_out are from the perspective of the BitScrambler,
    //however tx/rx are from the perspective of the memory. So buffer_in=tx, buffer_out=rx.
    esp_err_t ret = ESP_OK;
    bitscrambler_loopback_t *bsl = (bitscrambler_loopback_t*)bs;
    if (length_bytes_in > bsl->max_transfer_sz_bytes) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (length_bytes_out > bsl->max_transfer_sz_bytes) {
        return ESP_ERR_INVALID_SIZE;
    }

    //Casual check to see if the buffer is aligned to cache requirements.
    esp_dma_mem_info_t dma_mem_info = {
        .dma_alignment_bytes = 4
    };
    //Note: we know the size of the data, but not of the buffer that contains it, so we set length=0.
    if (!esp_dma_is_buffer_alignment_satisfied(buffer_in, 0, dma_mem_info)) {
        ESP_LOGE(TAG, "buffer_in not aligned to DMA requirements");
        return ESP_ERR_INVALID_ARG;
    }
    if (!esp_dma_is_buffer_alignment_satisfied(buffer_out, 0, dma_mem_info)) {
        ESP_LOGE(TAG, "buffer_out not aligned to DMA requirements");
        return ESP_ERR_INVALID_ARG;
    }

    gdma_reset(bsl->rx_channel);
    gdma_reset(bsl->tx_channel);
    bitscrambler_reset(bs);

    int link_ct_in = fill_dma_links(bsl->tx_desc_link, buffer_in, length_bytes_in, 1);
    int link_ct_out = fill_dma_links(bsl->rx_desc_link, buffer_out, length_bytes_out, 0);

    //Note: we add the ESP_CACHE_MSYNC_FLAG_UNALIGNED flag for now as otherwise esp_cache_msync will complain about
    //the size not being aligned... we miss out on a check to see if the address is aligned this way. This needs to
    //be improved, but potentially needs a fix in esp_cache_msync not to check the size.

    esp_cache_msync(bsl->rx_desc_link, link_ct_out * sizeof(dma_descriptor_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    esp_cache_msync(bsl->tx_desc_link, link_ct_in * sizeof(dma_descriptor_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    esp_cache_msync(buffer_in, length_bytes_in, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    gdma_start(bsl->rx_channel, (intptr_t)bsl->rx_desc_link);
    gdma_start(bsl->tx_channel, (intptr_t)bsl->tx_desc_link);
    bitscrambler_start(bs);

    int timeout_ms = (length_bytes_out + length_bytes_in) / (BS_MIN_BYTES_PER_SEC / 1000);
    int timeout = pdMS_TO_TICKS(timeout_ms) + BS_TIMEOUT_BASE_TICKS;
    if (!xSemaphoreTake(bsl->sema_done, timeout)) {
        gdma_reset(bsl->rx_channel);
        gdma_reset(bsl->tx_channel);
        bitscrambler_reset(bs);
        ESP_LOGE(TAG, "bitscrambler_loopback_run: timed out waiting for BitScrambler program to complete!");
        ret = ESP_ERR_TIMEOUT;
    }

    esp_cache_msync(buffer_out, length_bytes_out, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

    if (bytes_written) {
        size_t l = 0;
        for (int i = 0; i < link_ct_out; i++) {
            l += bsl->rx_desc_link[i].dw0.length;
            if (bsl->rx_desc_link[i].dw0.suc_eof) {
                break;
            }
        }
        *bytes_written = l;
    }

    return ret;
}
