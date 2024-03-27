/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/queue.h>
#include <sys/lock.h>
#include "esp_check.h"
#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_private/periph_ctrl.h"
#include "dma2d_priv.h"
#include "esp_private/dma2d.h"
#include "hal/dma2d_hal.h"
#include "hal/dma2d_ll.h"
#include "soc/dma2d_channel.h"
#include "soc/dma2d_periph.h"
#include "soc/soc_caps.h"
#include "esp_bit_defs.h"

/**
 * The 2D-DMA driver is designed with a pool & client model + queue design pattern.
 *
 * Pools represents the groups of 2D-DMA module, which contain the limited resource, channels.
 * Clients represents the upper modules which are the consumers of the 2D-DMA channels, such as JPEG and PPA.
 *
 * Each pool has a queue to store the 2D-DMA transactions that are waiting to be processed.
 *
 * The upper modules should register themselves as the clients to a 2D-DMA pool. And then they should push the
 * 2D-DMA transactions into the pool queue. The driver will continuously look for the desired resources from the pool to
 * complete the transactions.
 */

static const char *TAG = "dma2d";

typedef struct dma2d_platform_t {
    _lock_t mutex;                              // platform level mutex lock to protect the dma2d_acquire_pool/dma2d_release_pool process
    dma2d_group_t *groups[SOC_DMA2D_GROUPS];    // array of 2D-DMA group instances
    int group_ref_counts[SOC_DMA2D_GROUPS];     // reference count used to protect group install/uninstall
} dma2d_platform_t;

// 2D-DMA driver platform
static dma2d_platform_t s_platform = {
    .groups = {},
};

// extern 2D-DMA channel reserved mask variables to be ORed in the constructors
uint32_t dma2d_tx_channel_reserved_mask[SOC_DMA2D_GROUPS] = { [0 ... SOC_DMA2D_GROUPS - 1] = 0 };
uint32_t dma2d_rx_channel_reserved_mask[SOC_DMA2D_GROUPS] = { [0 ... SOC_DMA2D_GROUPS - 1] = 0 };

// The most number of channels required for a 2D-DMA transaction (a PPA Blend operation requires 2 TX + 1 RX)
#define DMA2D_MAX_CHANNEL_NUM_PER_TRANSACTION   3


/* This static function is not thread-safe, group's spinlock protection should be added in its caller */
static bool acquire_free_channels_for_trans(dma2d_group_t *dma2d_group, const dma2d_trans_config_t *trans_desc, dma2d_trans_channel_info_t *channel_handle_array)
{
    bool found = true;
    uint32_t idx = 0;
    uint32_t bundled_tx_channel_mask = 0;

    if (trans_desc->tx_channel_num > 0) {
        uint32_t tx_free_channel_mask;
        if (!trans_desc->specified_tx_channel_mask) {
            tx_free_channel_mask = dma2d_group->tx_channel_free_mask;
            tx_free_channel_mask &= (((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_TX_REORDER) ? DMA2D_LL_TX_CHANNEL_SUPPORT_RO_MASK : UINT32_MAX) &
                                    ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC) ? DMA2D_LL_TX_CHANNEL_SUPPORT_CSC_MASK : UINT32_MAX));
            tx_free_channel_mask &= ~dma2d_group->tx_channel_reserved_mask;
            if (trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING) {
                uint32_t rx_channel_candidate = dma2d_group->rx_channel_free_mask &
                                                ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_REORDER) ? DMA2D_LL_RX_CHANNEL_SUPPORT_RO_MASK : UINT32_MAX) &
                                                ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC) ? DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK : UINT32_MAX) &
                                                ~dma2d_group->rx_channel_reserved_mask;
                tx_free_channel_mask &= rx_channel_candidate;
            }
            // As long as __builtin_popcount(tx_free_channel_mask) >= trans_desc->tx_channel_num, it can meet the criteria of "found"
        } else {
            tx_free_channel_mask = trans_desc->specified_tx_channel_mask & dma2d_group->tx_channel_free_mask;
            // tx_free_channel_mask need to be exactly equal to trans_desc->specified_tx_channel_mask to meet the criteria of "found"
        }

        for (int i = 0; i < trans_desc->tx_channel_num; i++) {
            if (tx_free_channel_mask) {
                int channel_id = 31 - __builtin_clz(tx_free_channel_mask); // channel 0 has the most features, acquire other channels first if possible
                tx_free_channel_mask &= ~(1 << channel_id);
                dma2d_group->tx_channel_free_mask &= ~(1 << channel_id);
                bundled_tx_channel_mask |= (1 << channel_id);
                // Record channel status
                memset(&dma2d_group->tx_chans[channel_id]->base.status, 0, sizeof(dma2d_group->tx_chans[channel_id]->base.status));
                dma2d_group->tx_chans[channel_id]->base.status.periph_sel_id = -1;
                if (trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_TX_REORDER) {
                    dma2d_group->tx_chans[channel_id]->base.status.reorder_en = true;
                }
                channel_handle_array[idx].chan = &dma2d_group->tx_chans[channel_id]->base;
                channel_handle_array[idx].dir = DMA2D_CHANNEL_DIRECTION_TX;
                idx++;
            } else {
                found = false;
                goto revert;
            }
        }
    }

    if (trans_desc->rx_channel_num > 0) {
        uint32_t rx_free_channel_mask;
        if (trans_desc->specified_rx_channel_mask) {
            rx_free_channel_mask = trans_desc->specified_rx_channel_mask & dma2d_group->rx_channel_free_mask;
            // rx_free_channel_mask need to be exactly equal to trans_desc->specified_rx_channel_mask to meet the criteria of "found"
        } else if (trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING) {
            // rx channel has already been determined
            rx_free_channel_mask = bundled_tx_channel_mask;
        } else {
            rx_free_channel_mask = dma2d_group->rx_channel_free_mask;
            rx_free_channel_mask &= (((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_REORDER) ? DMA2D_LL_RX_CHANNEL_SUPPORT_RO_MASK : UINT32_MAX) &
                                    ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC) ? DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK : UINT32_MAX));
            rx_free_channel_mask &= ~dma2d_group->rx_channel_reserved_mask;
            // As long as __builtin_popcount(rx_free_channel_mask) >= trans_desc->rx_channel_num, it can meet the criteria of "found"
        }

        // Requires one RX channel at most, no need a for loop
        if (rx_free_channel_mask) {
            int channel_id = 31 - __builtin_clz(rx_free_channel_mask); // channel 0 has full features, acquire other channels first if possible
            rx_free_channel_mask &= ~(1 << channel_id);
            dma2d_group->rx_channel_free_mask &= ~(1 << channel_id);
            // Record channel status
            memset(&dma2d_group->rx_chans[channel_id]->base.status, 0, sizeof(dma2d_group->rx_chans[channel_id]->base.status));
            dma2d_group->rx_chans[channel_id]->base.status.periph_sel_id = -1;
            if (trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_REORDER) {
                dma2d_group->rx_chans[channel_id]->base.status.reorder_en = true;
            }
            channel_handle_array[idx].chan = &dma2d_group->rx_chans[channel_id]->base;
            channel_handle_array[idx].dir = DMA2D_CHANNEL_DIRECTION_RX;
            idx++;

            // Record its bundled TX channels, to be freed in the isr
            dma2d_rx_channel_t *rx_chan = dma2d_group->rx_chans[channel_id];
            portENTER_CRITICAL_SAFE(&rx_chan->base.spinlock);
            rx_chan->bundled_tx_channel_mask = bundled_tx_channel_mask;
            portEXIT_CRITICAL_SAFE(&rx_chan->base.spinlock);
        } else {
            found = false;
            goto revert;
        }
    }

