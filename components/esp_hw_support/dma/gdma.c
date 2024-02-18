/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <stdlib.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
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

#if CONFIG_PM_ENABLE && SOC_PAU_SUPPORTED
#include "esp_private/gdma_sleep_retention.h"
#endif

static const char *TAG = "gdma";

#define GDMA_INVALID_PERIPH_TRIG  (0x3F)
#define SEARCH_REQUEST_RX_CHANNEL (1 << 0)
#define SEARCH_REQUEST_TX_CHANNEL (1 << 1)

/**
 * GDMA driver consists of there object class, namely: Group, Pair and Channel.
 * Channel is allocated when user calls `gdma_new_channel`, its lifecycle is maintained by user.
 * Pair and Group are all lazy allocated, their life cycles are maintained by this driver.
 * We use reference count to track their life cycles, i.e. the driver will free their memory only when their reference count reached to 0.
 *
 * We don't use an all-in-one spin lock in this driver, instead, we created different spin locks at different level.
 * For platform, it has a spinlock, which is used to protect the group handle slots and reference count of each group.
 * For group, it has a spinlock, which is used to protect group level stuffs, e.g. hal object, pair handle slots and reference count of each pair.
 * For pair, it has a spinlock, which is used to protect pair level stuffs, e.g. channel handle slots, occupy code.
 */

typedef struct gdma_platform_t {
    portMUX_TYPE spinlock;                 // platform level spinlock
    gdma_group_t *groups[SOC_GDMA_GROUPS]; // array of GDMA group instances
    int group_ref_counts[SOC_GDMA_GROUPS]; // reference count used to protect group install/uninstall
} gdma_platform_t;

static gdma_group_t *gdma_acquire_group_handle(int group_id);
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
    .groups = {} // groups will be lazy installed
};

