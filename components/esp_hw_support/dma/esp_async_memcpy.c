/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hal/dma_types.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_async_memcpy.h"
#include "esp_async_memcpy_impl.h"

static const char *TAG = "async_memcpy";

#define ALIGN_DOWN(val, align)  ((val) & ~((align) - 1))

/**
 * @brief Type of async mcp stream
 *        mcp stream inherits DMA descriptor, besides that, it has a callback function member
 */
typedef struct {
    dma_descriptor_t desc;
    async_memcpy_isr_cb_t cb;
    void *cb_args;
} async_memcpy_stream_t;

/**
 * @brief Type of async mcp driver context
 */
typedef struct async_memcpy_context_t {
    async_memcpy_impl_t mcp_impl; // implementation layer
    portMUX_TYPE spinlock;     // spinlock, prevent operating descriptors concurrently
    intr_handle_t intr_hdl;    // interrupt handle
    uint32_t flags;            // extra driver flags
    dma_descriptor_t *tx_desc; // pointer to the next free TX descriptor
    dma_descriptor_t *rx_desc; // pointer to the next free RX descriptor
    dma_descriptor_t *next_rx_desc_to_check; // pointer to the next RX descriptor to recycle
    uint32_t max_stream_num;    // maximum number of streams
    size_t max_dma_buffer_size; // maximum DMA buffer size
    async_memcpy_stream_t *out_streams;    // pointer to the first TX stream
    async_memcpy_stream_t *in_streams;     // pointer to the first RX stream
    async_memcpy_stream_t streams_pool[0]; // stream pool (TX + RX), the size is configured during driver installation
} async_memcpy_context_t;

esp_err_t esp_async_memcpy_install(const async_memcpy_config_t *config, async_memcpy_t *asmcp)
{
    esp_err_t ret = ESP_OK;
    async_memcpy_context_t *mcp_hdl = NULL;

    ESP_GOTO_ON_FALSE(config, ESP_ERR_INVALID_ARG, err, TAG, "configuration can't be null");
    ESP_GOTO_ON_FALSE(asmcp, ESP_ERR_INVALID_ARG, err, TAG, "can't assign mcp handle to null");

    // context memory size + stream pool size
    size_t total_malloc_size = sizeof(async_memcpy_context_t) + sizeof(async_memcpy_stream_t) * config->backlog * 2;
    // to work when cache is disabled, the driver handle should located in SRAM
    mcp_hdl = heap_caps_calloc(1, total_malloc_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    ESP_GOTO_ON_FALSE(mcp_hdl, ESP_ERR_NO_MEM, err, TAG, "allocate context memory failed");

    mcp_hdl->flags = config->flags;
    mcp_hdl->out_streams = mcp_hdl->streams_pool;
    mcp_hdl->in_streams = mcp_hdl->streams_pool + config->backlog;
    mcp_hdl->max_stream_num = config->backlog;

    // circle TX/RX descriptors
    for (size_t i = 0; i < mcp_hdl->max_stream_num; i++) {
        mcp_hdl->out_streams[i].desc.dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        mcp_hdl->out_streams[i].desc.next = &mcp_hdl->out_streams[i + 1].desc;
        mcp_hdl->in_streams[i].desc.dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        mcp_hdl->in_streams[i].desc.next = &mcp_hdl->in_streams[i + 1].desc;
    }
    mcp_hdl->out_streams[mcp_hdl->max_stream_num - 1].desc.next = &mcp_hdl->out_streams[0].desc;
    mcp_hdl->in_streams[mcp_hdl->max_stream_num - 1].desc.next = &mcp_hdl->in_streams[0].desc;

    mcp_hdl->tx_desc = &mcp_hdl->out_streams[0].desc;
    mcp_hdl->rx_desc = &mcp_hdl->in_streams[0].desc;
    mcp_hdl->next_rx_desc_to_check = &mcp_hdl->in_streams[0].desc;
    mcp_hdl->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    mcp_hdl->mcp_impl.sram_trans_align = config->sram_trans_align;
    mcp_hdl->mcp_impl.psram_trans_align = config->psram_trans_align;
    size_t trans_align = MAX(config->sram_trans_align, config->psram_trans_align);
    mcp_hdl->max_dma_buffer_size = trans_align ? ALIGN_DOWN(DMA_DESCRIPTOR_BUFFER_MAX_SIZE, trans_align) : DMA_DESCRIPTOR_BUFFER_MAX_SIZE;

    // initialize implementation layer
    ret = async_memcpy_impl_init(&mcp_hdl->mcp_impl);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "DMA M2M init failed");

    ESP_LOGD(TAG, "installed memory to memory copy channel at %p", mcp_hdl);

    *asmcp = mcp_hdl;

    async_memcpy_impl_start(&mcp_hdl->mcp_impl, (intptr_t)&mcp_hdl->out_streams[0].desc, (intptr_t)&mcp_hdl->in_streams[0].desc);

    return ESP_OK;
err:
    if (mcp_hdl) {
        free(mcp_hdl);
    }
    if (asmcp) {
        *asmcp = NULL;
    }
    return ret;
}

