/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "esp_heap_caps.h"
#include "esp_check.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"
#include "esp_cam_ctlr_dvp_dma.h"
#include "esp_memory_utils.h"

#define ALIGN_UP_BY(num, align)             (((num) + ((align) - 1)) & ~((align) - 1))

#if defined(SOC_GDMA_TRIG_PERIPH_CAM0_BUS) && (SOC_GDMA_TRIG_PERIPH_CAM0_BUS == SOC_GDMA_BUS_AHB)
#define DVP_GDMA_NEW_CHANNEL            gdma_new_ahb_channel
#define DVP_GDMA_DESC_ALLOC_CAPS        (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)
#elif defined(SOC_GDMA_TRIG_PERIPH_CAM0_BUS) && (SOC_GDMA_TRIG_PERIPH_CAM0_BUS == SOC_GDMA_BUS_AXI)
#define DVP_GDMA_NEW_CHANNEL            gdma_new_axi_channel
#if CONFIG_SPIRAM
#define DVP_GDMA_DESC_ALLOC_CAPS        (MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA)
#else
#define DVP_GDMA_DESC_ALLOC_CAPS        (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)
#endif
#else
#error "Unsupported GDMA bus type for DVP"
#endif

static const char *TAG = "dvp_gdma";

/**
 * @brief Configure DMA description
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
static void IRAM_ATTR esp_cam_ctlr_dvp_config_dma_desc(esp_cam_ctlr_dvp_dma_desc_t *desc, uint8_t *buffer, uint32_t size)
{
    size_t n = 0;

    while (size) {
        uint32_t node_size = MIN(size, ESP_CAM_CTLR_DVP_DMA_DESC_BUFFER_MAX_SIZE);

        desc[n].dw0.size = node_size;
        desc[n].dw0.length = 0;
        desc[n].dw0.err_eof = 0;
        desc[n].dw0.suc_eof = 0;
        desc[n].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc[n].buffer = (uint8_t *)buffer;
        desc[n].next = &desc[n + 1];

        size -= node_size;
        buffer += node_size;
        n++;
    }

    if (n > 0) {
        desc[n - 1].next = NULL;
    }
}

/**
 * @brief Initialize DVP DMA object
 *
 * @param dma  DVP DMA object pointer
 * @param burst_size DVP DMA burst transmission block size
 * @param size DVP DMA buffer size
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_init(esp_cam_ctlr_dvp_dma_t *dma, uint32_t burst_size, size_t size)
{
    esp_err_t ret = ESP_OK;
    size_t alignment_size;

    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
#if CONFIG_CAM_CTLR_DVP_CAM_ISR_CACHE_SAFE
        .flags.isr_cache_safe = true,
#endif
    };

    ESP_RETURN_ON_ERROR(esp_cache_get_alignment(DVP_GDMA_DESC_ALLOC_CAPS, &alignment_size), TAG, "failed to get cache alignment");

    ESP_RETURN_ON_ERROR(DVP_GDMA_NEW_CHANNEL(&rx_alloc_config, &dma->dma_chan), TAG, "new channel failed");

    ESP_GOTO_ON_ERROR(gdma_connect(dma->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_CAM, 0)), fail0, TAG, "connect failed");

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = false,
        .owner_check = true
    };

    ESP_GOTO_ON_ERROR(gdma_apply_strategy(dma->dma_chan, &strategy_config), fail1, TAG, "apply strategy failed");
    // set DMA transfer ability
    gdma_transfer_config_t transfer_config = {
        .max_data_burst_size = burst_size,
        .access_ext_mem = true,
    };
    ESP_GOTO_ON_ERROR(gdma_config_transfer(dma->dma_chan, &transfer_config), fail1, TAG, "set trans ability failed");

    gdma_get_alignment_constraints(dma->dma_chan, &dma->int_mem_align, &dma->ext_mem_align);

    dma->desc_count = size / ESP_CAM_CTLR_DVP_DMA_DESC_BUFFER_MAX_SIZE;
    if (size % ESP_CAM_CTLR_DVP_DMA_DESC_BUFFER_MAX_SIZE) {
        dma->desc_count++;
    }
    dma->size = size;
    alignment_size = (alignment_size == 0) ? 1 : alignment_size;
    dma->desc_size = ALIGN_UP_BY(dma->desc_count * sizeof(esp_cam_ctlr_dvp_dma_desc_t), alignment_size);

    ESP_LOGD(TAG, "alignment_size: %d, dma->desc_count: %d, dma->desc_size: %d", alignment_size, dma->desc_count, dma->desc_size);
    dma->desc = heap_caps_aligned_alloc(alignment_size, dma->desc_size, DVP_GDMA_DESC_ALLOC_CAPS);

    ESP_GOTO_ON_FALSE(dma->desc, ESP_ERR_NO_MEM, fail1, TAG, "no mem for DVP DMA descriptor");

    return ESP_OK;

fail1:
    gdma_disconnect(dma->dma_chan);
fail0:
    gdma_del_channel(dma->dma_chan);
    return ret;
}

/**
 * @brief De-initialize DVP DMA object
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_deinit(esp_cam_ctlr_dvp_dma_t *dma)
{
    ESP_RETURN_ON_ERROR(gdma_disconnect(dma->dma_chan), TAG, "disconnect dma channel failed");
    ESP_RETURN_ON_ERROR(gdma_del_channel(dma->dma_chan), TAG, "delete dma channel failed");

    heap_caps_free(dma->desc);

    return ESP_OK;
}

/**
 * @brief Set DVP DMA descriptor address and start engine
 *
 * @param dma    DVP DMA object pointer
 * @param buffer DVP DMA buffer pointer
 * @param size   DVP DMA buffer size
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t IRAM_ATTR esp_cam_ctlr_dvp_dma_start(esp_cam_ctlr_dvp_dma_t *dma, uint8_t *buffer, size_t size)
{
    ESP_RETURN_ON_FALSE_ISR(dma, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE_ISR(dma->size >= size, ESP_ERR_INVALID_ARG, TAG, "input buffer size is out of range");

    esp_cam_ctlr_dvp_config_dma_desc(dma->desc, buffer, size);

    if (esp_ptr_external_ram(dma->desc)) {
        esp_err_t ret = esp_cache_msync(dma->desc, dma->desc_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
        assert(ret == ESP_OK);
        (void)ret;
    }

    return gdma_start(dma->dma_chan, (intptr_t)dma->desc);
}

/**
 * @brief Stop DVP DMA engine
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t IRAM_ATTR esp_cam_ctlr_dvp_dma_stop(esp_cam_ctlr_dvp_dma_t *dma)
{
    return gdma_stop(dma->dma_chan);
}

/**
 * @brief Reset DVP DMA FIFO and internal finite state machine
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t IRAM_ATTR esp_cam_ctlr_dvp_dma_reset(esp_cam_ctlr_dvp_dma_t *dma)
{
    return gdma_reset(dma->dma_chan);
}

/**
 * @brief Get DMA received data size
 *
 * @param dma DVP DMA object pointer
 *
 * @return DMA received data size
 */
size_t IRAM_ATTR esp_cam_ctlr_dvp_dma_get_recv_size(esp_cam_ctlr_dvp_dma_t *dma)
{
    size_t recv_size = 0;

    for (int i = 0; i < dma->desc_count; i++) {
        recv_size += dma->desc[i].dw0.length;
        if (dma->desc[i].dw0.suc_eof) {
            break;
        }
    }

    return recv_size;
}
