/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "../dma/gdma_priv.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

static const char *TAG = "gdma";

esp_err_t gdma_set_transfer_ability(gdma_channel_handle_t dma_chan, const gdma_transfer_ability_t *ability)
{
    ESP_RETURN_ON_FALSE(dma_chan && ability, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    size_t int_mem_alignment = ability->sram_trans_align;
    size_t ext_mem_alignment = ability->psram_trans_align;
    // alignment should be 2^n
    ESP_RETURN_ON_FALSE((int_mem_alignment & (int_mem_alignment - 1)) == 0, ESP_ERR_INVALID_ARG,
                        TAG, "invalid sram alignment: %zu", int_mem_alignment);

    uint32_t ext_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    if (ext_mem_alignment == 0) {
        // fall back to use the same size of the psram data cache line size
        ext_mem_alignment = ext_mem_cache_line_size;
    }
    if ((ext_mem_cache_line_size > 0) && (ext_mem_alignment > ext_mem_cache_line_size)) {
        ESP_RETURN_ON_FALSE(((ext_mem_alignment % ext_mem_cache_line_size) == 0), ESP_ERR_INVALID_ARG,
                            TAG, "ext_mem_alignment(%d) should be multiple of the ext_mem_cache_line_size(%"PRIu32")",
                            ext_mem_alignment, ext_mem_cache_line_size);
    }

    // if the DMA can't access the PSRAM, this HAL function is no-op
    gdma_hal_set_burst_size(hal, pair->pair_id, dma_chan->direction, ext_mem_alignment);

    // TX channel can always enable burst mode, no matter data alignment
    bool en_burst = true;
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        // RX channel burst mode depends on specific data alignment
        en_burst = int_mem_alignment >= 4;
    }
    gdma_hal_enable_burst(hal, pair->pair_id, dma_chan->direction, en_burst, en_burst);

    dma_chan->int_mem_alignment = int_mem_alignment;
    dma_chan->ext_mem_alignment = ext_mem_alignment;
    ESP_LOGD(TAG, "%s channel (%d,%d), (%u:%u) bytes aligned, burst %s", dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX ? "tx" : "rx",
             group->group_id, pair->pair_id, int_mem_alignment, ext_mem_alignment, en_burst ? "enabled" : "disabled");

    return ESP_OK;
}