revert:
    if (!found) {
        for (int i = 0; i < idx; i++) {
            int free_channel_mask = (1 << channel_handle_array[i].chan->channel_id);
            if (channel_handle_array[i].dir == DMA2D_CHANNEL_DIRECTION_TX) {
                dma2d_group->tx_channel_free_mask |= free_channel_mask;
            } else {
                dma2d_group->rx_channel_free_mask |= free_channel_mask;
            }
        }
    }
    return found;
}

/* This function will free up the RX channel and its bundled TX channels, then check for whether there is next transaction to be picked up */
static bool free_up_channels(dma2d_group_t *group, dma2d_rx_channel_t *rx_chan)
{
    bool need_yield = false;
    uint32_t channel_id = rx_chan->base.channel_id;
    // 1. Clean up channels
    uint32_t bundled_tx_channel_mask = rx_chan->bundled_tx_channel_mask;
    uint32_t tx_periph_sel_id_mask = 0;
    uint32_t rx_periph_sel_id_mask = 0;
    // Disable RX channel interrupt
    portENTER_CRITICAL_SAFE(&rx_chan->base.spinlock);
    dma2d_ll_rx_enable_interrupt(group->hal.dev, channel_id, UINT32_MAX, false);
    // Reset RX channel event related pointers and flags
    rx_chan->on_recv_eof = NULL;
    rx_chan->on_desc_done = NULL;
    // Disconnect RX channel from the peripheral
    dma2d_ll_rx_disconnect_from_periph(group->hal.dev, channel_id);
    // Clear the pointer that points to the finished transaction
    rx_chan->base.status.transaction = NULL;
    // Record its periph_sel_id
    assert(rx_chan->base.status.periph_sel_id != -1);
    rx_periph_sel_id_mask |= (1 << rx_chan->base.status.periph_sel_id);
    portEXIT_CRITICAL_SAFE(&rx_chan->base.spinlock);
    // For every bundled TX channels:
    while (rx_chan->bundled_tx_channel_mask) {
        uint32_t nbit = __builtin_ffs(rx_chan->bundled_tx_channel_mask) - 1;
        rx_chan->bundled_tx_channel_mask &= ~(1 << nbit);
        dma2d_tx_channel_t *tx_chan = group->tx_chans[nbit];
        // Disable TX channel interrupt
        portENTER_CRITICAL_SAFE(&tx_chan->base.spinlock);
        dma2d_ll_tx_enable_interrupt(group->hal.dev, nbit, UINT32_MAX, false);
        // Reset TX channel event related pointers
        tx_chan->on_desc_done = NULL;
        // Disconnect TX channel from the peripheral
        dma2d_ll_tx_disconnect_from_periph(group->hal.dev, nbit);
        // Clear the pointer that points to the finished transaction
        tx_chan->base.status.transaction = NULL;
        // Record its periph_sel_id
        assert(tx_chan->base.status.periph_sel_id != -1);
        tx_periph_sel_id_mask |= (1 << tx_chan->base.status.periph_sel_id);
        portEXIT_CRITICAL_SAFE(&tx_chan->base.spinlock);
    }
    // Channel functionality flags will be reset and assigned new values inside `acquire_free_channels_for_trans`
    // Channel reset will always be done at `dma2d_connect` (i.e. when the channel is selected for a new transaction)

    // 2. Check if next pending transaction in the tailq can start
    bool channels_found = false;
    const dma2d_trans_config_t *next_trans = NULL;
    dma2d_trans_channel_info_t channel_handle_array[DMA2D_MAX_CHANNEL_NUM_PER_TRANSACTION];

    portENTER_CRITICAL_SAFE(&group->spinlock);
    // Release channels
    group->tx_channel_free_mask |= bundled_tx_channel_mask;
    group->rx_channel_free_mask |= (1 << channel_id);
    // Release M2M periph_sel_id
    group->tx_periph_m2m_free_id_mask |= (tx_periph_sel_id_mask & DMA2D_LL_TX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK);
    group->rx_periph_m2m_free_id_mask |= (rx_periph_sel_id_mask & DMA2D_LL_RX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK);

    dma2d_trans_t *next_trans_elm = TAILQ_FIRST(&group->pending_trans_tailq);
    if (next_trans_elm) {
        // There is a pending transaction
        next_trans = next_trans_elm->desc;
        channels_found = acquire_free_channels_for_trans(group, next_trans, channel_handle_array);
    }

    if (channels_found) {
        TAILQ_REMOVE(&group->pending_trans_tailq, next_trans_elm, entry);
    }
    portEXIT_CRITICAL_SAFE(&group->spinlock);

    if (channels_found) {
        // If the transaction can be processed, let consumer handle the transaction
        uint32_t total_channel_num = next_trans->tx_channel_num + next_trans->rx_channel_num;
        // Store the acquired rx_chan into trans_elm (dma2d_trans_t) in case upper driver later need it to call `dma2d_force_end`
        // Upper driver controls the life cycle of trans_elm
        for (int i = 0; i < total_channel_num; i++) {
            if (channel_handle_array[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
                next_trans_elm->rx_chan = channel_handle_array[i].chan;
            }
            // Also save the transaction pointer
            channel_handle_array[i].chan->status.transaction = next_trans_elm;
        }
        need_yield |= next_trans->on_job_picked(total_channel_num, channel_handle_array, next_trans->user_config);
    }
    return need_yield;
}

static NOINLINE_ATTR bool _dma2d_default_tx_isr(dma2d_group_t *group, int channel_id)
{
    bool need_yield = false;
    dma2d_tx_channel_t *tx_chan = group->tx_chans[channel_id];
    dma2d_event_data_t edata = {
        .transaction = tx_chan->base.status.transaction,
    };
    // Clear pending interrupt event
    uint32_t intr_status = dma2d_ll_tx_get_interrupt_status(group->hal.dev, channel_id);
    dma2d_ll_tx_clear_interrupt_status(group->hal.dev, channel_id, intr_status);

    // Handle callback
    if (intr_status & DMA2D_LL_EVENT_TX_DONE) {
        if (tx_chan->on_desc_done) {
            need_yield |= tx_chan->on_desc_done(&tx_chan->base, &edata, tx_chan->user_data);
        }
    }
    return need_yield;
}

static NOINLINE_ATTR bool _dma2d_default_rx_isr(dma2d_group_t *group, int channel_id)
{
    bool need_yield = false;
    dma2d_rx_channel_t *rx_chan = group->rx_chans[channel_id];
    dma2d_event_data_t edata = {
        .transaction = rx_chan->base.status.transaction,
    };
    // Clear pending interrupt event
    uint32_t intr_status = dma2d_ll_rx_get_interrupt_status(group->hal.dev, channel_id);
    dma2d_ll_rx_clear_interrupt_status(group->hal.dev, channel_id, intr_status);
    // Save RX channel EOF callback pointers temporarily, could be overwritten by new ones
    dma2d_event_callback_t on_recv_eof = rx_chan->on_recv_eof;
    void *user_data = rx_chan->user_data;
    uint32_t suc_eof_desc_addr = dma2d_ll_rx_get_success_eof_desc_addr(group->hal.dev, channel_id);

    // It is guaranteed in hardware that if SUC_EOF/ERR_EOF interrupt is raised, it will always be raised together with
    // RX_DONE interrupt at the same time.

    // On RX_DONE triggered, it may be an indication of partially done, call `on_desc_done` callback, allowing 2D-DMA
    // channel operations on the currently acquired channels. Channel may continue running again.
    if (intr_status & DMA2D_LL_EVENT_RX_DONE) {
        if (rx_chan->on_desc_done) {
            need_yield |= rx_chan->on_desc_done(&rx_chan->base, &edata, user_data);
        }
    }

    // If last transcation completes (regardless success or not), free the channels
    if (intr_status & (DMA2D_LL_EVENT_RX_SUC_EOF | DMA2D_LL_EVENT_RX_ERR_EOF | DMA2D_LL_EVENT_RX_DESC_ERROR)) {
        if (!(intr_status & DMA2D_LL_EVENT_RX_ERR_EOF)) {
            assert(dma2d_ll_rx_is_fsm_idle(group->hal.dev, channel_id));
        }
        need_yield |= free_up_channels(group, rx_chan);
    }

    // Handle last transaction's end callbacks (at this point, last transaction's channels are completely freed,
    // therefore, we don't pass in channel handle to the callbacks anymore)
    if (intr_status & DMA2D_LL_EVENT_RX_SUC_EOF) {
        if (on_recv_eof) {
            edata.rx_eof_desc_addr = suc_eof_desc_addr;
            need_yield |= on_recv_eof(NULL, &edata, user_data);
        }
    }
    return need_yield;
}

static void dma2d_default_isr(void *args)
{
    dma2d_channel_t *chan = (dma2d_channel_t *)args;
    dma2d_group_t *group = chan->group;

    bool need_yield = false;

    if (chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        need_yield |= _dma2d_default_tx_isr(group, chan->channel_id);
    } else {
        // For RX channel interrupt triggered, we need to check whether there is any interrupt triggered for the
        // bundled TX channels but hasn't been processed. If so, handle TX interrupt first.
        uint32_t bundled_tx_channel_mask = group->rx_chans[chan->channel_id]->bundled_tx_channel_mask;
        while (bundled_tx_channel_mask) {
            uint32_t chan_id = __builtin_ffs(bundled_tx_channel_mask) - 1;
            bundled_tx_channel_mask &= ~(1 << chan_id);
            need_yield |= _dma2d_default_tx_isr(group, chan_id);
        }

        need_yield |= _dma2d_default_rx_isr(group, chan->channel_id);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t dma2d_acquire_pool(const dma2d_pool_config_t *config, dma2d_pool_handle_t *ret_pool)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(config && ret_pool, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->pool_id < SOC_DMA2D_GROUPS, ESP_ERR_INVALID_ARG, TAG, "invalid pool_id");
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & ESP_INTR_FLAG_LOWMED, ESP_ERR_INVALID_ARG, TAG,
                            "invalid interrupt priority: %" PRIu32, config->intr_priority);
    }

    int group_id = config->pool_id; // A pool is referring to a module group in hardware

    _lock_acquire(&s_platform.mutex);
    if (!s_platform.groups[group_id]) {
        dma2d_group_t *pre_alloc_group = heap_caps_calloc(1, sizeof(dma2d_group_t), DMA2D_MEM_ALLOC_CAPS);
        dma2d_tx_channel_t *pre_alloc_tx_channels = heap_caps_calloc(SOC_DMA2D_TX_CHANNELS_PER_GROUP, sizeof(dma2d_tx_channel_t), DMA2D_MEM_ALLOC_CAPS);
        dma2d_rx_channel_t *pre_alloc_rx_channels = heap_caps_calloc(SOC_DMA2D_RX_CHANNELS_PER_GROUP, sizeof(dma2d_rx_channel_t), DMA2D_MEM_ALLOC_CAPS);
        if (pre_alloc_group && pre_alloc_tx_channels && pre_alloc_rx_channels) {
            pre_alloc_group->group_id = group_id;
            pre_alloc_group->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            TAILQ_INIT(&pre_alloc_group->pending_trans_tailq);
            pre_alloc_group->tx_channel_free_mask = (1 << SOC_DMA2D_TX_CHANNELS_PER_GROUP) - 1;
            pre_alloc_group->rx_channel_free_mask = (1 << SOC_DMA2D_RX_CHANNELS_PER_GROUP) - 1;
            pre_alloc_group->tx_channel_reserved_mask = dma2d_tx_channel_reserved_mask[group_id];
            pre_alloc_group->rx_channel_reserved_mask = dma2d_rx_channel_reserved_mask[group_id];
            pre_alloc_group->tx_periph_m2m_free_id_mask = DMA2D_LL_TX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK;
            pre_alloc_group->rx_periph_m2m_free_id_mask = DMA2D_LL_RX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK;
            pre_alloc_group->intr_priority = -1;
            for (int i = 0; i < SOC_DMA2D_TX_CHANNELS_PER_GROUP; i++) {
                pre_alloc_group->tx_chans[i] = &pre_alloc_tx_channels[i];
                dma2d_tx_channel_t *tx_chan = pre_alloc_group->tx_chans[i];
                tx_chan->base.group = pre_alloc_group;
                tx_chan->base.channel_id = i;
                tx_chan->base.direction = DMA2D_CHANNEL_DIRECTION_TX;
                tx_chan->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            }
            for (int i = 0; i < SOC_DMA2D_RX_CHANNELS_PER_GROUP; i++) {
                pre_alloc_group->rx_chans[i] = &pre_alloc_rx_channels[i];
                dma2d_rx_channel_t *rx_chan = pre_alloc_group->rx_chans[i];
                rx_chan->base.group = pre_alloc_group;
                rx_chan->base.channel_id = i;
                rx_chan->base.direction = DMA2D_CHANNEL_DIRECTION_RX;
                rx_chan->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            }
            s_platform.groups[group_id] = pre_alloc_group; // register to platform

            // Enable bus clock for the 2D-DMA registers
            PERIPH_RCC_ATOMIC() {
                dma2d_ll_enable_bus_clock(group_id, true);
                dma2d_ll_reset_register(group_id);
            }
            dma2d_hal_init(&pre_alloc_group->hal, group_id); // initialize HAL context
            // Enable 2D-DMA module clock
            dma2d_ll_hw_enable(s_platform.groups[group_id]->hal.dev, true);
        } else {
            ret = ESP_ERR_NO_MEM;
            free(pre_alloc_tx_channels);
            free(pre_alloc_rx_channels);
            free(pre_alloc_group);
        }
    }

    // Tracks the number of consumers of 2D-DMA module (clients of the pool)
    if (s_platform.groups[group_id]) {
        s_platform.group_ref_counts[group_id]++;
    }

    // Allocate interrupts
    // First figure out the interrupt priority
    bool intr_priority_conflict = false;
    if (s_platform.groups[group_id]->intr_priority == -1) {
        s_platform.groups[group_id]->intr_priority = config->intr_priority;
    } else if (config->intr_priority != 0) {
        intr_priority_conflict = (s_platform.groups[group_id]->intr_priority != config->intr_priority);
    }
    ESP_GOTO_ON_FALSE(!intr_priority_conflict, ESP_ERR_INVALID_ARG, wrap_up, TAG, "intr_priority conflict, already is %d but attempt to %" PRIu32, s_platform.groups[group_id]->intr_priority, config->intr_priority);
    uint32_t intr_flags = DMA2D_INTR_ALLOC_FLAGS;
    if (s_platform.groups[group_id]->intr_priority) {
        intr_flags |= (1 << s_platform.groups[group_id]->intr_priority);
    } else {
        intr_flags |= ESP_INTR_FLAG_LOWMED;
    }

    // Allocate TX and RX interrupts
    if (s_platform.groups[group_id]) {
        for (int i = 0; i < SOC_DMA2D_RX_CHANNELS_PER_GROUP; i++) {
            dma2d_rx_channel_t *rx_chan = s_platform.groups[group_id]->rx_chans[i];
            if (rx_chan->base.intr == NULL) {
                ret = esp_intr_alloc_intrstatus(dma2d_periph_signals.groups[group_id].rx_irq_id[i],
                                                intr_flags,
                                                (uint32_t)dma2d_ll_rx_get_interrupt_status_reg(s_platform.groups[group_id]->hal.dev, i),
                                                DMA2D_LL_RX_EVENT_MASK, dma2d_default_isr, &rx_chan->base, &rx_chan->base.intr);
                if (ret != ESP_OK) {
                    ret = ESP_FAIL;
                    ESP_LOGE(TAG, "alloc interrupt failed on rx channel (%d, %d)", group_id, i);
                    goto wrap_up;
                }
            }
        }

        for (int i = 0; i < SOC_DMA2D_TX_CHANNELS_PER_GROUP; i++) {
            dma2d_tx_channel_t *tx_chan = s_platform.groups[group_id]->tx_chans[i];
            if (tx_chan->base.intr == NULL) {
                ret = esp_intr_alloc_intrstatus(dma2d_periph_signals.groups[group_id].tx_irq_id[i],
                                                intr_flags,
                                                (uint32_t)dma2d_ll_tx_get_interrupt_status_reg(s_platform.groups[group_id]->hal.dev, i),
                                                DMA2D_LL_TX_EVENT_MASK, dma2d_default_isr, &tx_chan->base, &tx_chan->base.intr);
                if (ret != ESP_OK) {
                    ret = ESP_FAIL;
                    ESP_LOGE(TAG, "alloc interrupt failed on tx channel (%d, %d)", group_id, i);
                    goto wrap_up;
                }
            }
        }
    }
wrap_up:
    _lock_release(&s_platform.mutex);

    if (ret != ESP_OK && s_platform.groups[group_id]) {
        dma2d_release_pool(s_platform.groups[group_id]);
    }

    *ret_pool = s_platform.groups[group_id];
    return ret;
}

