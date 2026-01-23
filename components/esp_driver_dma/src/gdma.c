/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
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
 *  | | GDMA-Pair-0 |... |GDMA-Pair-N | |  |     |  | | GDMA-Pair-0 |... |GDMA-Pair-M | |
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

#include "gdma_priv.h"
#include "esp_memory_utils.h"

#define GDMA_INVALID_PERIPH_TRIG  (0x3F)
#define SEARCH_REQUEST_RX_CHANNEL (1 << 0)
#define SEARCH_REQUEST_TX_CHANNEL (1 << 1)

typedef struct gdma_platform_t {
    portMUX_TYPE spinlock;                       // platform level spinlock, protect the group handle slots
    gdma_group_t *groups[GDMA_LL_GET(INST_NUM)]; // array of GDMA group instances
} gdma_platform_t;

static gdma_group_t *gdma_try_alloc_group_handle(int group_id, void (*hal_init)(gdma_hal_context_t *hal, const gdma_hal_config_t *config));
static gdma_pair_t *gdma_try_alloc_pair_handle(gdma_group_t *group, int pair_id);
static void gdma_try_free_group_handle(gdma_group_t *group);
static void gdma_try_free_pair_handle(gdma_pair_t *pair);
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
    uint32_t m2m_capable_mask;
    void (*hal_init)(gdma_hal_context_t *hal, const gdma_hal_config_t *config);
} gdma_channel_search_info_t;

ESP_LOG_ATTR_TAG(TAG, "gdma");