esp_err_t gdma_new_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_tx_channel_t *alloc_tx_channel = NULL;
    gdma_rx_channel_t *alloc_rx_channel = NULL;
    int search_code = 0;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_GOTO_ON_FALSE(config && ret_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

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

    for (int i = 0; i < SOC_GDMA_GROUPS && search_code; i++) { // loop to search group
        group = gdma_acquire_group_handle(i);
        ESP_GOTO_ON_FALSE(group, ESP_ERR_NO_MEM, err, TAG, "no mem for group(%d)", i);
        for (int j = 0; j < SOC_GDMA_PAIRS_PER_GROUP && search_code; j++) { // loop to search pair
            pair = gdma_acquire_pair_handle(group, j);
            ESP_GOTO_ON_FALSE(pair, ESP_ERR_NO_MEM, err, TAG, "no mem for pair(%d,%d)", i, j);
            portENTER_CRITICAL(&pair->spinlock);
            if (!(search_code & pair->occupy_code)) { // pair has suitable position for acquired channel(s)
                pair->occupy_code |= search_code;
                search_code = 0; // exit search loop
            }
            portEXIT_CRITICAL(&pair->spinlock);
            if (search_code) {
                gdma_release_pair_handle(pair);
                pair = NULL;
            }
        } // loop used to search pair
        if (search_code) {
            gdma_release_group_handle(group);
            group = NULL;
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

esp_err_t gdma_del_channel(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    ret = dma_chan->del(dma_chan); // call `gdma_del_tx_channel` or `gdma_del_rx_channel`

err:
    return ret;
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
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(dma_chan->periph_id == GDMA_INVALID_PERIPH_TRIG, ESP_ERR_INVALID_STATE, TAG, "channel is using by peripheral: %d", dma_chan->periph_id);
    pair = dma_chan->pair;
    group = pair->group;
    bool periph_conflict = false;

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
        if (!periph_conflict) {
            gdma_ll_tx_reset_channel(group->hal.dev, pair->pair_id); // reset channel
            gdma_ll_tx_connect_to_periph(group->hal.dev, pair->pair_id, trig_periph.periph, trig_periph.instance_id);
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
        if (!periph_conflict) {
            gdma_ll_rx_reset_channel(group->hal.dev, pair->pair_id); // reset channel
            gdma_ll_rx_connect_to_periph(group->hal.dev, pair->pair_id, trig_periph.periph, trig_periph.instance_id);
        }
    }

    ESP_RETURN_ON_FALSE(!periph_conflict, ESP_ERR_INVALID_STATE, TAG, "peripheral %d is already used by another channel", trig_periph.instance_id);
    dma_chan->periph_id = trig_periph.instance_id;
    return ESP_OK;
}

esp_err_t gdma_disconnect(gdma_channel_handle_t dma_chan)
{
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(dma_chan->periph_id != GDMA_INVALID_PERIPH_TRIG, ESP_ERR_INVALID_STATE, TAG, "no peripheral is connected to the channel");

    pair = dma_chan->pair;
    group = pair->group;
    int save_periph_id = dma_chan->periph_id;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        if (save_periph_id >= 0) {
            portENTER_CRITICAL(&group->spinlock);
            group->tx_periph_in_use_mask &= ~(1 << save_periph_id);
            portEXIT_CRITICAL(&group->spinlock);
        }
        gdma_ll_tx_disconnect_from_periph(group->hal.dev, pair->pair_id);
    } else {
        if (save_periph_id >= 0) {
            portENTER_CRITICAL(&group->spinlock);
            group->rx_periph_in_use_mask &= ~(1 << save_periph_id);
            portEXIT_CRITICAL(&group->spinlock);
        }
        gdma_ll_rx_disconnect_from_periph(group->hal.dev, pair->pair_id);
    }

    dma_chan->periph_id = GDMA_INVALID_PERIPH_TRIG;
    return ESP_OK;
}

esp_err_t gdma_get_free_m2m_trig_id_mask(gdma_channel_handle_t dma_chan, uint32_t *mask)
{
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(dma_chan && mask, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    uint32_t free_mask = GDMA_LL_M2M_FREE_PERIPH_ID_MASK;
    pair = dma_chan->pair;
    group = pair->group;

    portENTER_CRITICAL(&group->spinlock);
    free_mask &= ~(group->tx_periph_in_use_mask);
    free_mask &= ~(group->rx_periph_in_use_mask);
    portEXIT_CRITICAL(&group->spinlock);

    *mask = free_mask;
    return ESP_OK;
}

esp_err_t gdma_set_transfer_ability(gdma_channel_handle_t dma_chan, const gdma_transfer_ability_t *ability)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    bool en_burst = true;
    ESP_GOTO_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;
    size_t sram_alignment = ability->sram_trans_align;
    size_t psram_alignment = ability->psram_trans_align;
    // alignment should be 2^n
    ESP_GOTO_ON_FALSE((sram_alignment & (sram_alignment - 1)) == 0, ESP_ERR_INVALID_ARG, err, TAG, "invalid sram alignment: %zu", sram_alignment);

#if SOC_GDMA_SUPPORT_PSRAM
    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_TYPE_DATA);
    int block_size_index = 0;
    switch (psram_alignment) {
    case 64: // 64 Bytes alignment
        block_size_index = GDMA_LL_EXT_MEM_BK_SIZE_64B;
        break;
    case 32: // 32 Bytes alignment
        block_size_index = GDMA_LL_EXT_MEM_BK_SIZE_32B;
        break;
    case 16: // 16 Bytes alignment
        block_size_index = GDMA_LL_EXT_MEM_BK_SIZE_16B;
        break;
    case 0: // no alignment is requirement
        block_size_index = GDMA_LL_EXT_MEM_BK_SIZE_16B;
        psram_alignment = data_cache_line_size; // fall back to use the same size of the psram data cache line size
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid psram alignment: %zu", psram_alignment);
        break;
    }
    ESP_GOTO_ON_FALSE(((psram_alignment % data_cache_line_size) == 0), ESP_ERR_INVALID_ARG, err, TAG, "psram alignment (%d)B should be multiple of the data cache line size (%d)B", psram_alignment, data_cache_line_size);
#endif // #if SOC_GDMA_SUPPORT_PSRAM

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        // TX channel can always enable burst mode, no matter data alignment
        gdma_ll_tx_enable_data_burst(group->hal.dev, pair->pair_id, true);
        gdma_ll_tx_enable_descriptor_burst(group->hal.dev, pair->pair_id, true);
#if SOC_GDMA_SUPPORT_PSRAM
        gdma_ll_tx_set_block_size_psram(group->hal.dev, pair->pair_id, block_size_index);
#endif // #if SOC_GDMA_SUPPORT_PSRAM
    } else {
        // RX channel burst mode depends on specific data alignment
        en_burst = sram_alignment >= 4;
        gdma_ll_rx_enable_data_burst(group->hal.dev, pair->pair_id, en_burst);
        gdma_ll_rx_enable_descriptor_burst(group->hal.dev, pair->pair_id, en_burst);
#if SOC_GDMA_SUPPORT_PSRAM
        gdma_ll_rx_set_block_size_psram(group->hal.dev, pair->pair_id, block_size_index);
#endif // #if SOC_GDMA_SUPPORT_PSRAM
    }

    dma_chan->sram_alignment = sram_alignment;
    dma_chan->psram_alignment = psram_alignment;
    ESP_LOGD(TAG, "%s channel (%d,%d), (%u:%u) bytes aligned, burst %s", dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX ? "tx" : "rx",
             group->group_id, pair->pair_id, sram_alignment, psram_alignment, en_burst ? "enabled" : "disabled");
err:
    return ret;
}