esp_err_t dma2d_release_pool(dma2d_pool_handle_t dma2d_pool)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(dma2d_pool, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    dma2d_group_t *dma2d_group = dma2d_pool;

    bool do_deinitialize = false;
    int group_id = dma2d_group->group_id;

    _lock_acquire(&s_platform.mutex);
    // Remove a client from the 2D-DMA pool
    s_platform.group_ref_counts[group_id]--;
    // If the pool has no client, then release pool resources
    if (s_platform.group_ref_counts[group_id] == 0) {
        assert(s_platform.groups[group_id]);
        do_deinitialize = true;
        // There must be no transaction pending (this should be handled by upper (consumer) driver)
        // Transaction tailq should be empty at this moment
        if (!TAILQ_EMPTY(&dma2d_group->pending_trans_tailq)) {
            ret = ESP_ERR_NOT_ALLOWED;
            ESP_LOGE(TAG, "Still pending transaction in the pool");
            s_platform.group_ref_counts[group_id]++;
            goto err;
        }
        s_platform.groups[group_id] = NULL; // deregister from platform
        // Disable 2D-DMA module clock
        dma2d_ll_hw_enable(dma2d_group->hal.dev, false);
        // Disable the bus clock for the 2D-DMA registers
        PERIPH_RCC_ATOMIC() {
            dma2d_ll_enable_bus_clock(group_id, false);
        }
    }

    if (do_deinitialize) {
        for (int i = 0; i < SOC_DMA2D_RX_CHANNELS_PER_GROUP; i++) {
            if (dma2d_group->rx_chans[i]->base.intr) {
                esp_intr_free(dma2d_group->rx_chans[i]->base.intr);
            }
        }
        for (int i = 0; i < SOC_DMA2D_TX_CHANNELS_PER_GROUP; i++) {
            if (dma2d_group->tx_chans[i]->base.intr) {
                esp_intr_free(dma2d_group->tx_chans[i]->base.intr);
            }
        }
        free(*(dma2d_group->tx_chans));
        free(*(dma2d_group->rx_chans));
        free(dma2d_group);
        s_platform.groups[group_id] = NULL;
    }
err:
    _lock_release(&s_platform.mutex);
    return ret;
}