static esp_err_t do_allocate_gdma_channel(const gdma_channel_search_info_t *search_info, const gdma_channel_alloc_config_t *config,
                                          gdma_channel_handle_t *ret_tx_chan, gdma_channel_handle_t *ret_rx_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_tx_channel_t *alloc_tx_channel = NULL;
    gdma_rx_channel_t *alloc_rx_channel = NULL;
    int search_code = 0;
    bool found_pair = false;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;

    // Validate input parameters
    ESP_RETURN_ON_FALSE(search_info && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ret_tx_chan || ret_rx_chan, ESP_ERR_INVALID_ARG, TAG, "both channel pointers are NULL");
    ESP_RETURN_ON_FALSE(search_info->start_group_id < search_info->end_group_id, ESP_ERR_INVALID_ARG, TAG, "invalid group range");
    ESP_RETURN_ON_FALSE(search_info->pairs_per_group > 0, ESP_ERR_INVALID_ARG, TAG, "invalid pairs_per_group");

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // retention module is per GDMA pair, before we allocate the pair object, some common registers are already configured in "hal_init"
    // if a light sleep happens and powers off the gdma module, those registers will get lost
    // to work around it, we can acquire a power lock first, before any register configuration
    sleep_retention_power_lock_acquire();
#endif

    // Determine search code based on which channels are requested
    if (ret_tx_chan) {
        search_code |= SEARCH_REQUEST_TX_CHANNEL;
        alloc_tx_channel = heap_caps_calloc(1, sizeof(gdma_tx_channel_t), GDMA_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(alloc_tx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for gdma tx channel");
    }
    if (ret_rx_chan) {
        search_code |= SEARCH_REQUEST_RX_CHANNEL;
        alloc_rx_channel = heap_caps_calloc(1, sizeof(gdma_rx_channel_t), GDMA_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(alloc_rx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for gdma rx channel");
    }

    // If both channels are requested, they must be in the same pair
    // The search_code will have both bits set, ensuring we find a pair with both channels available

    int start_group_id = search_info->start_group_id;
    int end_group_id = search_info->end_group_id;
    int pairs_per_group = search_info->pairs_per_group;

    // loop to search group
    for (int i = start_group_id; i < end_group_id && !found_pair; i++) {
        group = gdma_try_alloc_group_handle(i, search_info->hal_init);
        ESP_GOTO_ON_FALSE(group, ESP_ERR_NO_MEM, err, TAG, "no mem for group(%d)", i);
        group->bus_id = search_info->bus_id;

        // loop to search pair
        for (int j = 0; j < pairs_per_group && !found_pair; j++) {
            // Check M2M capability BEFORE acquiring pair to avoid unnecessary allocation
            if (ret_tx_chan && ret_rx_chan) {  // Both channels requested (implicit M2M usage)
                if (!((1 << j) & search_info->m2m_capable_mask)) {
                    ESP_LOGV(TAG, "pair(%d,%d) not M2M capable, skip", i, j);
                    continue;  // Skip pairs that don't support M2M
                }
            }

            pair = gdma_try_alloc_pair_handle(group, j);
            ESP_GOTO_ON_FALSE(pair, ESP_ERR_NO_MEM, err, TAG, "no mem for pair(%d,%d)", i, j);

            // Check if channels are available
            esp_os_enter_critical(&pair->spinlock);
            if (!(search_code & pair->occupy_code)) { // pair has suitable position for acquired channel(s)
                pair->occupy_code |= search_code;
                found_pair = true;
            }
            esp_os_exit_critical(&pair->spinlock);
        } // loop used to search pair

        // If no suitable pair found in this group, release the group before trying next group
        if (!found_pair) {
            gdma_try_free_group_handle(group);
            group = NULL;
        }
    } // loop used to search group
    ESP_GOTO_ON_FALSE(found_pair, ESP_ERR_NOT_FOUND, err, TAG, "no free gdma channel, search code=%d", search_code);

    // Note: No lock protection is needed here because pair->occupy_code was already set atomically
    // in the critical section above. Once the occupy_code bit is set, this is the only thread that
    // can write to pair->tx_chan, so there's no race condition.
    if (alloc_tx_channel) {
        pair->tx_chan = alloc_tx_channel;
        alloc_tx_channel->base.pair = pair;
        alloc_tx_channel->base.direction = GDMA_CHANNEL_DIRECTION_TX;
        alloc_tx_channel->base.periph_id = GDMA_INVALID_PERIPH_TRIG;
        alloc_tx_channel->base.flags.isr_cache_safe = config->flags.isr_cache_safe;
        alloc_tx_channel->base.del = gdma_del_tx_channel; // set channel deletion function
        alloc_tx_channel->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        *ret_tx_chan = &alloc_tx_channel->base; // return the installed channel
#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
        // set 1 as default weight, can be overwritten by user
        gdma_set_weight(&alloc_tx_channel->base, 1);
#endif
    }

    // Note: Same as TX channel - no lock needed due to occupy_code protection
    if (alloc_rx_channel) {
        pair->rx_chan = alloc_rx_channel;
        alloc_rx_channel->base.pair = pair;
        alloc_rx_channel->base.direction = GDMA_CHANNEL_DIRECTION_RX;
        alloc_rx_channel->base.periph_id = GDMA_INVALID_PERIPH_TRIG;
        alloc_rx_channel->base.flags.isr_cache_safe = config->flags.isr_cache_safe;
        alloc_rx_channel->base.del = gdma_del_rx_channel; // set channel deletion function
        alloc_rx_channel->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        *ret_rx_chan = &alloc_rx_channel->base; // return the installed channel
#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
        // set 1 as default weight, can be overwritten by user
        gdma_set_weight(&alloc_rx_channel->base, 1);
#endif
    }

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // set up the sleep retention link for the pair
    // Each channel holds a reference to the sleep retention, so we need to acquire it for each channel
    if (alloc_tx_channel) {
        gdma_acquire_sleep_retention(pair);
    }
    if (alloc_rx_channel) {
        gdma_acquire_sleep_retention(pair);
    }
    // release the helper power lock because we have finished setting up the sleep retention link
    sleep_retention_power_lock_release();
#endif
    ESP_LOGV(TAG, "new channel(s) in (%d,%d)%s%s", group->group_id, pair->pair_id,
             alloc_tx_channel ? " tx" : "", alloc_rx_channel ? " rx" : "");
    return ESP_OK;

err:
    if (pair) {
        // If we successfully set occupy_code and/or channel pointers but failed later,
        // we need to restore the pair state before releasing it
        if (found_pair) {
            esp_os_enter_critical(&pair->spinlock);
            // Clear the occupy_code bits we set
            pair->occupy_code &= ~search_code;
            // NULL out any channel pointers we may have set to avoid dangling pointers
            // The channel memory will be freed below
            if (alloc_tx_channel && pair->tx_chan == alloc_tx_channel) {
                pair->tx_chan = NULL;
            }
            if (alloc_rx_channel && pair->rx_chan == alloc_rx_channel) {
                pair->rx_chan = NULL;
            }
            esp_os_exit_critical(&pair->spinlock);
        }
        // If we successfully acquired a pair but failed later, release it
        // The pair release will also check and free the group if all pairs are gone
        gdma_try_free_pair_handle(pair);
    } else if (group) {
        // If we acquired a group but failed to get a pair, try to release the group
        // This handles the case where we created an empty group that never got any pairs
        // Note: gdma_try_free_group_handle is state-based and idempotent, so calling it
        // even when pair was released is safe (though the second call would be redundant)
        gdma_try_free_group_handle(group);
    }
    // Free the channel structures after clearing the pair pointers
    if (alloc_tx_channel) {
        free(alloc_tx_channel);
    }
    if (alloc_rx_channel) {
        free(alloc_rx_channel);
    }

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    sleep_retention_power_lock_release();
#endif
    return ret;
}

#if SOC_HAS(AHB_GDMA)
esp_err_t gdma_new_ahb_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_tx_chan, gdma_channel_handle_t *ret_rx_chan)
{
    gdma_channel_search_info_t search_info = {
        .bus_id = SOC_GDMA_BUS_AHB,
        .start_group_id = GDMA_LL_AHB_GROUP_START_ID,
        .end_group_id = GDMA_LL_AHB_GROUP_START_ID + GDMA_LL_AHB_NUM_GROUPS,
        .pairs_per_group = GDMA_LL_AHB_PAIRS_PER_GROUP,
        .m2m_capable_mask = GDMA_LL_AHB_M2M_CAPABLE_PAIR_MASK,
        .hal_init = gdma_ahb_hal_init,
    };
    return do_allocate_gdma_channel(&search_info, config, ret_tx_chan, ret_rx_chan);
}
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
esp_err_t gdma_new_axi_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_tx_chan, gdma_channel_handle_t *ret_rx_chan)
{
    gdma_channel_search_info_t search_info = {
        .bus_id = SOC_GDMA_BUS_AXI,
        .start_group_id = GDMA_LL_AXI_GROUP_START_ID,
        .end_group_id = GDMA_LL_AXI_GROUP_START_ID + GDMA_LL_AXI_NUM_GROUPS,
        .pairs_per_group = GDMA_LL_AXI_PAIRS_PER_GROUP,
        .m2m_capable_mask = GDMA_LL_AXI_M2M_CAPABLE_PAIR_MASK,
        .hal_init = gdma_axi_hal_init,
    };
    return do_allocate_gdma_channel(&search_info, config, ret_tx_chan, ret_rx_chan);
}
#endif // SOC_HAS(AXI_GDMA)

esp_err_t gdma_del_channel(gdma_channel_handle_t dma_chan)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    // reset the channel priority to default
    gdma_hal_set_priority(hal, pair->pair_id, dma_chan->direction, 0);

    // call `gdma_del_tx_channel` or `gdma_del_rx_channel` under the hood
    return dma_chan->del(dma_chan);
}

esp_err_t gdma_get_group_channel_id(gdma_channel_handle_t dma_chan, int *group_id, int *channel_id)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    if (group_id != NULL) {
        *group_id = pair->group->group_id;
    }
    if (channel_id != NULL) {
        *channel_id = pair->pair_id;
    }
    return ESP_OK;
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
            esp_os_enter_critical(&group->spinlock);
            if (group->tx_periph_in_use_mask & (1 << trig_periph.instance_id)) {
                periph_conflict = true;
            } else {
                group->tx_periph_in_use_mask |= (1 << trig_periph.instance_id);
            }
            esp_os_exit_critical(&group->spinlock);
        }
    } else {
        if (trig_periph.instance_id >= 0) {
            esp_os_enter_critical(&group->spinlock);
            if (group->rx_periph_in_use_mask & (1 << trig_periph.instance_id)) {
                periph_conflict = true;
            } else {
                group->rx_periph_in_use_mask |= (1 << trig_periph.instance_id);
            }
            esp_os_exit_critical(&group->spinlock);
        }
    }

    ESP_RETURN_ON_FALSE(!periph_conflict, ESP_ERR_INVALID_STATE, TAG, "peripheral %d is already used by another channel", trig_periph.instance_id);
    if (trig_periph.bus_id == SOC_GDMA_TRIG_PERIPH_M2M0_BUS) {
        // M2M transfer
        gdma_hal_connect_mem(hal, pair->pair_id, dma_chan->direction, trig_periph.instance_id);
    } else {
        gdma_hal_connect_peri(hal, pair->pair_id, dma_chan->direction, trig_periph.instance_id);
    }
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
            esp_os_enter_critical(&group->spinlock);
            group->tx_periph_in_use_mask &= ~(1 << save_periph_id);
            esp_os_exit_critical(&group->spinlock);
        }
    } else {
        if (save_periph_id >= 0) {
            esp_os_enter_critical(&group->spinlock);
            group->rx_periph_in_use_mask &= ~(1 << save_periph_id);
            esp_os_exit_critical(&group->spinlock);
        }
    }

    gdma_hal_disconnect_all(hal, pair->pair_id, dma_chan->direction);

    dma_chan->periph_id = GDMA_INVALID_PERIPH_TRIG;
    return ESP_OK;
}

