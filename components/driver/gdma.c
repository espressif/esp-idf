// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <stdlib.h>
#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "driver/periph_ctrl.h"
#include "esp_private/gdma.h"
#include "hal/gdma_hal.h"
#include "hal/gdma_ll.h"
#include "soc/gdma_periph.h"

static const char *TAG = "gdma";

#define DMA_CHECK(a, msg, tag, ret, ...)                                          \
    do {                                                                          \
        if (unlikely(!(a))) {                                                     \
            ESP_LOGE(TAG, "%s(%d): " msg, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret_code = ret;                                                       \
            goto tag;                                                             \
        }                                                                         \
    } while (0)

#define GDMA_INVALID_PERIPH_TRIG  (0x3F)
#define SEARCH_REQUEST_RX_CHANNEL (1 << 0)
#define SEARCH_REQUEST_TX_CHANNEL (1 << 1)

typedef struct gdma_platform_t gdma_platform_t;
typedef struct gdma_group_t gdma_group_t;
typedef struct gdma_pair_t gdma_pair_t;
typedef struct gdma_channel_t gdma_channel_t;
typedef struct gdma_tx_channel_t gdma_tx_channel_t;
typedef struct gdma_rx_channel_t gdma_rx_channel_t;

/**
 * GDMA driver consists of there object class, namely: Group, Pair and Channel.
 * Channel is allocated when user calls `gdma_new_channel`, its lifecycle is maintained by user.
 * Pair and Group are all lazy allocated, their life cycles are maintained by this driver.
 * We use reference count to track their life cycles, i.e. the driver will free their memory only when their reference count reached to 0.
 *
 * We don't use an all-in-one spin lock in this driver, instead, we created different spin locks at different level.
 * For platform, it has a spinlock, which is used to protect the group handle slots and reference count of each group.
 * For group, it has a spinlock, which is used to protect group level stuffs, e.g. hal object, pair handle slots and reference count of each pair.
 * For pair, it has a sinlock, which is used to protect pair level stuffs, e.g. interrupt handle, channel handle slots, occupy code.
 */

struct gdma_platform_t {
    portMUX_TYPE spinlock;                 // platform level spinlock
    gdma_group_t *groups[SOC_GDMA_GROUPS]; // array of GDMA group instances
    int group_ref_counts[SOC_GDMA_GROUPS]; // reference count used to protect group install/uninstall
};

struct gdma_group_t {
    int group_id;           // Group ID, index from 0
    gdma_hal_context_t hal; // HAL instance is at group level
    portMUX_TYPE spinlock;  // group level spinlock
    gdma_pair_t *pairs[SOC_GDMA_PAIRS_PER_GROUP];  // handles of GDMA pairs
    int pair_ref_counts[SOC_GDMA_PAIRS_PER_GROUP]; // reference count used to protect pair install/uninstall
};

struct gdma_pair_t {
    gdma_group_t *group;        // which group the pair belongs to
    int pair_id;                // Pair ID, index from 0
    gdma_tx_channel_t *tx_chan; // pointer of tx channel in the pair
    gdma_rx_channel_t *rx_chan; // pointer of rx channel in the pair
    int occupy_code;            // each bit indicates which channel has been occupied (an occupied channel will be skipped during channel search)
    intr_handle_t intr;         // Interrupt is at pair level
    portMUX_TYPE spinlock;      // pair level spinlock
};

struct gdma_channel_t {
    gdma_pair_t *pair;  // which pair the channel belongs to
    gdma_channel_direction_t direction; // channel direction
    int periph_id; // Peripheral instance ID, indicates which peripheral is connected to this GDMA channel
    esp_err_t (*del)(gdma_channel_t *channel); // channel deletion function, it's polymorphic, see `gdma_del_tx_channel` or `gdma_del_rx_channel`
};

struct gdma_tx_channel_t {
    gdma_channel_t base; // GDMA channel, base class
    void *user_data;     // user registered DMA event data
    gdma_event_callback_t on_trans_eof; // TX EOF callback
};

struct gdma_rx_channel_t {
    gdma_channel_t base; // GDMA channel, base class
    void *user_data;     // user registered DMA event data
    gdma_event_callback_t on_recv_eof; // RX EOF callback
};

static gdma_group_t *gdma_acquire_group_handle(int group_id);
static void gdma_release_group_handle(gdma_group_t *group);
static gdma_pair_t *gdma_acquire_pair_handle(gdma_group_t *group, int pair_id);
static void gdma_release_pair_handle(gdma_pair_t *pair);
static void gdma_uninstall_group(gdma_group_t *group);
static void gdma_uninstall_pair(gdma_pair_t *pair);
static esp_err_t gdma_del_tx_channel(gdma_channel_t *dma_channel);
static esp_err_t gdma_del_rx_channel(gdma_channel_t *dma_channel);
static esp_err_t gdma_install_interrupt(gdma_pair_t *pair);

// gdma driver platform
static gdma_platform_t s_platform = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
    .groups = {} // groups will be lazy installed
};

