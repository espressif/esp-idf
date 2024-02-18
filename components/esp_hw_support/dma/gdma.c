/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 *                        AHB-Bus  --------+     +-------- AXI-Bus
 *                                         |     |
 *                                         |     |
 *  +-----------------------------------+--+     +--+-----------------------------------+
 *  |            GDMA-Group-X           |  |     |  |            GDMA-Group-Y           |
 *  | +-------------+    +------------+ |  |     |  | +-------------+    +------------+ |
 *  | | GDMA-Pair-0 |... |GDMA-Pair-N | |  |     |  | | GDMA-Pair-0 |... |GDMA-Pair-N | |
 *  | |             |    |            | |  |     |  | |             |    |            | |
 *  | |  TX-Chan    |... | TX-Chan    | |  |     |  | |  TX-Chan    |... | TX-Chan    | |
 *  | |  RX-Chan    |    | RX-Chan    | |  |     |  | |  RX-Chan    |    | RX-Chan    | |
 *  | +-------------+    +------------+ |  |     |  | +-------------+    +------------+ |
 *  |                                   |  |     |  |                                   |
 *  +-----------------------------------+--+     +--+-----------------------------------+
 *                                         |     |
 *                                         |     |
 *
 * - Channel is allocated when user calls `gdma_new_ahb/axi_channel`, its lifecycle is maintained by the user.
 * - Pair and Group are all lazy allocated, their life cycles are maintained by this driver.
 * - We're not using a global spin lock, instead, we created different spin locks at different level (group, pair).
 */

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#if CONFIG_GDMA_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_private/periph_ctrl.h"
#include "gdma_priv.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

#if CONFIG_PM_ENABLE && SOC_PAU_SUPPORTED
#include "esp_private/gdma_sleep_retention.h"
#endif

static const char *TAG = "gdma";

#if !SOC_RCC_IS_INDEPENDENT
// Reset and Clock Control registers are mixing with other peripherals, so we need to use a critical section
#define GDMA_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define GDMA_RCC_ATOMIC()
#endif

#define GDMA_INVALID_PERIPH_TRIG  (0x3F)
#define SEARCH_REQUEST_RX_CHANNEL (1 << 0)
#define SEARCH_REQUEST_TX_CHANNEL (1 << 1)

typedef struct gdma_platform_t {
    portMUX_TYPE spinlock;                         // platform level spinlock, protect the group handle slots and reference count of each group.
    gdma_group_t *groups[SOC_GDMA_NUM_GROUPS_MAX]; // array of GDMA group instances
    int group_ref_counts[SOC_GDMA_NUM_GROUPS_MAX]; // reference count used to protect group install/uninstall
} gdma_platform_t;

static gdma_group_t *gdma_acquire_group_handle(int group_id, void (*hal_init)(gdma_hal_context_t *hal, const gdma_hal_config_t *config));
static gdma_pair_t *gdma_acquire_pair_handle(gdma_group_t *group, int pair_id);
static void gdma_release_group_handle(gdma_group_t *group);
static void gdma_release_pair_handle(gdma_pair_t *pair);
static esp_err_t gdma_del_tx_channel(gdma_channel_t *dma_channel);
static esp_err_t gdma_del_rx_channel(gdma_channel_t *dma_channel);
static esp_err_t gdma_install_rx_interrupt(gdma_rx_channel_t *rx_chan);
static esp_err_t gdma_install_tx_interrupt(gdma_tx_channel_t *tx_chan);

// gdma driver platform
static gdma_platform_t s_platform = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
};

typedef struct {
    int bus_id;
    int start_group_id;
    int end_group_id;
    int pairs_per_group;
    void (*hal_init)(gdma_hal_context_t *hal, const gdma_hal_config_t *config);
} gdma_channel_search_info_t;