esp_err_t gdma_get_free_m2m_trig_id_mask(gdma_channel_handle_t dma_chan, uint32_t *mask)
{
    if (!dma_chan || !mask) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    uint32_t free_mask = group->hal.priv_data->m2m_free_periph_mask;

    esp_os_enter_critical(&group->spinlock);
    free_mask &= ~(group->tx_periph_in_use_mask);
    free_mask &= ~(group->rx_periph_in_use_mask);
    esp_os_exit_critical(&group->spinlock);

    *mask = free_mask;
    return ESP_OK;
}

esp_err_t gdma_config_transfer(gdma_channel_handle_t dma_chan, const gdma_transfer_config_t *config)
{
    if (!dma_chan || !config) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t max_data_burst_size = config->max_data_burst_size;
    if (max_data_burst_size) {
        // burst size must be power of 2
        ESP_RETURN_ON_FALSE((max_data_burst_size & (max_data_burst_size - 1)) == 0, ESP_ERR_INVALID_ARG,
                            TAG, "invalid max_data_burst_size: %"PRIu32, max_data_burst_size);
#if GDMA_LL_GET(AHB_PSRAM_CAPABLE) || GDMA_LL_GET(AXI_PSRAM_CAPABLE)
        if (config->access_ext_mem) {
            ESP_RETURN_ON_FALSE(max_data_burst_size <= GDMA_LL_MAX_BURST_SIZE_PSRAM, ESP_ERR_INVALID_ARG,
                                TAG, "max_data_burst_size must not exceed %d when accessing external memory", GDMA_LL_MAX_BURST_SIZE_PSRAM);
        }
#endif
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    size_t int_mem_alignment = 1;
    size_t ext_mem_alignment = 1;

    // always enable descriptor burst as the descriptor is always word aligned and is in the internal SRAM
    bool en_desc_burst = true;
    bool en_data_burst = max_data_burst_size > 0;

#if SOC_PSRAM_DMA_CAPABLE || SOC_DMA_CAN_ACCESS_FLASH
    // if MSPI encryption is enabled, and DMA wants to read/write external memory
    if (efuse_hal_flash_encryption_enabled() && config->access_ext_mem) {
        uint32_t enc_mem_alignment = GDMA_LL_GET(ACCESS_ENCRYPTION_MEM_ALIGNMENT);
        // when DMA access the encrypted external memory, extra alignment is needed for external memory
        ext_mem_alignment = MAX(ext_mem_alignment, enc_mem_alignment);
        if (max_data_burst_size < enc_mem_alignment) {
            ESP_LOGW(TAG, "GDMA channel access encrypted external memory, adjust burst size to %d", enc_mem_alignment);
            en_data_burst = true;
            max_data_burst_size = enc_mem_alignment;
        }
    }
#endif // SOC_PSRAM_DMA_CAPABLE || SOC_DMA_CAN_ACCESS_FLASH

    gdma_hal_enable_burst(hal, pair->pair_id, dma_chan->direction, en_data_burst, en_desc_burst);
    if (en_data_burst) {
        gdma_hal_set_burst_size(hal, pair->pair_id, dma_chan->direction, max_data_burst_size);
#if CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
        // due to hardware limitation, if weighted arbitration is enabled, the data must be aligned to burst size
        int_mem_alignment = MAX(int_mem_alignment, max_data_burst_size);
        ext_mem_alignment = MAX(ext_mem_alignment, max_data_burst_size);
#endif
    }

#if GDMA_LL_AHB_RX_BURST_NEEDS_ALIGNMENT
    if (en_data_burst && dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        int_mem_alignment = MAX(int_mem_alignment, 4);
        ext_mem_alignment = MAX(ext_mem_alignment, max_data_burst_size);
    }
#endif

    // if the channel is not allowed to access external memory, set a super big (meaningless) alignment value
    // so when the upper layer checks the alignment with an external buffer, the check should fail
    if (!config->access_ext_mem) {
        ext_mem_alignment = BIT(31);
    }

    dma_chan->int_mem_alignment = int_mem_alignment;
    dma_chan->ext_mem_alignment = ext_mem_alignment;
    return ESP_OK;
}

esp_err_t gdma_get_alignment_constraints(gdma_channel_handle_t dma_chan, size_t *int_mem_alignment, size_t *ext_mem_alignment)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    if (int_mem_alignment) {
        *int_mem_alignment = dma_chan->int_mem_alignment;
    }
    if (ext_mem_alignment) {
        *ext_mem_alignment = dma_chan->ext_mem_alignment;
    }
    return ESP_OK;
}