esp_err_t gdma_new_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
{
    esp_err_t ret_code = ESP_OK;
    gdma_tx_channel_t *alloc_tx_channel = NULL;
    gdma_rx_channel_t *alloc_rx_channel = NULL;
    int search_code = 0;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(config && ret_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);

    if (config->flags.reserve_sibling) {
        search_code = SEARCH_REQUEST_RX_CHANNEL | SEARCH_REQUEST_TX_CHANNEL; // search for a pair of channels
    }
    if (config->direction == GDMA_CHANNEL_DIRECTION_TX) {
        search_code |= SEARCH_REQUEST_TX_CHANNEL; // search TX only
        alloc_tx_channel = calloc(1, sizeof(gdma_tx_channel_t));
        DMA_CHECK(alloc_tx_channel, "no mem for gdma tx channel", err, ESP_ERR_NO_MEM);
    } else if (config->direction == GDMA_CHANNEL_DIRECTION_RX) {
        search_code |= SEARCH_REQUEST_RX_CHANNEL; // search RX only
        alloc_rx_channel = calloc(1, sizeof(gdma_rx_channel_t));
        DMA_CHECK(alloc_rx_channel, "no mem for gdma rx channel", err, ESP_ERR_NO_MEM);
    }

    if (config->sibling_chan) {
        pair = config->sibling_chan->pair;
        DMA_CHECK(pair, "invalid sibling channel", err, ESP_ERR_INVALID_ARG);
        DMA_CHECK(config->sibling_chan->direction != config->direction,
                  "sibling channel should have a different direction", err, ESP_ERR_INVALID_ARG);
        group = pair->group;
        portENTER_CRITICAL(&group->spinlock);
        group->pair_ref_counts[pair->pair_id]++; // channel obtains a reference to pair
        portEXIT_CRITICAL(&group->spinlock);
        goto search_done; // skip the search path below if user has specify a sibling channel
    }

    for (int i = 0; i < SOC_GDMA_GROUPS && search_code; i++) { // loop to search group
        group = gdma_acquire_group_handle(i);
        for (int j = 0; j < SOC_GDMA_PAIRS_PER_GROUP && search_code && group; j++) { // loop to search pair
            pair = gdma_acquire_pair_handle(group, j);
            if (pair) {
                portENTER_CRITICAL(&pair->spinlock);
                if (!(search_code & pair->occupy_code)) { // pair has suitable position for acquired channel(s)
                    pair->occupy_code |= search_code;
                    search_code = 0; // exit search loop
                }
                portEXIT_CRITICAL(&pair->spinlock);
                if (!search_code) {
                    portENTER_CRITICAL(&group->spinlock);
                    group->pair_ref_counts[j]++; // channel obtains a reference to pair
                    portEXIT_CRITICAL(&group->spinlock);
                }
            }
            gdma_release_pair_handle(pair);
        } // loop used to search pair
        gdma_release_group_handle(group);
    } // loop used to search group
    DMA_CHECK(search_code == 0, "no free gdma channel, search code=%d", err, ESP_ERR_NOT_FOUND, search_code);

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
    return ret_code;
}

esp_err_t gdma_del_channel(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret_code = ESP_OK;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);

    ret_code = dma_chan->del(dma_chan); // call `gdma_del_tx_channel` or `gdma_del_rx_channel`