static esp_err_t do_allocate_gdma_channel(const gdma_channel_search_info_t *search_info, const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
{
#if CONFIG_GDMA_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    gdma_tx_channel_t *alloc_tx_channel = NULL;
    gdma_rx_channel_t *alloc_rx_channel = NULL;
    int search_code = 0;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(config && ret_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    if (config->flags.reserve_sibling) {
        search_code = SEARCH_REQUEST_RX_CHANNEL | SEARCH_REQUEST_TX_CHANNEL; // search for a pair of channels
    }
    if (config->direction == GDMA_CHANNEL_DIRECTION_TX) {
        search_code |= SEARCH_REQUEST_TX_CHANNEL; // search TX only
        alloc_tx_channel = heap_caps_calloc(1, sizeof(gdma_tx_channel_t), GDMA_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(alloc_tx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for gdma tx channel");
    } else if (config->direction == GDMA_CHANNEL_DIRECTION_RX) {
        search_code |= SEARCH_REQUEST_RX_CHANNEL; // search RX only
        alloc_rx_channel = heap_caps_calloc(1, sizeof(gdma_rx_channel_t), GDMA_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(alloc_rx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for gdma rx channel");
    }

    if (config->sibling_chan) {
        pair = config->sibling_chan->pair;
        ESP_GOTO_ON_FALSE(pair, ESP_ERR_INVALID_ARG, err, TAG, "invalid sibling channel");
        ESP_GOTO_ON_FALSE(config->sibling_chan->direction != config->direction, ESP_ERR_INVALID_ARG, err, TAG, "sibling channel should have a different direction");
        group = pair->group;
        portENTER_CRITICAL(&group->spinlock);
        group->pair_ref_counts[pair->pair_id]++; // channel obtains a reference to pair
        portEXIT_CRITICAL(&group->spinlock);
        goto search_done; // skip the search path below if user has specify a sibling channel
    }

    int start_group_id = search_info->start_group_id;
    int end_group_id = search_info->end_group_id;
    int pairs_per_group = search_info->pairs_per_group;

    for (int i = start_group_id; i < end_group_id && search_code; i++) { // loop to search group
        group = gdma_acquire_group_handle(i, search_info->hal_init);
        group->bus_id = search_info->bus_id;
        ESP_GOTO_ON_FALSE(group, ESP_ERR_NO_MEM, err, TAG, "no mem for group(%d)", i);
        for (int j = 0; j < pairs_per_group && search_code; j++) { // loop to search pair
            pair = gdma_acquire_pair_handle(group, j);
            ESP_GOTO_ON_FALSE(pair, ESP_ERR_NO_MEM, err, TAG, "no mem for pair(%d,%d)", i, j);
            portENTER_CRITICAL(&pair->spinlock);
            if (!(search_code & pair->occupy_code)) { // pair has suitable position for acquired channel(s)
                pair->occupy_code |= search_code;
                search_code = 0; // exit search loop
            }
            portEXIT_CRITICAL(&pair->spinlock);
            // found a pair that satisfies the search condition
            if (search_code == 0) {
                portENTER_CRITICAL(&group->spinlock);
                group->pair_ref_counts[pair->pair_id]++; // channel obtains a reference to pair
                portEXIT_CRITICAL(&group->spinlock);
            }
            gdma_release_pair_handle(pair);
        } // loop used to search pair
        gdma_release_group_handle(group);
        // restore to initial state if no suitable channel slot is found
        if (search_code) {
            group = NULL;
            pair = NULL;
        }
    } // loop used to search group
    ESP_GOTO_ON_FALSE(search_code == 0, ESP_ERR_NOT_FOUND, err, TAG, "no free gdma channel, search code=%d", search_code);
    assert(pair && group); // pair and group handle shouldn't be NULL
search_done:
    // register TX channel
    if (alloc_tx_channel) {
        pair->tx_chan = alloc_tx_channel;
        alloc_tx_channel->base.pair = pair;
        alloc_tx_channel->base.direction = GDMA_CHANNEL_DIRECTION_TX;
        alloc_tx_channel->base.periph_id = GDMA_INVALID_PERIPH_TRIG;
        alloc_tx_channel->base.del = gdma_del_tx_channel; // set channel deletion function
        *ret_chan = &alloc_tx_channel->base; // return the installed channel
    }

    // register RX channel
    if (alloc_rx_channel) {
        pair->rx_chan = alloc_rx_channel;
        alloc_rx_channel->base.pair = pair;
        alloc_rx_channel->base.direction = GDMA_CHANNEL_DIRECTION_RX;
        alloc_rx_channel->base.periph_id = GDMA_INVALID_PERIPH_TRIG;
        alloc_rx_channel->base.del = gdma_del_rx_channel; // set channel deletion function
        *ret_chan = &alloc_rx_channel->base; // return the installed channel
    }

    (*ret_chan)->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    ESP_LOGD(TAG, "new %s channel (%d,%d) at %p", (config->direction == GDMA_CHANNEL_DIRECTION_TX) ? "tx" : "rx",
             group->group_id, pair->pair_id, *ret_chan);
    return ESP_OK;

err:
    if (alloc_tx_channel) {
        free(alloc_tx_channel);
    }
    if (alloc_rx_channel) {
        free(alloc_rx_channel);
    }
    if (pair) {
        gdma_release_pair_handle(pair);
    }
    if (group) {
        gdma_release_group_handle(group);
    }
    return ret;
}

#if SOC_AHB_GDMA_SUPPORTED
esp_err_t gdma_new_ahb_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
{
    gdma_channel_search_info_t search_info = {
        .bus_id = SOC_GDMA_BUS_AHB,
        .start_group_id = GDMA_LL_AHB_GROUP_START_ID,
        .end_group_id = GDMA_LL_AHB_GROUP_START_ID + GDMA_LL_AHB_NUM_GROUPS,
        .pairs_per_group = GDMA_LL_AHB_PAIRS_PER_GROUP,
        .hal_init = gdma_ahb_hal_init,
    };
    return do_allocate_gdma_channel(&search_info, config, ret_chan);
}

esp_err_t gdma_new_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
__attribute__((alias("gdma_new_ahb_channel")));
#endif // SOC_AHB_GDMA_SUPPORTED

#if SOC_AXI_GDMA_SUPPORTED
esp_err_t gdma_new_axi_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
{
    gdma_channel_search_info_t search_info = {
        .bus_id = SOC_GDMA_BUS_AXI,
        .start_group_id = GDMA_LL_AXI_GROUP_START_ID,
        .end_group_id = GDMA_LL_AXI_GROUP_START_ID + GDMA_LL_AXI_NUM_GROUPS,
        .pairs_per_group = GDMA_LL_AXI_PAIRS_PER_GROUP,
        .hal_init = gdma_axi_hal_init,
    };
    return do_allocate_gdma_channel(&search_info, config, ret_chan);
}
#endif // SOC_AXI_GDMA_SUPPORTED

esp_err_t gdma_del_channel(gdma_channel_handle_t dma_chan)
{
    ESP_RETURN_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    // reset the channel priority to default
    gdma_hal_set_priority(hal, pair->pair_id, dma_chan->direction, 0);

    // call `gdma_del_tx_channel` or `gdma_del_rx_channel` under the hood
    return dma_chan->del(dma_chan);
}

esp_err_t gdma_get_channel_id(gdma_channel_handle_t dma_chan, int *channel_id)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    ESP_GOTO_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    *channel_id = pair->pair_id;
err:
    return ret;
}

esp_err_t gdma_connect(gdma_channel_handle_t dma_chan, gdma_trigger_t trig_periph)
{
    ESP_RETURN_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(dma_chan->periph_id == GDMA_INVALID_PERIPH_TRIG, ESP_ERR_INVALID_STATE, TAG, "channel is using by peripheral: %d", dma_chan->periph_id);
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    bool periph_conflict = false;

    if (trig_periph.bus_id != SOC_GDMA_BUS_ANY) {
        ESP_RETURN_ON_FALSE(trig_periph.bus_id == group->bus_id, ESP_ERR_INVALID_ARG, TAG,
                            "peripheral and DMA system bus mismatch");
    }

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        if (trig_periph.instance_id >= 0) {
            portENTER_CRITICAL(&group->spinlock);
            if (group->tx_periph_in_use_mask & (1 << trig_periph.instance_id)) {
                periph_conflict = true;
            } else {
                group->tx_periph_in_use_mask |= (1 << trig_periph.instance_id);
            }
            portEXIT_CRITICAL(&group->spinlock);
        }
    } else {
        if (trig_periph.instance_id >= 0) {
            portENTER_CRITICAL(&group->spinlock);
            if (group->rx_periph_in_use_mask & (1 << trig_periph.instance_id)) {
                periph_conflict = true;
            } else {
                group->rx_periph_in_use_mask |= (1 << trig_periph.instance_id);
            }
            portEXIT_CRITICAL(&group->spinlock);
        }
    }

    ESP_RETURN_ON_FALSE(!periph_conflict, ESP_ERR_INVALID_STATE, TAG, "peripheral %d is already used by another channel", trig_periph.instance_id);
    gdma_hal_connect_peri(hal, pair->pair_id, dma_chan->direction, trig_periph.periph, trig_periph.instance_id);
    dma_chan->periph_id = trig_periph.instance_id;
    return ESP_OK;
}

esp_err_t gdma_disconnect(gdma_channel_handle_t dma_chan)
{
    ESP_RETURN_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(dma_chan->periph_id != GDMA_INVALID_PERIPH_TRIG, ESP_ERR_INVALID_STATE, TAG, "no peripheral is connected to the channel");

    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int save_periph_id = dma_chan->periph_id;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        if (save_periph_id >= 0) {
            portENTER_CRITICAL(&group->spinlock);
            group->tx_periph_in_use_mask &= ~(1 << save_periph_id);
            portEXIT_CRITICAL(&group->spinlock);
        }
    } else {
        if (save_periph_id >= 0) {
            portENTER_CRITICAL(&group->spinlock);
            group->rx_periph_in_use_mask &= ~(1 << save_periph_id);
            portEXIT_CRITICAL(&group->spinlock);
        }
    }

    gdma_hal_disconnect_peri(hal, pair->pair_id, dma_chan->direction);

    dma_chan->periph_id = GDMA_INVALID_PERIPH_TRIG;
    return ESP_OK;
}

esp_err_t gdma_get_free_m2m_trig_id_mask(gdma_channel_handle_t dma_chan, uint32_t *mask)
{
    ESP_RETURN_ON_FALSE(dma_chan && mask, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    uint32_t free_mask = group->hal.priv_data->m2m_free_periph_mask;

    portENTER_CRITICAL(&group->spinlock);
    free_mask &= ~(group->tx_periph_in_use_mask);
    free_mask &= ~(group->rx_periph_in_use_mask);
    portEXIT_CRITICAL(&group->spinlock);

    *mask = free_mask;
    return ESP_OK;
}

esp_err_t gdma_set_transfer_ability(gdma_channel_handle_t dma_chan, const gdma_transfer_ability_t *ability)
{
    ESP_RETURN_ON_FALSE(dma_chan && ability, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    size_t sram_alignment = ability->sram_trans_align;
    size_t psram_alignment = ability->psram_trans_align;
    // alignment should be 2^n
    ESP_RETURN_ON_FALSE((sram_alignment & (sram_alignment - 1)) == 0, ESP_ERR_INVALID_ARG,
                        TAG, "invalid sram alignment: %zu", sram_alignment);

    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
    if (psram_alignment == 0) {
        // fall back to use the same size of the psram data cache line size
        psram_alignment = data_cache_line_size;
    }
    if (psram_alignment > data_cache_line_size) {
        ESP_RETURN_ON_FALSE(((psram_alignment % data_cache_line_size) == 0), ESP_ERR_INVALID_ARG,
                            TAG, "psram_alignment(%d) should be multiple of the data_cache_line_size(%"PRIu32")",
                            psram_alignment, data_cache_line_size);
    }

    // if the DMA can't access the PSRAM, this HAL function is no-op
    gdma_hal_set_ext_mem_align(hal, pair->pair_id, dma_chan->direction, psram_alignment);

    // TX channel can always enable burst mode, no matter data alignment
    bool en_burst = true;
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        // RX channel burst mode depends on specific data alignment
        en_burst = sram_alignment >= 4;
    }
    gdma_hal_enable_burst(hal, pair->pair_id, dma_chan->direction, en_burst, en_burst);

    dma_chan->sram_alignment = sram_alignment;
    dma_chan->psram_alignment = psram_alignment;
    ESP_LOGD(TAG, "%s channel (%d,%d), (%u:%u) bytes aligned, burst %s", dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX ? "tx" : "rx",
             group->group_id, pair->pair_id, sram_alignment, psram_alignment, en_burst ? "enabled" : "disabled");

    return ESP_OK;
}

esp_err_t gdma_apply_strategy(gdma_channel_handle_t dma_chan, const gdma_strategy_config_t *config)
{
    ESP_RETURN_ON_FALSE(dma_chan && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    gdma_hal_set_strategy(hal, pair->pair_id, dma_chan->direction, config->owner_check, config->auto_update_desc);

    return ESP_OK;
}

esp_err_t gdma_set_priority(gdma_channel_handle_t dma_chan, uint32_t priority)
{
    ESP_RETURN_ON_FALSE(dma_chan && priority <= GDMA_LL_CHANNEL_MAX_PRIORITY, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    gdma_hal_set_priority(hal, pair->pair_id, dma_chan->direction, priority);

    return ESP_OK;
}

#if SOC_GDMA_SUPPORT_CRC
esp_err_t gdma_config_crc_calculator(gdma_channel_handle_t dma_chan, const gdma_crc_calculator_config_t *config)
{
    ESP_RETURN_ON_FALSE(dma_chan && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    switch (group->bus_id) {
#if SOC_AHB_GDMA_SUPPORTED
    case SOC_GDMA_BUS_AHB:
        ESP_RETURN_ON_FALSE(config->crc_bit_width <= GDMA_LL_AHB_MAX_CRC_BIT_WIDTH, ESP_ERR_INVALID_ARG, TAG, "invalid crc bit width");
        break;
#endif // SOC_AHB_GDMA_SUPPORTED
#if SOC_AXI_GDMA_SUPPORTED
    case SOC_GDMA_BUS_AXI:
        ESP_RETURN_ON_FALSE(config->crc_bit_width <= GDMA_LL_AXI_MAX_CRC_BIT_WIDTH, ESP_ERR_INVALID_ARG, TAG, "invalid crc bit width");
        break;
#endif // SOC_AXI_GDMA_SUPPORTED
    default:
        ESP_LOGE(TAG, "invalid bus id: %d", group->bus_id);
        return ESP_ERR_INVALID_ARG;
    }

    // clear the previous CRC result
    gdma_hal_clear_crc(hal, pair->pair_id, dma_chan->direction);

    // set polynomial and initial value
    gdma_hal_crc_config_t hal_config = {
        .crc_bit_width = config->crc_bit_width,
        .poly_hex = config->poly_hex,
        .init_value = config->init_value,
        .reverse_data_mask = config->reverse_data_mask,
    };
    gdma_hal_set_crc_poly(hal, pair->pair_id, dma_chan->direction, &hal_config);

    return ESP_OK;
}

esp_err_t gdma_crc_get_result(gdma_channel_handle_t dma_chan, uint32_t *result)
{
    ESP_RETURN_ON_FALSE(dma_chan && result, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    *result = gdma_hal_get_crc_result(hal, pair->pair_id, dma_chan->direction);

    return ESP_OK;
}
#endif // SOC_GDMA_SUPPORT_CRC

esp_err_t gdma_register_tx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_tx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(dma_chan && cbs && dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    gdma_tx_channel_t *tx_chan = __containerof(dma_chan, gdma_tx_channel_t, base);

#if CONFIG_GDMA_ISR_IRAM_SAFE
    if (cbs->on_trans_eof) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_eof), ESP_ERR_INVALID_ARG,
                            TAG, "on_trans_eof not in IRAM");
    }
    if (cbs->on_descr_err) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_descr_err), ESP_ERR_INVALID_ARG,
                            TAG, "on_descr_err not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG,
                            TAG, "user context not in internal RAM");
    }
#endif // CONFIG_GDMA_ISR_IRAM_SAFE

    // lazy install interrupt service
    ESP_RETURN_ON_ERROR(gdma_install_tx_interrupt(tx_chan), TAG, "install interrupt service failed");

    // enable/disable GDMA interrupt events for TX channel
    portENTER_CRITICAL(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_TX, GDMA_LL_EVENT_TX_EOF, cbs->on_trans_eof != NULL);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_TX, GDMA_LL_EVENT_TX_DESC_ERROR, cbs->on_descr_err != NULL);
    portEXIT_CRITICAL(&pair->spinlock);

    memcpy(&tx_chan->cbs, cbs, sizeof(gdma_tx_event_callbacks_t));
    tx_chan->user_data = user_data;

    ESP_RETURN_ON_ERROR(esp_intr_enable(dma_chan->intr), TAG, "enable interrupt failed");

    return ESP_OK;
}

esp_err_t gdma_register_rx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_rx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(dma_chan && cbs && dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    gdma_rx_channel_t *rx_chan = __containerof(dma_chan, gdma_rx_channel_t, base);

#if CONFIG_GDMA_ISR_IRAM_SAFE
    if (cbs->on_recv_eof) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_recv_eof), ESP_ERR_INVALID_ARG,
                            TAG, "on_recv_eof not in IRAM");
    }
    if (cbs->on_descr_err) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_descr_err), ESP_ERR_INVALID_ARG,
                            TAG, "on_descr_err not in IRAM");
    }
    if (cbs->on_recv_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_recv_done), ESP_ERR_INVALID_ARG,
                            TAG, "on_recv_done not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG,
                            TAG, "user context not in internal RAM");
    }