esp_err_t dma2d_connect(dma2d_channel_handle_t dma2d_chan, const dma2d_trigger_t *trig_periph)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && trig_periph, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    // Find periph_sel_id for the channel
    int peri_sel_id = trig_periph->periph_sel_id;
    uint32_t *periph_m2m_free_id_mask = NULL;
    uint32_t periph_m2m_available_id_mask = 0;
    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        periph_m2m_free_id_mask = &group->tx_periph_m2m_free_id_mask;
        periph_m2m_available_id_mask = DMA2D_LL_TX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK;
    } else {
        periph_m2m_free_id_mask = &group->rx_periph_m2m_free_id_mask;
        periph_m2m_available_id_mask = DMA2D_LL_RX_CHANNEL_PERIPH_M2M_AVAILABLE_ID_MASK;
    }
    portENTER_CRITICAL_SAFE(&group->spinlock);
    if (trig_periph->periph == DMA2D_TRIG_PERIPH_M2M) {
        if (peri_sel_id == -1) {
            // Unspecified periph_sel_id, decide by the driver
            peri_sel_id = __builtin_ctz(*periph_m2m_free_id_mask);
        } else {
            // Check whether specified periph_sel_id is valid
            if (!((1 << peri_sel_id) & *periph_m2m_free_id_mask & periph_m2m_available_id_mask)) {
                peri_sel_id = -1; // Occupied or invalid m2m peri_sel_id
            }
        }
    }
    if (peri_sel_id >= 0) {
        dma2d_chan->status.periph_sel_id = peri_sel_id;
        *periph_m2m_free_id_mask &= ~(1 << peri_sel_id); // acquire m2m periph_sel_id
    }
    portEXIT_CRITICAL_SAFE(&group->spinlock);
    ESP_GOTO_ON_FALSE_ISR(peri_sel_id >= 0, ESP_ERR_INVALID_ARG, err, TAG, "invalid periph_sel_id");

    portENTER_CRITICAL_SAFE(&dma2d_chan->spinlock);
    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_ll_tx_stop(group->hal.dev, channel_id);
        dma2d_hal_tx_reset_channel(&group->hal, channel_id);
        dma2d_ll_tx_connect_to_periph(group->hal.dev, channel_id, trig_periph->periph, peri_sel_id);

        // Configure reorder functionality
        dma2d_ll_tx_enable_reorder(group->hal.dev, channel_id, dma2d_chan->status.reorder_en);
        // Assume dscr_port enable or not can be directly derived from trig_periph
        dma2d_ll_tx_enable_dscr_port(group->hal.dev, channel_id, trig_periph->periph == DMA2D_TRIG_PERIPH_PPA_SR);

        // Reset to certain settings
        dma2d_ll_tx_enable_owner_check(group->hal.dev, channel_id, false);
        dma2d_ll_tx_enable_auto_write_back(group->hal.dev, channel_id, false);
        dma2d_ll_tx_enable_eof_mode(group->hal.dev, channel_id, true);
        dma2d_ll_tx_enable_descriptor_burst(group->hal.dev, channel_id, false);
        dma2d_ll_tx_set_data_burst_length(group->hal.dev, channel_id, DMA2D_DATA_BURST_LENGTH_128);
        dma2d_ll_tx_enable_page_bound_wrap(group->hal.dev, channel_id, true);
        dma2d_ll_tx_set_macro_block_size(group->hal.dev, channel_id, DMA2D_MACRO_BLOCK_SIZE_NONE);
        if ((1 << channel_id) & DMA2D_LL_TX_CHANNEL_SUPPORT_CSC_MASK) {
            dma2d_ll_tx_configure_color_space_conv(group->hal.dev, channel_id, DMA2D_CSC_TX_NONE);
        }

        // Disable and clear all interrupt events
        dma2d_ll_tx_enable_interrupt(group->hal.dev, channel_id, UINT32_MAX, false); // disable all interrupt events
        dma2d_ll_tx_clear_interrupt_status(group->hal.dev, channel_id, UINT32_MAX); // clear all pending events
    } else {
        dma2d_ll_rx_stop(group->hal.dev, channel_id);
        dma2d_hal_rx_reset_channel(&group->hal, channel_id);
        dma2d_ll_rx_connect_to_periph(group->hal.dev, channel_id, trig_periph->periph, peri_sel_id);

        // Configure reorder functionality
        dma2d_ll_rx_enable_reorder(group->hal.dev, channel_id, dma2d_chan->status.reorder_en);
        // Assume dscr_port enable or not can be directly derived from trig_periph
        dma2d_ll_rx_enable_dscr_port(group->hal.dev, channel_id, trig_periph->periph == DMA2D_TRIG_PERIPH_PPA_SR);

        // Reset to certain settings
        dma2d_ll_rx_enable_owner_check(group->hal.dev, channel_id, false);
        dma2d_ll_rx_set_auto_return_owner(group->hal.dev, channel_id, DMA2D_DESCRIPTOR_BUFFER_OWNER_CPU); // After auto write back, the owner field will be cleared
        dma2d_ll_rx_enable_descriptor_burst(group->hal.dev, channel_id, false);
        dma2d_ll_rx_set_data_burst_length(group->hal.dev, channel_id, DMA2D_DATA_BURST_LENGTH_128);
        dma2d_ll_rx_enable_page_bound_wrap(group->hal.dev, channel_id, true);
        dma2d_ll_rx_set_macro_block_size(group->hal.dev, channel_id, DMA2D_MACRO_BLOCK_SIZE_NONE);
        if ((1 << channel_id) & DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK) {
            dma2d_ll_rx_configure_color_space_conv(group->hal.dev, channel_id, DMA2D_CSC_RX_NONE);
        }

        // Disable and clear all interrupt events
        dma2d_ll_rx_enable_interrupt(group->hal.dev, channel_id, UINT32_MAX, false); // disable all interrupt events
        dma2d_ll_rx_clear_interrupt_status(group->hal.dev, channel_id, UINT32_MAX); // clear all pending events
    }
    portEXIT_CRITICAL_SAFE(&dma2d_chan->spinlock);