err:
    return ret_code;
}

esp_err_t gdma_get_channel_id(gdma_channel_handle_t dma_chan, int *channel_id)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    *channel_id = pair->pair_id;
err:
    return ret_code;
}

esp_err_t gdma_connect(gdma_channel_handle_t dma_chan, gdma_trigger_t trig_periph)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    DMA_CHECK(dma_chan->periph_id == GDMA_INVALID_PERIPH_TRIG, "channel is using by peripheral: %d", err, ESP_ERR_INVALID_STATE, dma_chan->periph_id);
    pair = dma_chan->pair;
    group = pair->group;

    dma_chan->periph_id = trig_periph.instance_id;
    // enable/disable m2m mode
    gdma_ll_enable_m2m_mode(group->hal.dev, pair->pair_id, trig_periph.periph == GDMA_TRIG_PERIPH_M2M);

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        gdma_ll_tx_reset_channel(group->hal.dev, pair->pair_id); // reset channel
        if (trig_periph.periph != GDMA_TRIG_PERIPH_M2M) {
            gdma_ll_tx_connect_to_periph(group->hal.dev, pair->pair_id, trig_periph.instance_id);
        }
    } else {
        gdma_ll_rx_reset_channel(group->hal.dev, pair->pair_id); // reset channel
        if (trig_periph.periph != GDMA_TRIG_PERIPH_M2M) {
            gdma_ll_rx_connect_to_periph(group->hal.dev, pair->pair_id, trig_periph.instance_id);
        }
    }

err:
    return ret_code;
}

esp_err_t gdma_disconnect(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    DMA_CHECK(dma_chan->periph_id != GDMA_INVALID_PERIPH_TRIG, "no peripheral is connected to the channel", err, ESP_ERR_INVALID_STATE);
    pair = dma_chan->pair;
    group = pair->group;

    dma_chan->periph_id = GDMA_INVALID_PERIPH_TRIG;
    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        gdma_ll_tx_connect_to_periph(group->hal.dev, pair->pair_id, GDMA_INVALID_PERIPH_TRIG);
    } else {
        gdma_ll_rx_connect_to_periph(group->hal.dev, pair->pair_id, GDMA_INVALID_PERIPH_TRIG);
    }

err:
    return ret_code;
}

esp_err_t gdma_apply_strategy(gdma_channel_handle_t dma_chan, const gdma_strategy_config_t *config)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    group = pair->group;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX) {
        gdma_ll_tx_enable_owner_check(group->hal.dev, pair->pair_id, config->owner_check);
        gdma_ll_tx_enable_auto_write_back(group->hal.dev, pair->pair_id, config->auto_update_desc);
    } else {
        gdma_ll_rx_enable_owner_check(group->hal.dev, pair->pair_id, config->owner_check);
    }

err:
    return ret_code;
}

esp_err_t gdma_register_tx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_tx_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan && dma_chan->direction == GDMA_CHANNEL_DIRECTION_TX, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    group = pair->group;
    gdma_tx_channel_t *tx_chan = __containerof(dma_chan, gdma_tx_channel_t, base);

    // lazy install interrupt service
    DMA_CHECK(gdma_install_interrupt(pair) == ESP_OK, "install interrupt service failed", err, ESP_FAIL);

    // enable/disable GDMA interrupt events for TX channel
    portENTER_CRITICAL(&pair->spinlock);
    gdma_ll_enable_interrupt(group->hal.dev, pair->pair_id, GDMA_LL_EVENT_TX_EOF, cbs->on_trans_eof != NULL);
    portEXIT_CRITICAL(&pair->spinlock);

    tx_chan->on_trans_eof = cbs->on_trans_eof;
    tx_chan->user_data = user_data;

    DMA_CHECK(esp_intr_enable(pair->intr) == ESP_OK, "enable interrupt failed", err, ESP_FAIL);

err:
    return ret_code;
}