esp_err_t gdma_apply_strategy(gdma_channel_handle_t dma_chan, const gdma_strategy_config_t *config)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_GOTO_ON_FALSE(dma_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        gdma_ll_tx_enable_owner_check(group->hal.dev, pair->pair_id, config->owner_check);
        gdma_ll_tx_enable_auto_write_back(group->hal.dev, pair->pair_id, config->auto_update_desc);
    } else {
        gdma_ll_rx_enable_owner_check(group->hal.dev, pair->pair_id, config->owner_check);
    }

err:
    return ret;
}

esp_err_t gdma_set_priority(gdma_channel_handle_t dma_chan, uint32_t priority)
{
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE(dma_chan && priority <= GDMA_LL_CHANNEL_MAX_PRIORITY, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        gdma_ll_tx_set_priority(group->hal.dev, pair->pair_id, priority);
    } else {
        gdma_ll_rx_set_priority(group->hal.dev, pair->pair_id, priority);
    }

    return ESP_OK;

}

esp_err_t gdma_register_tx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_tx_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_GOTO_ON_FALSE(dma_chan && dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;
    gdma_tx_channel_t *tx_chan = __containerof(dma_chan, gdma_tx_channel_t, base);

#if CONFIG_GDMA_ISR_IRAM_SAFE
    if (cbs->on_trans_eof) {
        ESP_GOTO_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_eof), ESP_ERR_INVALID_ARG, err, TAG, "on_trans_eof not in IRAM");
    }
    if (user_data) {
        ESP_GOTO_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, err, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_GDMA_ISR_IRAM_SAFE

    // lazy install interrupt service
    ESP_GOTO_ON_ERROR(gdma_install_tx_interrupt(tx_chan), err, TAG, "install interrupt service failed");

    // enable/disable GDMA interrupt events for TX channel
    portENTER_CRITICAL(&pair->spinlock);
    gdma_ll_tx_enable_interrupt(group->hal.dev, pair->pair_id, GDMA_LL_EVENT_TX_EOF, cbs->on_trans_eof != NULL);
    portEXIT_CRITICAL(&pair->spinlock);

    tx_chan->on_trans_eof = cbs->on_trans_eof;
    tx_chan->user_data = user_data;

    ESP_GOTO_ON_ERROR(esp_intr_enable(dma_chan->intr), err, TAG, "enable interrupt failed");

err:
    return ret;
}