err:
    return ret;
}

esp_err_t dma2d_register_tx_event_callbacks(dma2d_channel_handle_t dma2d_chan, dma2d_tx_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX && cbs, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan->intr, ESP_ERR_INVALID_STATE, err, TAG, "tx channel intr not allocated");

    dma2d_group_t *group = dma2d_chan->group;
    dma2d_tx_channel_t *tx_chan = __containerof(dma2d_chan, dma2d_tx_channel_t, base);

#if CONFIG_DMA2D_ISR_IRAM_SAFE
    if (cbs->on_desc_done) {
        ESP_GOTO_ON_FALSE_ISR(esp_ptr_in_iram(cbs->on_desc_done),
                              ESP_ERR_INVALID_ARG, err, TAG, "on_desc_done not in IRAM");
    }
    if (user_data) {
        ESP_GOTO_ON_FALSE_ISR(esp_ptr_internal(user_data),
                              ESP_ERR_INVALID_ARG, err, TAG, "user context not in internal RAM");
    }
#endif

    // Enable/Disable 2D-DMA interrupt events for the TX channel
    uint32_t mask = 0;
    portENTER_CRITICAL_SAFE(&tx_chan->base.spinlock);
    if (cbs->on_desc_done) {
        tx_chan->on_desc_done = cbs->on_desc_done;
        mask |= DMA2D_LL_EVENT_TX_DONE;
    }
    tx_chan->user_data = user_data;
    dma2d_ll_tx_enable_interrupt(group->hal.dev, tx_chan->base.channel_id, mask, true);
    portEXIT_CRITICAL_SAFE(&tx_chan->base.spinlock);