esp_err_t gdma_register_rx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_rx_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan && dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    group = pair->group;
    gdma_rx_channel_t *rx_chan = __containerof(dma_chan, gdma_rx_channel_t, base);

    // lazy install interrupt service
    DMA_CHECK(gdma_install_interrupt(pair) == ESP_OK, "install interrupt service failed", err, ESP_FAIL);

    // enable/disable GDMA interrupt events for RX channel
    portENTER_CRITICAL(&pair->spinlock);
    gdma_ll_enable_interrupt(group->hal.dev, pair->pair_id, GDMA_LL_EVENT_RX_SUC_EOF, cbs->on_recv_eof != NULL);
    portEXIT_CRITICAL(&pair->spinlock);

    rx_chan->on_recv_eof = cbs->on_recv_eof;
    rx_chan->user_data = user_data;

    DMA_CHECK(esp_intr_enable(pair->intr) == ESP_OK, "enable interrupt failed", err, ESP_FAIL);

err:
    return ret_code;
}

esp_err_t gdma_start(gdma_channel_handle_t dma_chan, intptr_t desc_base_addr)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    group = pair->group;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_set_desc_addr(group->hal.dev, pair->pair_id, desc_base_addr);
        gdma_ll_rx_start(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_set_desc_addr(group->hal.dev, pair->pair_id, desc_base_addr);
        gdma_ll_tx_start(group->hal.dev, pair->pair_id);
    }

err:
    return ret_code;
}

esp_err_t gdma_stop(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    group = pair->group;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_stop(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_stop(group->hal.dev, pair->pair_id);
    }

err:
    return ret_code;
}

esp_err_t gdma_append(gdma_channel_handle_t dma_chan)
{
    esp_err_t ret_code = ESP_OK;
    gdma_pair_t *pair = NULL;
    gdma_group_t *group = NULL;
    DMA_CHECK(dma_chan, "invalid argument", err, ESP_ERR_INVALID_ARG);
    pair = dma_chan->pair;
    group = pair->group;

    if (dma_chan->direction == GDMA_CHANNEL_DIRECTION_RX) {
        gdma_ll_rx_restart(group->hal.dev, pair->pair_id);
    } else {
        gdma_ll_tx_restart(group->hal.dev, pair->pair_id);
    }

err:
    return ret_code;
}

static void gdma_uninstall_group(gdma_group_t *group)
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
    gdma_group_t *pre_alloc_group = calloc(1, sizeof(gdma_group_t));
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

static void gdma_release_group_handle(gdma_group_t *group)
{
    if (group) {
        gdma_uninstall_group(group);
    }
}

static void gdma_uninstall_pair(gdma_pair_t *pair)
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
        if (pair->intr) {
            // disable interrupt handler (but not freed, esp_intr_free is a blocking API, we can't use it in a critical section)
            esp_intr_disable(pair->intr);
            gdma_ll_enable_interrupt(group->hal.dev, pair->pair_id, UINT32_MAX, false); // disable all interupt events
            gdma_ll_clear_interrupt_status(group->hal.dev, pair->pair_id, UINT32_MAX);  // clear all pending events
        }
    }
    portEXIT_CRITICAL(&group->spinlock);

    if (do_deinitialize) {
        if (pair->intr) {
            esp_intr_free(pair->intr); // free interrupt resource
            ESP_LOGD(TAG, "uninstall interrupt service for pair (%d,%d)", group->group_id, pair_id);
        }
        free(pair);
        ESP_LOGD(TAG, "del pair (%d,%d)", group->group_id, pair_id);

        gdma_uninstall_group(group);
    }
}

static gdma_pair_t *gdma_acquire_pair_handle(gdma_group_t *group, int pair_id)
{
    bool new_pair = false;
    gdma_pair_t *pair = NULL;
    gdma_pair_t *pre_alloc_pair = calloc(1, sizeof(gdma_pair_t));
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
        ESP_LOGD(TAG, "new pair (%d,%d) at %p", group->group_id, pair->pair_id, pair);
    } else {
        free(pre_alloc_pair);
    }
out:
    return pair;
}

static void gdma_release_pair_handle(gdma_pair_t *pair)
{
    if (pair) {
        gdma_uninstall_pair(pair);
    }
}