esp_err_t gdma_register_rx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_rx_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_GOTO_ON_FALSE(dma_chan && dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;
    gdma_rx_channel_t *rx_chan = __containerof(dma_chan, gdma_rx_channel_t, base);

#if CONFIG_GDMA_ISR_IRAM_SAFE
    if (cbs->on_recv_eof) {
        ESP_GOTO_ON_FALSE(esp_ptr_in_iram(cbs->on_recv_eof), ESP_ERR_INVALID_ARG, err, TAG, "on_recv_eof not in IRAM");
    }
    if (user_data) {
        ESP_GOTO_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, err, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_GDMA_ISR_IRAM_SAFE

    // lazy install interrupt service
    ESP_GOTO_ON_ERROR(gdma_install_rx_interrupt(rx_chan), err, TAG, "install interrupt service failed");

    // enable/disable GDMA interrupt events for RX channel
    portENTER_CRITICAL(&pair->spinlock);
    gdma_ll_rx_enable_interrupt(group->hal.dev, pair->pair_id, GDMA_LL_EVENT_RX_SUC_EOF, cbs->on_recv_eof != NULL);
    portEXIT_CRITICAL(&pair->spinlock);

    rx_chan->on_recv_eof = cbs->on_recv_eof;
    rx_chan->user_data = user_data;

    ESP_GOTO_ON_ERROR(esp_intr_enable(dma_chan->intr), err, TAG, "enable interrupt failed");

err:
    return ret;
}

esp_err_t gdma_start(gdma_channel_handle_t dma_chan, intptr_t desc_base_addr)
{
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(dma_chan->flags.start_stop_by_etm == false, ESP_ERR_INVALID_STATE, TAG, "channel is controlled by ETM");
    pair = dma_chan->pair;
    group = pair->group;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_set_desc_addr(group->hal.dev, pair->pair_id, desc_base_addr);
        gdma_ll_rx_start(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_set_desc_addr(group->hal.dev, pair->pair_id, desc_base_addr);
        gdma_ll_tx_start(group->hal.dev, pair->pair_id);
    }
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_stop(gdma_channel_handle_t dma_chan)
{
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_RETURN_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(dma_chan->flags.start_stop_by_etm == false, ESP_ERR_INVALID_STATE, TAG, "channel is controlled by ETM");
    pair = dma_chan->pair;
    group = pair->group;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_stop(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_stop(group->hal.dev, pair->pair_id);
    }
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

    return ESP_OK;
}

esp_err_t gdma_append(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_GOTO_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_restart(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_restart(group->hal.dev, pair->pair_id);
    }
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

err:
    return ret;
}

esp_err_t gdma_reset(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    ESP_GOTO_ON_FALSE_ISR(dma_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    pair = dma_chan->pair;
    group = pair->group;

    portENTER_CRITICAL_SAFE(&dma_chan->spinlock);
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_reset_channel(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_reset_channel(group->hal.dev, pair->pair_id);
    }
    portEXIT_CRITICAL_SAFE(&dma_chan->spinlock);

err:
    return ret;
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
        s_platform.groups[group_id] = NULL; // deregister from platfrom
        gdma_ll_enable_clock(group->hal.dev, false);
        periph_module_disable(gdma_periph_signals.groups[group_id].module);
    }
    portEXIT_CRITICAL(&s_platform.spinlock);

    if (do_deinitialize) {
        free(group);
        ESP_LOGD(TAG, "del group %d", group_id);
    }
}

static gdma_group_t *gdma_acquire_group_handle(int group_id)
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
        group->group_id = group_id;
        group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        periph_module_enable(gdma_periph_signals.groups[group_id].module); // enable APB to access GDMA registers
        gdma_hal_init(&group->hal, group_id);       // initialize HAL context
        gdma_ll_enable_clock(group->hal.dev, true); // enable gdma clock
    } else {
        group = s_platform.groups[group_id];
    }
    // someone acquired the group handle means we have a new object that refer to this group
    s_platform.group_ref_counts[group_id]++;
    portEXIT_CRITICAL(&s_platform.spinlock);

    if (new_group) {
        ESP_LOGD(TAG, "new group (%d) at %p", group->group_id, group);
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
        group->pairs[pair_id] = pair; // register to group
        pair->group = group;
        pair->pair_id = pair_id;
        pair->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    } else {
        pair = group->pairs[pair_id];
    }
    // someone acquired the pair handle means we have a new object that refer to this pair
    group->pair_ref_counts[pair_id]++;
    portEXIT_CRITICAL(&group->spinlock);

    if (new_pair) {
        portENTER_CRITICAL(&s_platform.spinlock);
        s_platform.group_ref_counts[group->group_id]++; // pair obtains a reference to group
        portEXIT_CRITICAL(&s_platform.spinlock);

#if CONFIG_PM_ENABLE && SOC_PAU_SUPPORTED
        gdma_sleep_retention_init(group->group_id, pair->pair_id);
#endif
        ESP_LOGD(TAG, "new pair (%d,%d) at %p", group->group_id, pair->pair_id, pair);
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
        gdma_ll_tx_enable_interrupt(group->hal.dev, pair_id, UINT32_MAX, false); // disable all interupt events
        gdma_ll_tx_clear_interrupt_status(group->hal.dev, pair_id, UINT32_MAX);  // clear all pending events
        portEXIT_CRITICAL(&pair->spinlock);
        ESP_LOGD(TAG, "uninstall interrupt service for tx channel (%d,%d)", group_id, pair_id);
    }

    gdma_ll_tx_set_priority(group->hal.dev, pair_id, 0); // reset the priority to 0 (lowest)

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
        gdma_ll_rx_enable_interrupt(group->hal.dev, pair_id, UINT32_MAX, false); // disable all interupt events
        gdma_ll_rx_clear_interrupt_status(group->hal.dev, pair_id, UINT32_MAX);  // clear all pending events
        portEXIT_CRITICAL(&pair->spinlock);
        ESP_LOGD(TAG, "uninstall interrupt service for rx channel (%d,%d)", group_id, pair_id);
    }

    gdma_ll_rx_set_priority(group->hal.dev, pair_id, 0); // reset the priority to 0 (lowest)

    free(rx_chan);
    ESP_LOGD(TAG, "del rx channel (%d,%d)", group_id, pair_id);
    gdma_release_pair_handle(pair);
    return ESP_OK;
}