#endif // CONFIG_GDMA_ISR_IRAM_SAFE

    // lazy install interrupt service
    ESP_RETURN_ON_ERROR(gdma_install_rx_interrupt(rx_chan), TAG, "install interrupt service failed");

    // enable/disable GDMA interrupt events for RX channel
    portENTER_CRITICAL(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, GDMA_LL_EVENT_RX_SUC_EOF, cbs->on_recv_eof != NULL);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, GDMA_LL_EVENT_RX_DESC_ERROR, cbs->on_descr_err != NULL);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, GDMA_LL_EVENT_RX_DONE, cbs->on_recv_done != NULL);
    portEXIT_CRITICAL(&pair->spinlock);

    memcpy(&rx_chan->cbs, cbs, sizeof(gdma_rx_event_callbacks_t));
    rx_chan->user_data = user_data;

    ESP_RETURN_ON_ERROR(esp_intr_enable(dma_chan->intr), TAG, "enable interrupt failed");

    return ESP_OK;
}

esp_err_t gdma_start(gdma_channel_handle_t dma_chan, intptr_t desc_base_addr)
{
    ESP_RETURN_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(dma_chan->flags.start_stop_by_etm == false, ESP_ERR_INVALID_STATE, TAG, "channel is controlled by ETM");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    gdma_hal_start_with_desc(hal, pair->pair_id, dma_chan->direction, desc_base_addr);
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_stop(gdma_channel_handle_t dma_chan)
{
    ESP_RETURN_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(dma_chan->flags.start_stop_by_etm == false, ESP_ERR_INVALID_STATE, TAG, "channel is controlled by ETM");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    gdma_hal_stop(hal, pair->pair_id, dma_chan->direction);
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_append(gdma_channel_handle_t dma_chan)
{
    ESP_RETURN_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    gdma_hal_append(hal, pair->pair_id, dma_chan->direction);
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_reset(gdma_channel_handle_t dma_chan)
{
    ESP_RETURN_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    gdma_hal_reset(hal, pair->pair_id, dma_chan->direction);
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

    return ESP_OK;
}

static void gdma_release_group_handle(gdma_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    portENTER_CRITICAL(&s_platform.spinlock);
    s_platform.group_ref_counts[group_id]--;
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        // deregister from the platform
        s_platform.groups[group_id] = NULL;
    }
    portEXIT_CRITICAL(&s_platform.spinlock);

    if (do_deinitialize) {
        gdma_hal_deinit(&group->hal);
        GDMA_RCC_ATOMIC() {
            gdma_ll_enable_bus_clock(group_id, false);
        }
        free(group);
        ESP_LOGD(TAG, "del group %d", group_id);
    }
}

static gdma_group_t *gdma_acquire_group_handle(int group_id, void (*hal_init)(gdma_hal_context_t *hal, const gdma_hal_config_t *config))
{
    bool new_group = false;
    gdma_group_t *group = NULL;
    gdma_group_t *pre_alloc_group = heap_caps_calloc(1, sizeof(gdma_group_t), GDMA_MEM_ALLOC_CAPS);
    if (!pre_alloc_group) {
        goto out;
    }

    portENTER_CRITICAL(&s_platform.spinlock);
    if (!s_platform.groups[group_id]) {
        new_group = true;
        group = pre_alloc_group;
        s_platform.groups[group_id] = group; // register to platform
    } else {
        group = s_platform.groups[group_id];
    }
    // someone acquired the group handle means we have a new object that refer to this group
    s_platform.group_ref_counts[group_id]++;
    portEXIT_CRITICAL(&s_platform.spinlock);

    if (new_group) {
        group->group_id = group_id;
        group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        // enable APB to access GDMA registers
        GDMA_RCC_ATOMIC() {
            gdma_ll_enable_bus_clock(group_id, true);
            gdma_ll_reset_register(group_id);
        }
        gdma_hal_config_t config = {
            .group_id = group_id,
        };
        hal_init(&group->hal, &config);
        ESP_LOGD(TAG, "new group (%d) at %p", group_id, group);
    } else {
        free(pre_alloc_group);
    }
out:
    return group;
}

static void gdma_release_pair_handle(gdma_pair_t *pair)
{
    gdma_group_t *group = pair->group;
    int pair_id = pair->pair_id;
    bool do_deinitialize = false;

    portENTER_CRITICAL(&group->spinlock);
    group->pair_ref_counts[pair_id]--;
    if (group->pair_ref_counts[pair_id] == 0) {
        assert(group->pairs[pair_id]);
        do_deinitialize = true;
        group->pairs[pair_id] = NULL; // deregister from pair
    }
    portEXIT_CRITICAL(&group->spinlock);

    if (do_deinitialize) {
        free(pair);
#if CONFIG_PM_ENABLE && SOC_PAU_SUPPORTED
        gdma_sleep_retention_deinit(group->group_id, pair_id);
#endif
        ESP_LOGD(TAG, "del pair (%d,%d)", group->group_id, pair_id);
        gdma_release_group_handle(group);
    }
}

static gdma_pair_t *gdma_acquire_pair_handle(gdma_group_t *group, int pair_id)
{
    bool new_pair = false;
    gdma_pair_t *pair = NULL;
    gdma_pair_t *pre_alloc_pair = heap_caps_calloc(1, sizeof(gdma_pair_t), GDMA_MEM_ALLOC_CAPS);
    if (!pre_alloc_pair) {
        goto out;
    }

    portENTER_CRITICAL(&group->spinlock);
    if (!group->pairs[pair_id]) {
        new_pair = true;
        pair = pre_alloc_pair;
        // register the pair to the group
        group->pairs[pair_id] = pair;
    } else {
        pair = group->pairs[pair_id];
    }
    // someone acquired the pair handle means we have a new object that refer to this pair
    group->pair_ref_counts[pair_id]++;
    portEXIT_CRITICAL(&group->spinlock);

    if (new_pair) {
        pair->group = group;
        pair->pair_id = pair_id;
        pair->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;

        portENTER_CRITICAL(&s_platform.spinlock);
        // pair obtains a reference to group, so increase it
        s_platform.group_ref_counts[group->group_id]++;
        portEXIT_CRITICAL(&s_platform.spinlock);

#if CONFIG_PM_ENABLE && SOC_PAU_SUPPORTED
        gdma_sleep_retention_init(group->group_id, pair_id);
#endif
        ESP_LOGD(TAG, "new pair (%d,%d) at %p", group->group_id, pair_id, pair);
    } else {
        free(pre_alloc_pair);
    }
out:
    return pair;
}

static esp_err_t gdma_del_tx_channel(gdma_channel_t *dma_channel)
{
    gdma_pair_t *pair = dma_channel->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int pair_id = pair->pair_id;
    int group_id = group->group_id;
    gdma_tx_channel_t *tx_chan = __containerof(dma_channel, gdma_tx_channel_t, base);
    portENTER_CRITICAL(&pair->spinlock);
    pair->tx_chan = NULL;
    pair->occupy_code &= ~SEARCH_REQUEST_TX_CHANNEL;
    portEXIT_CRITICAL(&pair->spinlock);

    if (dma_channel->intr) {
        esp_intr_free(dma_channel->intr);
        portENTER_CRITICAL(&pair->spinlock);
        gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX, false); // disable all interrupt events
        gdma_hal_clear_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX); // clear all pending events
        portEXIT_CRITICAL(&pair->spinlock);
        ESP_LOGD(TAG, "uninstall interrupt service for tx channel (%d,%d)", group_id, pair_id);
    }

    free(tx_chan);
    ESP_LOGD(TAG, "del tx channel (%d,%d)", group_id, pair_id);
    // channel has a reference on pair, release it now
    gdma_release_pair_handle(pair);
    return ESP_OK;
}