esp_err_t gdma_apply_strategy(gdma_channel_handle_t dma_chan, const gdma_strategy_config_t *config)
{
    if (!dma_chan || !config) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    gdma_hal_set_strategy(hal, pair->pair_id, dma_chan->direction, config->owner_check, config->auto_update_desc, config->eof_till_data_popped);

    return ESP_OK;
}

esp_err_t gdma_set_priority(gdma_channel_handle_t dma_chan, uint32_t priority)
{
    if (!dma_chan || priority > GDMA_LL_CHANNEL_MAX_PRIORITY) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    gdma_hal_set_priority(hal, pair->pair_id, dma_chan->direction, priority);

    return ESP_OK;
}

#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
esp_err_t gdma_set_weight(gdma_channel_handle_t dma_chan, uint32_t weight)
{
    if (!dma_chan || weight > GDMA_LL_CHANNEL_MAX_WEIGHT) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    gdma_hal_set_weight(hal, pair->pair_id, dma_chan->direction, weight);

    return ESP_OK;
}
# endif

esp_err_t gdma_register_tx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_tx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(dma_chan && cbs && dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    gdma_tx_channel_t *tx_chan = __containerof(dma_chan, gdma_tx_channel_t, base);

    if (dma_chan->flags.isr_cache_safe) {
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
    }

    // lazy install interrupt service
    ESP_RETURN_ON_ERROR(gdma_install_tx_interrupt(tx_chan), TAG, "install interrupt service failed");

    // enable/disable GDMA interrupt events for TX channel
    esp_os_enter_critical(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_TX, GDMA_LL_EVENT_TX_EOF, cbs->on_trans_eof != NULL);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_TX, GDMA_LL_EVENT_TX_DESC_ERROR, cbs->on_descr_err != NULL);
    esp_os_exit_critical(&pair->spinlock);

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

    if (dma_chan->flags.isr_cache_safe) {
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
    }

    // lazy install interrupt service
    ESP_RETURN_ON_ERROR(gdma_install_rx_interrupt(rx_chan), TAG, "install interrupt service failed");

    // enable/disable GDMA interrupt events for RX channel
    esp_os_enter_critical(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, GDMA_LL_EVENT_RX_SUC_EOF | GDMA_LL_EVENT_RX_ERR_EOF, cbs->on_recv_eof != NULL);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, GDMA_LL_EVENT_RX_DESC_ERROR, cbs->on_descr_err != NULL);
    gdma_hal_enable_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, GDMA_LL_EVENT_RX_DONE, cbs->on_recv_done != NULL);
    esp_os_exit_critical(&pair->spinlock);

    memcpy(&rx_chan->cbs, cbs, sizeof(gdma_rx_event_callbacks_t));
    rx_chan->user_data = user_data;

    ESP_RETURN_ON_ERROR(esp_intr_enable(dma_chan->intr), TAG, "enable interrupt failed");

    return ESP_OK;
}