err:
    return ret;
}

esp_err_t dma2d_register_rx_event_callbacks(dma2d_channel_handle_t dma2d_chan, dma2d_rx_event_callbacks_t *cbs, void *user_data)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_RX && cbs, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    dma2d_rx_channel_t *rx_chan = __containerof(dma2d_chan, dma2d_rx_channel_t, base);

#if CONFIG_DMA2D_ISR_IRAM_SAFE
    if (cbs->on_recv_eof) {
        ESP_GOTO_ON_FALSE_ISR(esp_ptr_in_iram(cbs->on_recv_eof),
                              ESP_ERR_INVALID_ARG, err, TAG, "on_recv_eof not in IRAM");
    }
    if (cbs->on_desc_done) {
        ESP_GOTO_ON_FALSE_ISR(esp_ptr_in_iram(cbs->on_desc_done),
                              ESP_ERR_INVALID_ARG, err, TAG, "on_desc_done not in IRAM");
    }
    if (user_data) {
        ESP_GOTO_ON_FALSE_ISR(esp_ptr_internal(user_data),
                              ESP_ERR_INVALID_ARG, err, TAG, "user context not in internal RAM");
    }
#endif

    // Enable/Disable 2D-DMA interrupt events for the RX channel
    uint32_t mask = 0;
    portENTER_CRITICAL_SAFE(&rx_chan->base.spinlock);
    if (cbs->on_recv_eof) {
        rx_chan->on_recv_eof = cbs->on_recv_eof;
        mask |= DMA2D_LL_EVENT_RX_SUC_EOF;
    }
    if (cbs->on_desc_done) {
        rx_chan->on_desc_done = cbs->on_desc_done;
        mask |= DMA2D_LL_EVENT_RX_DONE;
    }
    rx_chan->user_data = user_data;
    dma2d_ll_rx_enable_interrupt(group->hal.dev, rx_chan->base.channel_id, mask, true);

    portEXIT_CRITICAL_SAFE(&rx_chan->base.spinlock);

err:
    return ret;
}

esp_err_t dma2d_set_desc_addr(dma2d_channel_handle_t dma2d_chan, intptr_t desc_base_addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && desc_base_addr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    // 2D-DMA descriptor addr needs 8-byte alignment and not in TCM (addr not in TCM is IDF restriction)
    ESP_GOTO_ON_FALSE_ISR((desc_base_addr & 0x7) == 0 && !esp_ptr_in_tcm((void *)desc_base_addr), ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_ll_tx_set_desc_addr(group->hal.dev, channel_id, desc_base_addr);
    } else {
        dma2d_ll_rx_set_desc_addr(group->hal.dev, channel_id, desc_base_addr);
    }

err:
    return ret;
}

esp_err_t dma2d_start(dma2d_channel_handle_t dma2d_chan)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_RX) {
        // dma2d driver relies on going into ISR to free the channels,
        // so even if callbacks are not necessary in some cases, minimum interrupt events should be enabled to trigger ISR
        dma2d_ll_rx_enable_interrupt(group->hal.dev, channel_id, DMA2D_RX_DEFAULT_INTR_FLAG, true);
    }

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        assert(dma2d_ll_tx_is_fsm_idle(group->hal.dev, channel_id));
        dma2d_ll_tx_start(group->hal.dev, channel_id);
    } else {
        assert(dma2d_ll_rx_is_fsm_idle(group->hal.dev, channel_id));
        dma2d_ll_rx_start(group->hal.dev, channel_id);
    }