static void IRAM_ATTR gdma_default_rx_isr(void *args)
{
    gdma_rx_channel_t *rx_chan = (gdma_rx_channel_t *)args;
    gdma_pair_t *pair = rx_chan->base.pair;
    gdma_group_t *group = pair->group;
    bool need_yield = false;
    // clear pending interrupt event
    uint32_t intr_status = gdma_ll_rx_get_interrupt_status(group->hal.dev, pair->pair_id);
    gdma_ll_rx_clear_interrupt_status(group->hal.dev, pair->pair_id, intr_status);

    if (intr_status & GDMA_LL_EVENT_RX_SUC_EOF) {
        if (rx_chan->on_recv_eof) {
            uint32_t eof_addr = gdma_ll_rx_get_success_eof_desc_addr(group->hal.dev, pair->pair_id);
            gdma_event_data_t edata = {
                .rx_eof_desc_addr = eof_addr
            };
            if (rx_chan->on_recv_eof(&rx_chan->base, &edata, rx_chan->user_data)) {
                need_yield = true;
            }
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR gdma_default_tx_isr(void *args)
{
    gdma_tx_channel_t *tx_chan = (gdma_tx_channel_t *)args;
    gdma_pair_t *pair = tx_chan->base.pair;
    gdma_group_t *group = pair->group;
    bool need_yield = false;
    // clear pending interrupt event
    uint32_t intr_status = gdma_ll_tx_get_interrupt_status(group->hal.dev, pair->pair_id);
    gdma_ll_tx_clear_interrupt_status(group->hal.dev, pair->pair_id, intr_status);

    if (intr_status & GDMA_LL_EVENT_TX_EOF) {
        if (tx_chan && tx_chan->on_trans_eof) {
            uint32_t eof_addr = gdma_ll_tx_get_eof_desc_addr(group->hal.dev, pair->pair_id);
            gdma_event_data_t edata = {
                .tx_eof_desc_addr = eof_addr
            };
            if (tx_chan->on_trans_eof(&tx_chan->base, &edata, tx_chan->user_data)) {
                need_yield = true;
            }
        }
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
    // pre-alloc a interrupt handle, with handler disabled
    int isr_flags = GDMA_INTR_ALLOC_FLAGS;
#if SOC_GDMA_TX_RX_SHARE_INTERRUPT
    isr_flags |= ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
#endif
    intr_handle_t intr = NULL;
    ret = esp_intr_alloc_intrstatus(gdma_periph_signals.groups[group->group_id].pairs[pair->pair_id].rx_irq_id, isr_flags,
                                    (uint32_t)gdma_ll_rx_get_interrupt_status_reg(group->hal.dev, pair->pair_id), GDMA_LL_RX_EVENT_MASK,
                                    gdma_default_rx_isr, rx_chan, &intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    rx_chan->base.intr = intr;

    portENTER_CRITICAL(&pair->spinlock);
    gdma_ll_rx_enable_interrupt(group->hal.dev, pair->pair_id, UINT32_MAX, false); // disable all interupt events
    gdma_ll_rx_clear_interrupt_status(group->hal.dev, pair->pair_id, UINT32_MAX);  // clear all pending events
    portEXIT_CRITICAL(&pair->spinlock);
    ESP_LOGD(TAG, "install interrupt service for rx channel (%d,%d)", group->group_id, pair->pair_id);

err:
    return ret;
}

static esp_err_t gdma_install_tx_interrupt(gdma_tx_channel_t *tx_chan)
{
    esp_err_t ret = ESP_OK;
    gdma_pair_t *pair = tx_chan->base.pair;
    gdma_group_t *group = pair->group;
    // pre-alloc a interrupt handle, with handler disabled
    int isr_flags = GDMA_INTR_ALLOC_FLAGS;
#if SOC_GDMA_TX_RX_SHARE_INTERRUPT
    isr_flags |= ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
#endif
    intr_handle_t intr = NULL;
    ret = esp_intr_alloc_intrstatus(gdma_periph_signals.groups[group->group_id].pairs[pair->pair_id].tx_irq_id, isr_flags,
                                    (uint32_t)gdma_ll_tx_get_interrupt_status_reg(group->hal.dev, pair->pair_id), GDMA_LL_TX_EVENT_MASK,
                                    gdma_default_tx_isr, tx_chan, &intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc interrupt failed");
    tx_chan->base.intr = intr;

    portENTER_CRITICAL(&pair->spinlock);
    gdma_ll_tx_enable_interrupt(group->hal.dev, pair->pair_id, UINT32_MAX, false); // disable all interupt events
    gdma_ll_tx_clear_interrupt_status(group->hal.dev, pair->pair_id, UINT32_MAX);  // clear all pending events
    portEXIT_CRITICAL(&pair->spinlock);
    ESP_LOGD(TAG, "install interrupt service for tx channel (%d,%d)", group->group_id, pair->pair_id);

err:
    return ret;
}