esp_err_t gdma_start(gdma_channel_handle_t dma_chan, intptr_t desc_base_addr)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dma_chan->flags.start_stop_by_etm) {
        return ESP_ERR_INVALID_STATE;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    esp_os_enter_critical_safe(&dma_chan->spinlock);
    gdma_hal_start_with_desc(hal, pair->pair_id, dma_chan->direction, desc_base_addr);
    esp_os_exit_critical_safe(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_stop(gdma_channel_handle_t dma_chan)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dma_chan->flags.start_stop_by_etm) {
        return ESP_ERR_INVALID_STATE;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    esp_os_enter_critical_safe(&dma_chan->spinlock);
    gdma_hal_stop(hal, pair->pair_id, dma_chan->direction);
    esp_os_exit_critical_safe(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_append(gdma_channel_handle_t dma_chan)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    esp_os_enter_critical_safe(&dma_chan->spinlock);
    gdma_hal_append(hal, pair->pair_id, dma_chan->direction);
    esp_os_exit_critical_safe(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_reset(gdma_channel_handle_t dma_chan)
{
    if (!dma_chan) {
        return ESP_ERR_INVALID_ARG;
    }
    gdma_pair_t *pair = dma_chan->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;

    esp_os_enter_critical_safe(&dma_chan->spinlock);
    gdma_hal_reset(hal, pair->pair_id, dma_chan->direction);
    esp_os_exit_critical_safe(&dma_chan->spinlock);

    return ESP_OK;
}

static void gdma_try_free_group_handle(gdma_group_t *group)
{
    int group_id = group->group_id;
    bool do_deinitialize = false;

    esp_os_enter_critical(&s_platform.spinlock);
    // Check if all pairs are gone - if so, free the group
    bool all_pairs_gone = true;
    for (int i = 0; i < GDMA_LL_GET(PAIRS_PER_INST); i++) {
        if (group->pairs[i] != NULL) {
            all_pairs_gone = false;
            break;
        }
    }
    if (all_pairs_gone) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        // deregister from the platform
        s_platform.groups[group_id] = NULL;
        gdma_hal_deinit(&group->hal);
        PERIPH_RCC_ATOMIC() {
            gdma_ll_enable_bus_clock(group_id, false);
        }
    }
    esp_os_exit_critical(&s_platform.spinlock);

    if (do_deinitialize) {
        free(group);
        ESP_LOGV(TAG, "del group %d", group_id);
    }
}

static gdma_group_t *gdma_try_alloc_group_handle(int group_id, void (*hal_init)(gdma_hal_context_t *hal, const gdma_hal_config_t *config))
{
    bool new_group = false;
    gdma_group_t *group = NULL;
    gdma_group_t *pre_alloc_group = heap_caps_calloc(1, sizeof(gdma_group_t), GDMA_MEM_ALLOC_CAPS);
    if (!pre_alloc_group) {
        goto out;
    }

    esp_os_enter_critical(&s_platform.spinlock);
    if (!s_platform.groups[group_id]) {
        new_group = true;
        group = pre_alloc_group;
        s_platform.groups[group_id] = group; // register to platform

        group->group_id = group_id;
        group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        // enable APB to access GDMA registers
        PERIPH_RCC_ATOMIC() {
            gdma_ll_enable_bus_clock(group_id, true);
            gdma_ll_reset_register(group_id);
        }
        gdma_hal_config_t config = {
            .group_id = group_id,
#if CONFIG_GDMA_ENABLE_WEIGHTED_ARBITRATION
            .flags.enable_weighted_arbitration = true,
#endif
        };
        hal_init(&group->hal, &config);
    } else {
        group = s_platform.groups[group_id];
    }
    esp_os_exit_critical(&s_platform.spinlock);

    if (new_group) {
        ESP_LOGV(TAG, "new group (%d) at %p", group_id, group);
    } else {
        free(pre_alloc_group);
    }
out:
    return group;
}

static void gdma_try_free_pair_handle(gdma_pair_t *pair)
{
    gdma_group_t *group = pair->group;
    int pair_id = pair->pair_id;
    bool do_deinitialize = false;

    // Use pair->spinlock to check channel pointers since that's the lock protecting them
    esp_os_enter_critical(&pair->spinlock);
    // Only free the pair if nothing is using or reserving it
    if (pair->tx_chan == NULL && pair->rx_chan == NULL && pair->occupy_code == 0) {
        // Mark for deinitialization, but need to update group->pairs under group lock
        do_deinitialize = true;
    }
    esp_os_exit_critical(&pair->spinlock);

    if (do_deinitialize) {
        // Now safely update the group's pair pointer under group lock
        esp_os_enter_critical(&group->spinlock);
        // Double-check in case another thread beat us to it
        if (group->pairs[pair_id] != NULL) {
            group->pairs[pair_id] = NULL; // deregister from group
        } else {
            // Another thread already freed this pair, abort
            do_deinitialize = false;
        }
        esp_os_exit_critical(&group->spinlock);
    }

    if (do_deinitialize) {
        free(pair);
        ESP_LOGV(TAG, "del pair (%d,%d)", group->group_id, pair_id);
        gdma_try_free_group_handle(group);
    }
}

static gdma_pair_t *gdma_try_alloc_pair_handle(gdma_group_t *group, int pair_id)
{
    bool new_pair = false;
    gdma_pair_t *pair = NULL;
    gdma_pair_t *pre_alloc_pair = heap_caps_calloc(1, sizeof(gdma_pair_t), GDMA_MEM_ALLOC_CAPS);
    if (!pre_alloc_pair) {
        goto out;
    }

    esp_os_enter_critical(&group->spinlock);
    if (!group->pairs[pair_id]) {
        new_pair = true;
        pair = pre_alloc_pair;
        // initialize pair before registering to avoid accessing uninitialized pair
        pair->group = group;
        pair->pair_id = pair_id;
        pair->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        // register the pair to the group
        group->pairs[pair_id] = pair;
    } else {
        pair = group->pairs[pair_id];
    }
    esp_os_exit_critical(&group->spinlock);

    // Log new pair creation
    if (new_pair) {
        ESP_LOGV(TAG, "new pair (%d,%d) at %p", group->group_id, pair_id, pair);
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
    // Store IDs early to avoid use-after-free
    int pair_id = pair->pair_id;
    int group_id = group->group_id;
    gdma_tx_channel_t *tx_chan = __containerof(dma_channel, gdma_tx_channel_t, base);
    esp_os_enter_critical(&pair->spinlock);
    pair->tx_chan = NULL;
    pair->occupy_code &= ~SEARCH_REQUEST_TX_CHANNEL;
    esp_os_exit_critical(&pair->spinlock);

    if (dma_channel->intr) {
        esp_intr_free(dma_channel->intr);
        esp_os_enter_critical(&pair->spinlock);
        gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX, false); // disable all interrupt events
        gdma_hal_clear_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX); // clear all pending events
        esp_os_exit_critical(&pair->spinlock);
        ESP_LOGV(TAG, "uninstall interrupt service for tx channel (%d,%d)", group_id, pair_id);
    }

    free(tx_chan);
    ESP_LOGV(TAG, "del tx channel (%d,%d)", group_id, pair_id);

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // release sleep retention lock
    gdma_release_sleep_retention(pair);
#endif
    // release pair handle (will free pair if both channels are deleted)
    gdma_try_free_pair_handle(pair);
    return ESP_OK;
}

static esp_err_t gdma_del_rx_channel(gdma_channel_t *dma_channel)
{
    gdma_pair_t *pair = dma_channel->pair;
    gdma_group_t *group = pair->group;
    gdma_hal_context_t *hal = &group->hal;
    // Store IDs early to avoid use-after-free
    int pair_id = pair->pair_id;
    int group_id = group->group_id;
    gdma_rx_channel_t *rx_chan = __containerof(dma_channel, gdma_rx_channel_t, base);
    esp_os_enter_critical(&pair->spinlock);
    pair->rx_chan = NULL;
    pair->occupy_code &= ~SEARCH_REQUEST_RX_CHANNEL;
    esp_os_exit_critical(&pair->spinlock);

    if (dma_channel->intr) {
        esp_intr_free(dma_channel->intr);
        esp_os_enter_critical(&pair->spinlock);
        gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX, false); // disable all interrupt events
        gdma_hal_clear_intr(hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX); // clear all pending events
        esp_os_exit_critical(&pair->spinlock);
        ESP_LOGV(TAG, "uninstall interrupt service for rx channel (%d,%d)", group_id, pair_id);
    }

    free(rx_chan);
    ESP_LOGV(TAG, "del rx channel (%d,%d)", group_id, pair_id);

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_GDMA_SUPPORT_SLEEP_RETENTION
    // release sleep retention lock
    gdma_release_sleep_retention(pair);
#endif
    // release pair handle (will free pair if both channels are deleted)
    gdma_try_free_pair_handle(pair);
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
    bool abnormal_eof = false;
    bool normal_eof = false;

    // clear pending interrupt event first
    // reading the raw interrupt status because we also want to know the EOF status, even if the EOF interrupt is not enabled
    uint32_t intr_status = gdma_hal_read_intr_status(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, true);
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, intr_status);

    // prepare data for different events
    uint32_t eof_addr = 0;
    if (intr_status & GDMA_LL_EVENT_RX_SUC_EOF) {
        eof_addr = gdma_hal_get_eof_desc_addr(&group->hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, true);
        normal_eof = true;
    }
    if (intr_status & GDMA_LL_EVENT_RX_ERR_EOF) {
        eof_addr = gdma_hal_get_eof_desc_addr(&group->hal, pair->pair_id, GDMA_CHANNEL_DIRECTION_RX, false);
        abnormal_eof = true;
    }
    gdma_event_data_t edata = {
        .rx_eof_desc_addr = eof_addr,
        .flags = {
            .abnormal_eof = abnormal_eof,
            .normal_eof = normal_eof,
        }
    };

    if ((intr_status & GDMA_LL_EVENT_RX_DESC_ERROR) && rx_chan->cbs.on_descr_err) {
        // in the future, we may add more information about the error descriptor into the event data,
        // but for now, we just pass NULL
        need_yield |= rx_chan->cbs.on_descr_err(&rx_chan->base, NULL, rx_chan->user_data);
    }

    // we expect the caller will do data process in the recv_done callback first, and handle the EOF event later
    if ((intr_status & GDMA_LL_EVENT_RX_DONE) && rx_chan->cbs.on_recv_done) {
        need_yield |= rx_chan->cbs.on_recv_done(&rx_chan->base, &edata, rx_chan->user_data);
    }
    if ((intr_status & (GDMA_LL_EVENT_RX_SUC_EOF | GDMA_LL_EVENT_RX_ERR_EOF)) && rx_chan->cbs.on_recv_eof) {
        need_yield |= rx_chan->cbs.on_recv_eof(&rx_chan->base, &edata, rx_chan->user_data);
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
    uint32_t intr_status = gdma_hal_read_intr_status(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, false);
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, intr_status);

    if ((intr_status & GDMA_LL_EVENT_TX_EOF) && tx_chan->cbs.on_trans_eof) {
        uint32_t eof_addr = gdma_hal_get_eof_desc_addr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, true);
        gdma_event_data_t edata = {
            .tx_eof_desc_addr = eof_addr,
            .flags.normal_eof = true,
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
    int isr_flags = ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LOWMED;
    if (rx_chan->base.flags.isr_cache_safe) {
        isr_flags |= ESP_INTR_FLAG_IRAM;
    }
#if GDMA_LL_AHB_TX_RX_SHARE_INTERRUPT
    isr_flags |= ESP_INTR_FLAG_SHARED;
#endif
    intr_handle_t intr = NULL;
    ret = esp_intr_alloc_intrstatus(gdma_periph_signals.groups[group->group_id].pairs[pair_id].rx_irq_id, isr_flags,
                                    gdma_hal_get_intr_status_reg(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX), GDMA_LL_RX_EVENT_MASK,
                                    gdma_default_rx_isr, rx_chan, &intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    rx_chan->base.intr = intr;

    esp_os_enter_critical(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX, false); // disable all interrupt events
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_RX, UINT32_MAX); // clear all pending events
    esp_os_exit_critical(&pair->spinlock);
    ESP_LOGV(TAG, "install interrupt service for rx channel (%d,%d)", group->group_id, pair_id);

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
    int isr_flags = ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LOWMED;
    if (tx_chan->base.flags.isr_cache_safe) {
        isr_flags |= ESP_INTR_FLAG_IRAM;
    }
#if GDMA_LL_AHB_TX_RX_SHARE_INTERRUPT
    isr_flags |= ESP_INTR_FLAG_SHARED;
#endif
    intr_handle_t intr = NULL;
    ret = esp_intr_alloc_intrstatus(gdma_periph_signals.groups[group->group_id].pairs[pair_id].tx_irq_id, isr_flags,
                                    gdma_hal_get_intr_status_reg(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX), GDMA_LL_TX_EVENT_MASK,
                                    gdma_default_tx_isr, tx_chan, &intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    tx_chan->base.intr = intr;

    esp_os_enter_critical(&pair->spinlock);
    gdma_hal_enable_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX, false); // disable all interrupt events
    gdma_hal_clear_intr(hal, pair_id, GDMA_CHANNEL_DIRECTION_TX, UINT32_MAX); // clear all pending events
    esp_os_exit_critical(&pair->spinlock);
    ESP_LOGV(TAG, "install interrupt service for tx channel (%d,%d)", group->group_id, pair_id);

err:
    return ret;
}

#if CONFIG_GDMA_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void gdma_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