esp_err_t esp_async_memcpy_uninstall(async_memcpy_t asmcp)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(asmcp, ESP_ERR_INVALID_ARG, err, TAG, "mcp handle can't be null");

    async_memcpy_impl_stop(&asmcp->mcp_impl);
    async_memcpy_impl_deinit(&asmcp->mcp_impl);
    free(asmcp);
err:
    return ret;
}

esp_err_t esp_async_memcpy_new_etm_event(async_memcpy_t asmcp, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event)
{
    ESP_RETURN_ON_FALSE(asmcp, ESP_ERR_INVALID_ARG, TAG, "mcp handle can't be null");
    return async_memcpy_impl_new_etm_event(&asmcp->mcp_impl, event_type, out_event);
}

static int async_memcpy_prepare_receive(async_memcpy_t asmcp, void *buffer, size_t size, dma_descriptor_t **start_desc, dma_descriptor_t **end_desc)
{
    uint32_t prepared_length = 0;
    uint8_t *buf = (uint8_t *)buffer;
    dma_descriptor_t *desc = asmcp->rx_desc; // descriptor iterator
    dma_descriptor_t *start = desc;
    dma_descriptor_t *end = desc;

    while (size > asmcp->max_dma_buffer_size) {
        if (desc->dw0.owner != DMA_DESCRIPTOR_BUFFER_OWNER_DMA) {
            desc->dw0.suc_eof = 0;
            desc->dw0.size = asmcp->max_dma_buffer_size;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += asmcp->max_dma_buffer_size;
            size -= asmcp->max_dma_buffer_size;
        } else {
            // out of RX descriptors
            goto _exit;
        }
    }
    if (size) {
        if (desc->dw0.owner != DMA_DESCRIPTOR_BUFFER_OWNER_DMA) {
            end = desc; // the last descriptor used
            desc->dw0.suc_eof = 0;
            desc->dw0.size = size;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += size;
        } else {
            // out of RX descriptors
            goto _exit;
        }
    }

_exit:
    *start_desc = start;
    *end_desc = end;
    return prepared_length;
}

static int async_memcpy_prepare_transmit(async_memcpy_t asmcp, void *buffer, size_t len, dma_descriptor_t **start_desc, dma_descriptor_t **end_desc)
{
    uint32_t prepared_length = 0;
    uint8_t *buf = (uint8_t *)buffer;
    dma_descriptor_t *desc = asmcp->tx_desc; // descriptor iterator
    dma_descriptor_t *start = desc;
    dma_descriptor_t *end = desc;

    while (len > asmcp->max_dma_buffer_size) {
        if (desc->dw0.owner != DMA_DESCRIPTOR_BUFFER_OWNER_DMA) {
            desc->dw0.suc_eof = 0; // not the end of the transaction
            desc->dw0.size = asmcp->max_dma_buffer_size;
            desc->dw0.length = asmcp->max_dma_buffer_size;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += asmcp->max_dma_buffer_size;
            len -= asmcp->max_dma_buffer_size;
        } else {
            // out of TX descriptors
            goto _exit;
        }
    }
    if (len) {
        if (desc->dw0.owner != DMA_DESCRIPTOR_BUFFER_OWNER_DMA) {
            end = desc;            // the last descriptor used
            desc->dw0.suc_eof = 1; // end of the transaction
            desc->dw0.size = len;
            desc->dw0.length = len;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += len;
        } else {
            // out of TX descriptors
            goto _exit;
        }
    }

    *start_desc = start;
    *end_desc = end;
_exit:
    return prepared_length;
}

static bool async_memcpy_get_next_rx_descriptor(async_memcpy_t asmcp, dma_descriptor_t *eof_desc, dma_descriptor_t **next_desc)
{
    dma_descriptor_t *next = asmcp->next_rx_desc_to_check;
    // additional check, to avoid potential interrupt got triggered by mistake
    if (next->dw0.owner == DMA_DESCRIPTOR_BUFFER_OWNER_CPU) {
        asmcp->next_rx_desc_to_check = asmcp->next_rx_desc_to_check->next;
        *next_desc = next;
        // return if we need to continue
        return eof_desc == next ? false : true;
    }

    *next_desc = NULL;
    return false;
}