static esp_err_t gdma_del_rx_channel(gdma_channel_t *dma_channel)
{
    gdma_pair_t *pair = dma_channel->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int pair_id = pair->pair_id;
    int group_id = group->group_id;
    gdma_rx_channel_t *rx_chan = __containerof(dma_channel, gdma_rx_channel_t, base);
    portENTER_CRITICAL(&pair->spinlock);
    pair->rx_chan = NULL;
    pair->occupy_code &= ~SEARCH_REQUEST_RX_CHANNEL;
    portEXIT_CRITICAL(&pair->spinlock);

    if (dma_channel->intr) {
        esp_intr_free(dma_channel->intr);
        portENTER_CRITICAL(&pair->spinlock);
        gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX, false); // disable all interrupt events
        gdma_hal_clear_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX); // clear all pending events
        portEXIT_CRITICAL(&pair->spinlock);
        ESP_LOGD(TAG, "uninstall interrupt service for rx channel (%d,%d)", group_id, pair_id);
    }

    free(rx_chan);
    ESP_LOGD(TAG, "del rx channel (%d,%d)", group_id, pair_id);
    gdma_release_pair_handle(pair);
    return ESP_OK;
}

void gdma_default_rx_isr(void *args)
{
    gdma_rx_channel_t *rx_chan = (gdma_rx_channel_t *)args;
    gdma_pair_t *pair = rx_chan->base.pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int pair_id = pair->pair_id;
    bool need_yield = false;
    // clear pending interrupt event
    uint32_t intr_status = gdma_hal_read_intr_status(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX);
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, intr_status);

    /* Call on_recv_done before eof callbacks to ensure a correct sequence */
    if ((intr_status & GDMA_LL_EVENT_RX_DONE) && rx_chan->cbs.on_recv_done) {
        /* Here we don't return an event data in this callback.
         * Because we can't get a determinant descriptor address
         * that just finished processing by DMA controller.
         * When the `rx_done` interrupt triggers, the finished descriptor should ideally
         * stored in `in_desc_bf1` register, however, as it takes a while to
         * get the `in_desc_bf1` in software, `in_desc_bf1` might have already refreshed,
         * Therefore, instead of returning an unreliable descriptor, we choose to return nothing.
         */
        need_yield |= rx_chan->cbs.on_recv_done(&rx_chan->base, NULL, rx_chan->user_data);
    }
    if ((intr_status & GDMA_LL_EVENT_RX_DESC_ERROR) && rx_chan->cbs.on_descr_err) {
        need_yield |= rx_chan->cbs.on_descr_err(&rx_chan->base, NULL, rx_chan->user_data);
    }
    if ((intr_status & GDMA_LL_EVENT_RX_SUC_EOF) && rx_chan->cbs.on_recv_eof) {
        uint32_t eof_addr = gdma_hal_get_eof_desc_addr(&group->hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, true);
        gdma_event_data_t suc_eof_data = {
            .rx_eof_desc_addr = eof_addr,
        };
        need_yield |= rx_chan->cbs.on_recv_eof(&rx_chan->base, &suc_eof_data, rx_chan->user_data);
    }
    if ((intr_status & GDMA_LL_EVENT_RX_ERR_EOF) && rx_chan->cbs.on_recv_eof) {
        uint32_t eof_addr = gdma_hal_get_eof_desc_addr(&group->hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, false);
        gdma_event_data_t err_eof_data = {
            .rx_eof_desc_addr = eof_addr,
            .flags.abnormal_eof = true,
        };
        need_yield |= rx_chan->cbs.on_recv_eof(&rx_chan->base, &err_eof_data, rx_chan->user_data);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

void gdma_default_tx_isr(void *args)
{
    gdma_tx_channel_t *tx_chan = (gdma_tx_channel_t *)args;
    gdma_pair_t *pair = tx_chan->base.pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int pair_id = pair->pair_id;
    bool need_yield = false;
    // clear pending interrupt event
    uint32_t intr_status = gdma_hal_read_intr_status(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX);
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, intr_status);

    if ((intr_status & GDMA_LL_EVENT_TX_EOF) && tx_chan->cbs.on_trans_eof) {
        uint32_t eof_addr = gdma_hal_get_eof_desc_addr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, true);
        gdma_event_data_t edata = {
            .tx_eof_desc_addr = eof_addr,
        };
        need_yield |= tx_chan->cbs.on_trans_eof(&tx_chan->base, &edata, tx_chan->user_data);
    }
    if ((intr_status & GDMA_LL_EVENT_TX_DESC_ERROR) && tx_chan->cbs.on_descr_err) {
        need_yield |= tx_chan->cbs.on_descr_err(&tx_chan->base, NULL, tx_chan->user_data);
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t gdma_install_rx_interrupt(gdma_rx_channel_t *rx_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = rx_chan->base.pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int pair_id = pair->pair_id;
    // pre-alloc a interrupt handle, with handler disabled
    int isr_flags = GDMA_INTR_ALLOC_FLAGS;
#if GDMA_LL_AHB_TX_RX_SHARE_INTERRUPT
    isr_flags |= ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
#endif
    intr_handle_t intr = NULL;
    ret = esp_intr_alloc_intrstatus(gdma_periph_signals.groups[group->group_id].pairs[pair_id].rx_irq_id, isr_flags,
                                    gdma_hal_get_intr_status_reg(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX), GDMA_LL_RX_EVENT_MASK,
                                    gdma_default_rx_isr, rx_chan, &intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    rx_chan->base.intr = intr;

    portENTER_CRITICAL(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX, false); // disable all interrupt events
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX); // clear all pending events
    portEXIT_CRITICAL(&pair->spinlock);
    ESP_LOGD(TAG, "install interrupt service for rx channel (%d,%d)", group->group_id, pair_id);

err:
    return ret;
}

static esp_err_t gdma_install_tx_interrupt(gdma_tx_channel_t *tx_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = tx_chan->base.pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    int pair_id = pair->pair_id;
    // pre-alloc a interrupt handle, with handler disabled
    int isr_flags = GDMA_INTR_ALLOC_FLAGS;
#if GDMA_LL_AHB_TX_RX_SHARE_INTERRUPT
    isr_flags |= ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
#endif
    intr_handle_t intr = NULL;
    ret = esp_intr_alloc_intrstatus(gdma_periph_signals.groups[group->group_id].pairs[pair_id].tx_irq_id, isr_flags,
                                    gdma_hal_get_intr_status_reg(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX), GDMA_LL_TX_EVENT_MASK,
                                    gdma_default_tx_isr, tx_chan, &intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    tx_chan->base.intr = intr;

    portENTER_CRITICAL(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX, false); // disable all interrupt events
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX); // clear all pending events
    portEXIT_CRITICAL(&pair->spinlock);
    ESP_LOGD(TAG, "install interrupt service for tx channel (%d,%d)", group->group_id, pair_id);

err:
    return ret;
}
