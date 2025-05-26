/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/bitscrambler.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/bitscrambler.h"
#include "hal/dma_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "bitscrambler_private.h"
#include "bitscrambler_soc_specific.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"

const static char *TAG = "bs_loop";

typedef struct {
    bitscrambler_t bs;
    gdma_channel_handle_t tx_channel; // GDMA TX channel handle
    gdma_channel_handle_t rx_channel; // GDMA RX channel handle
    gdma_link_list_handle_t tx_link_list; // GDMA TX link list handle, the length of the link is determined by the copy buffer size
    gdma_link_list_handle_t rx_link_list; // GDMA RX link list handle, the length of the link is determined by the copy buffer size
    SemaphoreHandle_t sema_done;
    size_t max_transfer_sz_bytes;
} bitscrambler_loopback_t;

/// @brief make sure bs is indeed the first member of bitscrambler_loopback_t so we can cast it to a bitscrambler_t and safely passed to
//         any of the non-loopback bitscrambler functions.
_Static_assert(offsetof(bitscrambler_loopback_t, bs) == 0, "bs needs to be 1st member of bitscrambler_loopback_t");

static void bitscrambler_loopback_free(bitscrambler_loopback_t *bsl);
static esp_err_t bitscrambler_loopback_cleanup(bitscrambler_handle_t bs, void* user_ctx);

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

    // register extra cleanup function to free loopback resources
    bitscrambler_register_extra_clean_up(&bs->bs, bitscrambler_loopback_cleanup, bs);

    bs->sema_done = xSemaphoreCreateBinary();
    if (!bs->sema_done) {
        goto err;
    }

    bs->max_transfer_sz_bytes = max_transfer_sz_bytes;
    size_t desc_ct = esp_dma_calculate_node_count(max_transfer_sz_bytes, 4, DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
    int bus = g_bitscrambler_periph_desc[attach_to].bus;
#ifdef SOC_GDMA_BUS_AXI
    size_t align = (bus == SOC_GDMA_BUS_AXI) ? 8 : 4;
#else
    size_t align = 4;
#endif

    // create DMA link list for TX and RX
    gdma_link_list_config_t dma_link_cfg = {
        .buffer_alignment = 4,
        .item_alignment = align,
        .num_items = desc_ct,
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&dma_link_cfg, &bs->tx_link_list), err, TAG, "failed to create TX link list");
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&dma_link_cfg, &bs->rx_link_list), err, TAG, "failed to create RX link list");

    // create TX channel and RX channel, they should reside in the same DMA pair
    gdma_channel_alloc_config_t tx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
        .flags.reserve_sibling = 1,
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
        .eof_till_data_popped = true,
    };
    gdma_apply_strategy(bs->rx_channel, &gdma_strategy_conf);
    gdma_apply_strategy(bs->tx_channel, &gdma_strategy_conf);

    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = trans_done_cb,
    };
    gdma_register_rx_event_callbacks(bs->rx_channel, &rx_cbs, bs);
    ESP_GOTO_ON_ERROR(bitscrambler_enable(&bs->bs), err, TAG, "failed to enable bitscrambler");
    *handle = (bitscrambler_handle_t)bs;
    return ESP_OK;

err:
    bitscrambler_loopback_free(bs);
    free(bs);
    return ret;
}

static void bitscrambler_loopback_free(bitscrambler_loopback_t *bsl)
{
    if (bsl->rx_channel) {
        gdma_disconnect(bsl->rx_channel);
        gdma_del_channel(bsl->rx_channel);
    }
    if (bsl->tx_channel) {
        gdma_disconnect(bsl->tx_channel);
        gdma_del_channel(bsl->tx_channel);
    }
    if (bsl->tx_link_list) {
        gdma_del_link_list(bsl->tx_link_list);
    }
    if (bsl->rx_link_list) {
        gdma_del_link_list(bsl->rx_link_list);
    }
    if (bsl->sema_done) {
        vSemaphoreDelete(bsl->sema_done);
    }
}

static esp_err_t bitscrambler_loopback_cleanup(bitscrambler_handle_t bs, void* user_ctx)
{
    bitscrambler_loopback_t *bsl = (bitscrambler_loopback_t*)user_ctx;
    ESP_RETURN_ON_ERROR(bitscrambler_disable(bs), TAG, "failed to disable bitscrambler");
    bitscrambler_loopback_free(bsl);
    return ESP_OK;
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

    int int_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    int ext_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);

    bool need_cache_sync = esp_ptr_internal(buffer_in) ? (int_mem_cache_line_size > 0) : (ext_mem_cache_line_size > 0);
    if (need_cache_sync) {
        //Note: we add the ESP_CACHE_MSYNC_FLAG_UNALIGNED flag for now as otherwise esp_cache_msync will complain about
        //the size not being aligned... we miss out on a check to see if the address is aligned this way. This needs to
        //be improved, but potentially needs a fix in esp_cache_msync not to check the size.
        ESP_RETURN_ON_ERROR(esp_cache_msync(buffer_in, length_bytes_in, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED),
                            TAG, "failed in cache sync for input buffer");
    }
    need_cache_sync = esp_ptr_internal(buffer_out) ? (int_mem_cache_line_size > 0) : (ext_mem_cache_line_size > 0);
    if (need_cache_sync) {
        ESP_RETURN_ON_ERROR(esp_cache_msync(buffer_out, length_bytes_out, ESP_CACHE_MSYNC_FLAG_DIR_M2C),
                            TAG, "failed in cache sync for output buffer");
    }

    gdma_reset(bsl->rx_channel);
    gdma_reset(bsl->tx_channel);
    bitscrambler_reset(bs);

    // mount in and out buffer to the DMA link list
    gdma_buffer_mount_config_t in_buf_mount_config = {
        .buffer = buffer_in,
        .length = length_bytes_in,
        .flags = {
            .mark_eof = true,
            .mark_final = true,
        }
    };
    gdma_link_mount_buffers(bsl->tx_link_list, 0, &in_buf_mount_config, 1, NULL);
    gdma_buffer_mount_config_t out_buf_mount_config = {
        .buffer = buffer_out,
        .length = length_bytes_out,
        .flags = {
            .mark_eof = false,
            .mark_final = true,
        }
    };
    gdma_link_mount_buffers(bsl->rx_link_list, 0, &out_buf_mount_config, 1, NULL);

    gdma_start(bsl->rx_channel, gdma_link_get_head_addr(bsl->rx_link_list));
    gdma_start(bsl->tx_channel, gdma_link_get_head_addr(bsl->tx_link_list));
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

    if (bytes_written) {
        *bytes_written = gdma_link_count_buffer_size_till_eof(bsl->rx_link_list, 0);
    }

    return ret;
}