esp_err_t esp_async_memcpy(async_memcpy_t asmcp, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args)
{
    esp_err_t ret = ESP_OK;
    dma_descriptor_t *rx_start_desc = NULL;
    dma_descriptor_t *rx_end_desc = NULL;
    dma_descriptor_t *tx_start_desc = NULL;
    dma_descriptor_t *tx_end_desc = NULL;
    size_t rx_prepared_size = 0;
    size_t tx_prepared_size = 0;
    ESP_GOTO_ON_FALSE(asmcp, ESP_ERR_INVALID_ARG, err, TAG, "mcp handle can't be null");
    ESP_GOTO_ON_FALSE(async_memcpy_impl_is_buffer_address_valid(&asmcp->mcp_impl, src, dst), ESP_ERR_INVALID_ARG, err, TAG, "buffer address not valid: %p -> %p", src, dst);
    ESP_GOTO_ON_FALSE(n <= asmcp->max_dma_buffer_size * asmcp->max_stream_num, ESP_ERR_INVALID_ARG, err, TAG, "buffer size too large");
    if (asmcp->mcp_impl.sram_trans_align) {
        ESP_GOTO_ON_FALSE(((n & (asmcp->mcp_impl.sram_trans_align - 1)) == 0), ESP_ERR_INVALID_ARG, err, TAG, "copy size should align to %d bytes", asmcp->mcp_impl.sram_trans_align);
    }
    if (asmcp->mcp_impl.psram_trans_align) {
        ESP_GOTO_ON_FALSE(((n & (asmcp->mcp_impl.psram_trans_align - 1)) == 0), ESP_ERR_INVALID_ARG, err, TAG, "copy size should align to %d bytes", asmcp->mcp_impl.psram_trans_align);
    }

    // Prepare TX and RX descriptor
    portENTER_CRITICAL_SAFE(&asmcp->spinlock);
    rx_prepared_size = async_memcpy_prepare_receive(asmcp, dst, n, &rx_start_desc, &rx_end_desc);
    tx_prepared_size = async_memcpy_prepare_transmit(asmcp, src, n, &tx_start_desc, &tx_end_desc);
    if (rx_start_desc && tx_start_desc && (rx_prepared_size == n) && (tx_prepared_size == n)) {
        // register user callback to the last descriptor
        async_memcpy_stream_t *mcp_stream = __containerof(rx_end_desc, async_memcpy_stream_t, desc);
        mcp_stream->cb = cb_isr;
        mcp_stream->cb_args = cb_args;
        // restart RX firstly
        dma_descriptor_t *desc = rx_start_desc;
        while (desc != rx_end_desc) {
            desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
            desc = desc->next;
        }
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        asmcp->rx_desc = desc->next;
        // restart TX secondly
        desc = tx_start_desc;
        while (desc != tx_end_desc) {
            desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
            desc = desc->next;
        }
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        asmcp->tx_desc = desc->next;
        async_memcpy_impl_restart(&asmcp->mcp_impl);
    }
    portEXIT_CRITICAL_SAFE(&asmcp->spinlock);

    // It's unlikely that we have space for rx descriptor but no space for tx descriptor
    // Both tx and rx descriptor should move in the same pace
    ESP_GOTO_ON_FALSE(rx_prepared_size == n, ESP_FAIL, err, TAG, "out of rx descriptor");
    ESP_GOTO_ON_FALSE(tx_prepared_size == n, ESP_FAIL, err, TAG, "out of tx descriptor");

err:
    return ret;
}

IRAM_ATTR void async_memcpy_isr_on_rx_done_event(async_memcpy_impl_t *impl)
{
    bool to_continue = false;
    async_memcpy_stream_t *in_stream = NULL;
    dma_descriptor_t *next_desc = NULL;
    async_memcpy_context_t *asmcp = __containerof(impl, async_memcpy_context_t, mcp_impl);

    // get the RX eof descriptor address
    dma_descriptor_t *eof = (dma_descriptor_t *)impl->rx_eof_addr;
    // traversal all unchecked descriptors
    do {
        portENTER_CRITICAL_ISR(&asmcp->spinlock);
        // There is an assumption that the usage of rx descriptors are in the same pace as tx descriptors (this is determined by M2M DMA working mechanism)
        // And once the rx descriptor is recycled, the corresponding tx desc is guaranteed to be returned by DMA
        to_continue = async_memcpy_get_next_rx_descriptor(asmcp, eof, &next_desc);
        portEXIT_CRITICAL_ISR(&asmcp->spinlock);
        if (next_desc) {
            in_stream = __containerof(next_desc, async_memcpy_stream_t, desc);
            // invoke user registered callback if available
            if (in_stream->cb) {
                async_memcpy_event_t e = {0};
                if (in_stream->cb(asmcp, &e, in_stream->cb_args)) {
                    impl->isr_need_yield = true;
                }
                in_stream->cb = NULL;
                in_stream->cb_args = NULL;
            }
        }
    } while (to_continue);
}