err:
    return ret;
}

esp_err_t dma2d_stop(dma2d_channel_handle_t dma2d_chan)
{
    ESP_RETURN_ON_FALSE_ISR(dma2d_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_ll_tx_stop(group->hal.dev, channel_id);
    } else {
        dma2d_ll_rx_stop(group->hal.dev, channel_id);
    }

    return ESP_OK;
}

esp_err_t dma2d_append(dma2d_channel_handle_t dma2d_chan)
{
    ESP_RETURN_ON_FALSE_ISR(dma2d_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_ll_tx_restart(group->hal.dev, channel_id);
    } else {
        dma2d_ll_rx_restart(group->hal.dev, channel_id);
    }

    return ESP_OK;
}

esp_err_t dma2d_reset(dma2d_channel_handle_t dma2d_chan)
{
    ESP_RETURN_ON_FALSE_ISR(dma2d_chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    portENTER_CRITICAL_SAFE(&dma2d_chan->spinlock);
    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_hal_tx_reset_channel(&group->hal, channel_id);
    } else {
        dma2d_hal_rx_reset_channel(&group->hal, channel_id);
    }
    portEXIT_CRITICAL_SAFE(&dma2d_chan->spinlock);

    return ESP_OK;
}

esp_err_t dma2d_apply_strategy(dma2d_channel_handle_t dma2d_chan, const dma2d_strategy_config_t *config)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && config, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_ll_tx_enable_owner_check(group->hal.dev, channel_id, config->owner_check);
        dma2d_ll_tx_enable_auto_write_back(group->hal.dev, channel_id, config->auto_update_desc);
        dma2d_ll_tx_enable_eof_mode(group->hal.dev, channel_id, config->eof_till_data_popped);
    } else {
        dma2d_ll_rx_enable_owner_check(group->hal.dev, channel_id, config->owner_check);
        // RX channels do not have control over auto_write_back (always auto_write_back) and eof_mode
    }

err:
    return ret;
}

esp_err_t dma2d_set_transfer_ability(dma2d_channel_handle_t dma2d_chan, const dma2d_transfer_ability_t *ability)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && ability, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE_ISR(ability->data_burst_length < DMA2D_DATA_BURST_LENGTH_INVALID, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE_ISR(ability->mb_size < DMA2D_MACRO_BLOCK_SIZE_INVALID, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        dma2d_ll_tx_enable_descriptor_burst(group->hal.dev, channel_id, ability->desc_burst_en);
        dma2d_ll_tx_set_data_burst_length(group->hal.dev, channel_id, ability->data_burst_length);
        dma2d_ll_tx_set_macro_block_size(group->hal.dev, channel_id, ability->mb_size);
    } else {
        dma2d_ll_rx_enable_descriptor_burst(group->hal.dev, channel_id, ability->desc_burst_en);
        dma2d_ll_rx_set_data_burst_length(group->hal.dev, channel_id, ability->data_burst_length);
        dma2d_ll_rx_set_macro_block_size(group->hal.dev, channel_id, ability->mb_size);
    }

err:
    return ret;
}

esp_err_t dma2d_configure_color_space_conversion(dma2d_channel_handle_t dma2d_chan, const dma2d_csc_config_t *config)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_chan && config, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    dma2d_group_t *group = dma2d_chan->group;
    int channel_id = dma2d_chan->channel_id;

    if (dma2d_chan->direction == DMA2D_CHANNEL_DIRECTION_TX) {
        ESP_GOTO_ON_FALSE_ISR((1 << channel_id) & DMA2D_LL_TX_CHANNEL_SUPPORT_CSC_MASK, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
        ESP_GOTO_ON_FALSE_ISR(config->tx_csc_option < DMA2D_CSC_TX_INVALID, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
        ESP_GOTO_ON_FALSE_ISR(config->post_scramble == 0, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
        ESP_GOTO_ON_FALSE_ISR(config->pre_scramble == DMA2D_SCRAMBLE_ORDER_BYTE2_1_0 || (config->pre_scramble != DMA2D_SCRAMBLE_ORDER_BYTE2_1_0 && config->tx_csc_option != DMA2D_CSC_TX_NONE),
                              ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

        dma2d_ll_tx_configure_color_space_conv(group->hal.dev, channel_id, config->tx_csc_option);
        dma2d_ll_tx_set_csc_pre_scramble(group->hal.dev, channel_id, config->pre_scramble);
    } else {
        ESP_GOTO_ON_FALSE_ISR((1 << channel_id) & DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
        ESP_GOTO_ON_FALSE_ISR(config->rx_csc_option < DMA2D_CSC_RX_INVALID, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
        ESP_GOTO_ON_FALSE_ISR((config->pre_scramble == DMA2D_SCRAMBLE_ORDER_BYTE2_1_0 && config->post_scramble == DMA2D_SCRAMBLE_ORDER_BYTE2_1_0) ||
                              ((config->pre_scramble != DMA2D_SCRAMBLE_ORDER_BYTE2_1_0 || config->post_scramble != DMA2D_SCRAMBLE_ORDER_BYTE2_1_0) && config->rx_csc_option != DMA2D_CSC_RX_NONE),
                              ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

        dma2d_ll_rx_configure_color_space_conv(group->hal.dev, channel_id, config->rx_csc_option);
        dma2d_ll_rx_set_csc_pre_scramble(group->hal.dev, channel_id, config->pre_scramble);
        dma2d_ll_rx_set_csc_post_scramble(group->hal.dev, channel_id, config->post_scramble);
    }

err:
    return ret;
}

esp_err_t dma2d_enqueue(dma2d_pool_handle_t dma2d_pool, const dma2d_trans_config_t *trans_desc, dma2d_trans_t *trans_placeholder)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE_ISR(dma2d_pool && trans_desc && trans_placeholder, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE_ISR(trans_desc->rx_channel_num <= 1, ESP_ERR_INVALID_ARG, err, TAG, "one trans at most has one rx channel");
    uint32_t total_channel_num = trans_desc->tx_channel_num + trans_desc->rx_channel_num;
    ESP_GOTO_ON_FALSE_ISR(total_channel_num <= DMA2D_MAX_CHANNEL_NUM_PER_TRANSACTION, ESP_ERR_INVALID_ARG, err, TAG, "too many channels acquiring for a trans");
    dma2d_group_t *dma2d_group = dma2d_pool;
    if (trans_desc->specified_tx_channel_mask || trans_desc->specified_rx_channel_mask) {
        ESP_GOTO_ON_FALSE_ISR(
            (trans_desc->specified_tx_channel_mask ? (trans_desc->specified_tx_channel_mask & dma2d_group->tx_channel_reserved_mask) : 1 ) &&
            (trans_desc->specified_rx_channel_mask ? (trans_desc->specified_rx_channel_mask & dma2d_group->rx_channel_reserved_mask) : 1 ),
            ESP_ERR_INVALID_ARG, err, TAG, "specified channel(s) not reserved");
        ESP_GOTO_ON_FALSE_ISR(
            (__builtin_popcount(trans_desc->specified_tx_channel_mask) == trans_desc->tx_channel_num) &&
            (__builtin_popcount(trans_desc->specified_rx_channel_mask) == trans_desc->rx_channel_num) &&
            (!trans_desc->tx_channel_num ? 1 : (trans_desc->specified_tx_channel_mask & ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_TX_REORDER) ? DMA2D_LL_TX_CHANNEL_SUPPORT_RO_MASK : UINT32_MAX) & ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_TX_CSC) ? DMA2D_LL_TX_CHANNEL_SUPPORT_CSC_MASK : UINT32_MAX))) &&
            (!trans_desc->rx_channel_num ? 1 : (trans_desc->specified_rx_channel_mask & ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_REORDER) ? DMA2D_LL_RX_CHANNEL_SUPPORT_RO_MASK : UINT32_MAX) & ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_RX_CSC) ? DMA2D_LL_RX_CHANNEL_SUPPORT_CSC_MASK : UINT32_MAX))) &&
            ((trans_desc->channel_flags & DMA2D_CHANNEL_FUNCTION_FLAG_SIBLING) ? (trans_desc->specified_tx_channel_mask == trans_desc->specified_rx_channel_mask) : 1),
            ESP_ERR_INVALID_ARG, err, TAG, "specified channels cannot meet function requirements");
    }