static esp_err_t gdma_del_tx_channel(gdma_channel_t *dma_channel)
{
    gdma_pair_t *pair = dma_channel->pair;
    gdma_group_t *group = pair->group;
    gdma_tx_channel_t *tx_chan = __containerof(dma_channel, gdma_tx_channel_t, base);
    portENTER_CRITICAL(&pair->spinlock);
    pair->tx_chan = NULL;
    pair->occupy_code &= ~SEARCH_REQUEST_TX_CHANNEL;
    portEXIT_CRITICAL(&pair->spinlock);

    ESP_LOGD(TAG, "del tx channel (%d,%d)", group->group_id, pair->pair_id);
    free(tx_chan);
    gdma_uninstall_pair(pair);
    return ESP_OK;
}

static esp_err_t gdma_del_rx_channel(gdma_channel_t *dma_channel)
{
    gdma_pair_t *pair = dma_channel->pair;
    gdma_group_t *group = pair->group;
    gdma_rx_channel_t *rx_chan = __containerof(dma_channel, gdma_rx_channel_t, base);
    portENTER_CRITICAL(&pair->spinlock);
    pair->rx_chan = NULL;
    pair->occupy_code &= ~SEARCH_REQUEST_RX_CHANNEL;
    portEXIT_CRITICAL(&pair->spinlock);

    ESP_LOGD(TAG, "del rx channel (%d,%d)", group->group_id, pair->pair_id);
    free(rx_chan);
    gdma_uninstall_pair(pair);
    return ESP_OK;
}

static void IRAM_ATTR gdma_default_isr(void *args)
{
    gdma_pair_t *pair = (gdma_pair_t *)args;
    gdma_group_t *group = pair->group;
    gdma_rx_channel_t *rx_chan = pair->rx_chan;
    gdma_tx_channel_t *tx_chan = pair->tx_chan;
    bool need_yield = false;
    // clear pending interrupt event
    uint32_t intr_status = gdma_ll_get_interrupt_status(group->hal.dev, pair->pair_id);
    gdma_ll_clear_interrupt_status(group->hal.dev, pair->pair_id, intr_status);

    if (intr_status & GDMA_LL_EVENT_RX_SUC_EOF) {
        if (rx_chan && rx_chan->on_recv_eof) {
            uint32_t eof_addr = gdma_ll_rx_get_success_eof_desc_addr(group->hal.dev, pair->pair_id);
            gdma_event_data_t edata = {
                .rx_eof_desc_addr = eof_addr
            };
            if (rx_chan->on_recv_eof(&rx_chan->base, &edata, rx_chan->user_data)) {
                need_yield = true;
            }
        }
    }

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

static esp_err_t gdma_install_interrupt(gdma_pair_t *pair)
{
    esp_err_t ret_code = ESP_OK;
    gdma_group_t *group = pair->group;
    bool do_install_isr = false;
    // pre-alloc a interrupt handle, shared with other handle, with handler disabled
    // This is used to prevent potential concurrency between interrupt install and uninstall
    int isr_flags = ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED;
    intr_handle_t intr = NULL;
    ret_code = esp_intr_alloc(gdma_periph_signals.groups[group->group_id].pairs[pair->pair_id].irq_id, isr_flags, gdma_default_isr, pair, &intr);
    DMA_CHECK(ret_code == ESP_OK, "alloc interrupt failed", err, ret_code);

    if (!pair->intr) {
        portENTER_CRITICAL(&pair->spinlock);
        if (!pair->intr) {
            do_install_isr = true;
            pair->intr = intr;
            gdma_ll_enable_interrupt(group->hal.dev, pair->pair_id, UINT32_MAX, false); // disable all interupt events
            gdma_ll_clear_interrupt_status(group->hal.dev, pair->pair_id, UINT32_MAX);  // clear all pending events
        }
        portEXIT_CRITICAL(&pair->spinlock);
    }
    if (do_install_isr) {
        ESP_LOGD(TAG, "install interrupt service for pair (%d,%d)", group->group_id, pair->pair_id);
    } else {
        // interrupt handle has been installed before, so removed this one
        esp_intr_free(intr);
    }

err:
    return ret_code;
}