#if CONFIG_DMA2D_ISR_IRAM_SAFE
    ESP_GOTO_ON_FALSE_ISR(trans_desc->on_job_picked && esp_ptr_in_iram(trans_desc->on_job_picked),
                          ESP_ERR_INVALID_ARG, err, TAG, "on_job_picked not in IRAM");
    ESP_GOTO_ON_FALSE_ISR(trans_desc->user_config && esp_ptr_internal(trans_desc->user_config),
                          ESP_ERR_INVALID_ARG, err, TAG, "user context not in internal RAM");
#endif

    trans_placeholder->desc = trans_desc;
    dma2d_trans_channel_info_t channel_handle_array[DMA2D_MAX_CHANNEL_NUM_PER_TRANSACTION];

    portENTER_CRITICAL_SAFE(&dma2d_group->spinlock);
    bool enqueue = !acquire_free_channels_for_trans(dma2d_group, trans_desc, channel_handle_array);
    if (enqueue) {
        if (!trans_desc->specified_tx_channel_mask && !trans_desc->specified_rx_channel_mask) {
            TAILQ_INSERT_TAIL(&dma2d_group->pending_trans_tailq, trans_placeholder, entry);
        } else {
            TAILQ_INSERT_HEAD(&dma2d_group->pending_trans_tailq, trans_placeholder, entry);
        }
    }
    portEXIT_CRITICAL_SAFE(&dma2d_group->spinlock);
    if (!enqueue) {
        // Free channels available, start transaction immediately
        // Store the acquired rx_chan into trans_placeholder (dma2d_trans_t) in case upper driver later need it to call `dma2d_force_end`
        // Upper driver controls the life cycle of trans_placeholder
        for (int i = 0; i < total_channel_num; i++) {
            if (channel_handle_array[i].dir == DMA2D_CHANNEL_DIRECTION_RX) {
                trans_placeholder->rx_chan = channel_handle_array[i].chan;
            }
            // Also save the transaction pointer
            channel_handle_array[i].chan->status.transaction = trans_placeholder;
        }
        trans_desc->on_job_picked(total_channel_num, channel_handle_array, trans_desc->user_config);
    }

err:
    return ret;
}

esp_err_t dma2d_force_end(dma2d_trans_t *trans, bool *need_yield)
{
    ESP_RETURN_ON_FALSE_ISR(trans && trans->rx_chan && need_yield, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    assert(trans->rx_chan->direction == DMA2D_CHANNEL_DIRECTION_RX);

    dma2d_group_t *group = trans->rx_chan->group;

    bool in_flight = false;
    // We judge whether the transaction is in-flight by checking the RX channel it uses is in-use or free
    portENTER_CRITICAL_SAFE(&group->spinlock);
    if (!(group->rx_channel_free_mask & (1 << trans->rx_chan->channel_id))) {
        in_flight = true;
        dma2d_ll_rx_enable_interrupt(group->hal.dev, trans->rx_chan->channel_id, UINT32_MAX, false);
        assert(!dma2d_ll_rx_is_fsm_idle(group->hal.dev, trans->rx_chan->channel_id));
    }
    portEXIT_CRITICAL_SAFE(&group->spinlock);
    ESP_RETURN_ON_FALSE_ISR(in_flight, ESP_ERR_INVALID_STATE, TAG, "transaction not in-flight");

    dma2d_rx_channel_t *rx_chan = group->rx_chans[trans->rx_chan->channel_id];
    // Stop the RX channel and its bundled TX channels first
    dma2d_stop(&rx_chan->base);
    uint32_t tx_chans = rx_chan->bundled_tx_channel_mask;
    for (int i = 0; i < SOC_DMA2D_TX_CHANNELS_PER_GROUP; i++) {
        if (tx_chans & (1 << i)) {
            dma2d_stop(&group->tx_chans[i]->base);
        }
    }
    // Then release channels
    *need_yield = free_up_channels(group, rx_chan);

    return ESP_OK;
}

size_t dma2d_get_trans_elm_size(void)
{
    return sizeof(dma2d_trans_t);
}
