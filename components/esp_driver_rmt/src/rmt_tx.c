/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include "sdkconfig.h"
#if CONFIG_RMT_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_rom_gpio.h"
#include "soc/rmt_periph.h"
#include "soc/rtc.h"
#include "hal/rmt_ll.h"
#include "hal/gpio_hal.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_cache.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "rmt_private.h"
#include "esp_memory_utils.h"

static const char *TAG = "rmt";

struct rmt_sync_manager_t {
    rmt_group_t *group;    // which group the synchro belongs to
    uint32_t channel_mask; // Mask of channels that are managed
    size_t array_size;     // Size of the `tx_channel_array`
    rmt_channel_handle_t tx_channel_array[]; // Array of TX channels that are managed
};

static esp_err_t rmt_del_tx_channel(rmt_channel_handle_t channel);
static esp_err_t rmt_tx_modulate_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config);
static esp_err_t rmt_tx_enable(rmt_channel_handle_t channel);
static esp_err_t rmt_tx_disable(rmt_channel_handle_t channel);
static void rmt_tx_default_isr(void *args);
static void rmt_tx_do_transaction(rmt_tx_channel_t *tx_chan, rmt_tx_trans_desc_t *t);

#if SOC_RMT_SUPPORT_DMA
static bool rmt_dma_tx_eof_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

static esp_err_t rmt_tx_init_dma_link(rmt_tx_channel_t *tx_channel, const rmt_tx_channel_config_t *config)
{
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&dma_chan_config, &tx_channel->base.dma_chan), TAG, "allocate TX DMA channel failed");
    gdma_strategy_config_t gdma_strategy_conf = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    gdma_apply_strategy(tx_channel->base.dma_chan, &gdma_strategy_conf);
    gdma_transfer_config_t transfer_cfg = {
        .access_ext_mem = false, // for performance, we don't use external memory as the DMA buffer
        .max_data_burst_size = 32,
    };
    ESP_RETURN_ON_ERROR(gdma_config_transfer(tx_channel->base.dma_chan, &transfer_cfg), TAG, "config DMA transfer failed");
    gdma_tx_event_callbacks_t cbs = {
        .on_trans_eof = rmt_dma_tx_eof_cb,
    };
    // register the DMA callbacks may fail if the interrupt service can not be installed successfully
    ESP_RETURN_ON_ERROR(gdma_register_tx_event_callbacks(tx_channel->base.dma_chan, &cbs, tx_channel), TAG, "register DMA callbacks failed");

    size_t int_alignment = 0;
    // get the alignment requirement from DMA
    gdma_get_alignment_constraints(tx_channel->base.dma_chan, &int_alignment, NULL);
    // apply RMT hardware alignment requirement
    int_alignment = MAX(int_alignment, sizeof(rmt_symbol_word_t));
    // the memory returned by `heap_caps_aligned_calloc` also meets the cache alignment requirement (both address and size)
    rmt_symbol_word_t *dma_mem_base = heap_caps_aligned_calloc(int_alignment, sizeof(rmt_symbol_word_t), config->mem_block_symbols,
                                                               RMT_MEM_ALLOC_CAPS | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(dma_mem_base, ESP_ERR_NO_MEM, TAG, "no mem for tx DMA buffer");
    tx_channel->dma_mem_base = dma_mem_base;
    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    // do memory sync if the dma buffer is cached
    if (data_cache_line_size) {
        // write back and then invalidate the cache, because later RMT encoder accesses the dma_mem_base by non-cacheable address
        ESP_RETURN_ON_ERROR(esp_cache_msync(dma_mem_base, sizeof(rmt_symbol_word_t) * config->mem_block_symbols,
                                            ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED | ESP_CACHE_MSYNC_FLAG_INVALIDATE),
                            TAG, "cache sync failed");
    }
    // For simplicity, encoder will use the non-cached address to read/write the DMA buffer
    tx_channel->dma_mem_base_nc = (rmt_symbol_word_t *)RMT_GET_NON_CACHE_ADDR(dma_mem_base);
    // the DMA buffer size should be aligned to the DMA requirement
    size_t mount_size_per_node = ALIGN_DOWN(config->mem_block_symbols * sizeof(rmt_symbol_word_t) / RMT_DMA_NODES_PING_PONG, int_alignment);
    // check the upper and lower bound of mount_size_per_node
    ESP_RETURN_ON_FALSE(mount_size_per_node >= sizeof(rmt_symbol_word_t), ESP_ERR_INVALID_ARG,
                        TAG, "mem_block_symbols is too small");
    ESP_RETURN_ON_FALSE(mount_size_per_node <= DMA_DESCRIPTOR_BUFFER_MAX_SIZE, ESP_ERR_INVALID_ARG,
                        TAG, "mem_block_symbols can't exceed %zu", DMA_DESCRIPTOR_BUFFER_MAX_SIZE * RMT_DMA_NODES_PING_PONG / sizeof(rmt_symbol_word_t));

    tx_channel->ping_pong_symbols = mount_size_per_node / sizeof(rmt_symbol_word_t);
    for (int i = 0; i < RMT_DMA_NODES_PING_PONG; i++) {
        // each descriptor shares half of the DMA buffer
        tx_channel->dma_nodes_nc[i].buffer = dma_mem_base + tx_channel->ping_pong_symbols * i;
        tx_channel->dma_nodes_nc[i].dw0.size = tx_channel->ping_pong_symbols * sizeof(rmt_symbol_word_t);
        // the ownership will be switched to DMA in `rmt_tx_do_transaction()`
        tx_channel->dma_nodes_nc[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        // each node can generate the DMA eof interrupt, and the driver will do a ping-pong trick in the eof callback
        tx_channel->dma_nodes_nc[i].dw0.suc_eof = 1;
    }

    return ESP_OK;
}
#endif // SOC_RMT_SUPPORT_DMA

static esp_err_t rmt_tx_register_to_group(rmt_tx_channel_t *tx_channel, const rmt_tx_channel_config_t *config)
{
    size_t mem_block_num = 0;
    // start to search for a free channel
    // a channel can take up its neighbour's memory block, so the neighbour channel won't work, we should skip these "invaded" ones
    int channel_scan_start = RMT_TX_CHANNEL_OFFSET_IN_GROUP;
    int channel_scan_end = RMT_TX_CHANNEL_OFFSET_IN_GROUP + SOC_RMT_TX_CANDIDATES_PER_GROUP;
    if (config->flags.with_dma) {
        // for DMA mode, the memory block number is always 1; for non-DMA mode, memory block number is configured by user
        mem_block_num = 1;
        // Only the last channel has the DMA capability
        channel_scan_start = RMT_TX_CHANNEL_OFFSET_IN_GROUP + SOC_RMT_TX_CANDIDATES_PER_GROUP - 1;
    } else {
        // one channel can occupy multiple memory blocks
        mem_block_num = config->mem_block_symbols / SOC_RMT_MEM_WORDS_PER_CHANNEL;
        if (mem_block_num * SOC_RMT_MEM_WORDS_PER_CHANNEL < config->mem_block_symbols) {
            mem_block_num++;
        }
        tx_channel->ping_pong_symbols = mem_block_num * SOC_RMT_MEM_WORDS_PER_CHANNEL / 2;
    }
    tx_channel->base.mem_block_num = mem_block_num;

    // search free channel and then register to the group
    // memory blocks used by one channel must be continuous
    uint32_t channel_mask = (1 << mem_block_num) - 1;
    rmt_group_t *group = NULL;
    int channel_id = -1;
    for (int i = 0; i < SOC_RMT_GROUPS; i++) {
        group = rmt_acquire_group_handle(i);
        ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "no mem for group (%d)", i);
        portENTER_CRITICAL(&group->spinlock);
        for (int j = channel_scan_start; j < channel_scan_end; j++) {
            if (!(group->occupy_mask & (channel_mask << j))) {
                group->occupy_mask |= (channel_mask << j);
                // the channel ID should index from 0
                channel_id = j - RMT_TX_CHANNEL_OFFSET_IN_GROUP;
                group->tx_channels[channel_id] = tx_channel;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (channel_id < 0) {
            // didn't find a capable channel in the group, don't forget to release the group handle
            rmt_release_group_handle(group);
        } else {
            tx_channel->base.channel_id = channel_id;
            tx_channel->base.channel_mask = channel_mask;
            tx_channel->base.group = group;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(channel_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free tx channels");
    return ESP_OK;
}

static void rmt_tx_unregister_from_group(rmt_channel_t *channel, rmt_group_t *group)
{
    portENTER_CRITICAL(&group->spinlock);
    group->tx_channels[channel->channel_id] = NULL;
    group->occupy_mask &= ~(channel->channel_mask << (channel->channel_id + RMT_TX_CHANNEL_OFFSET_IN_GROUP));
    portEXIT_CRITICAL(&group->spinlock);
    // channel has a reference on group, release it now
    rmt_release_group_handle(group);
}

static esp_err_t rmt_tx_create_trans_queue(rmt_tx_channel_t *tx_channel, const rmt_tx_channel_config_t *config)
{
    esp_err_t ret;

    tx_channel->queue_size = config->trans_queue_depth;
    // Allocate transaction queues. Each queue only holds pointers to the transaction descriptors
    for (int i = 0; i < RMT_TX_QUEUE_MAX; i++) {
        tx_channel->trans_queues[i] = xQueueCreateWithCaps(config->trans_queue_depth, sizeof(rmt_tx_trans_desc_t *), RMT_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(tx_channel->trans_queues[i], ESP_ERR_NO_MEM, exit, TAG, "no mem for queues");
    }

    // Initialize the ready queue
    rmt_tx_trans_desc_t *p_trans_desc = NULL;
    for (int i = 0; i < config->trans_queue_depth; i++) {
        p_trans_desc = &tx_channel->trans_desc_pool[i];
        ESP_GOTO_ON_FALSE(xQueueSend(tx_channel->trans_queues[RMT_TX_QUEUE_READY], &p_trans_desc, 0) == pdTRUE,
                          ESP_ERR_INVALID_STATE, exit, TAG, "ready queue full");
    }

    return ESP_OK;

exit:
    for (int i = 0; i < RMT_TX_QUEUE_MAX; i++) {
        if (tx_channel->trans_queues[i]) {
            vQueueDeleteWithCaps(tx_channel->trans_queues[i]);
            tx_channel->trans_queues[i] = NULL;
        }
    }
    return ret;
}

static esp_err_t rmt_tx_destroy(rmt_tx_channel_t *tx_channel)
{
    if (tx_channel->base.gpio_num >= 0) {
        gpio_output_disable(tx_channel->base.gpio_num);
        esp_gpio_revoke(BIT64(tx_channel->base.gpio_num));
    }
    if (tx_channel->base.intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(tx_channel->base.intr), TAG, "delete interrupt service failed");
    }
    if (tx_channel->base.pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(tx_channel->base.pm_lock), TAG, "delete pm_lock failed");
    }
#if SOC_RMT_SUPPORT_DMA
    if (tx_channel->base.dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_del_channel(tx_channel->base.dma_chan), TAG, "delete dma channel failed");
    }
#endif // SOC_RMT_SUPPORT_DMA
    for (int i = 0; i < RMT_TX_QUEUE_MAX; i++) {
        if (tx_channel->trans_queues[i]) {
            vQueueDeleteWithCaps(tx_channel->trans_queues[i]);
        }
    }
    if (tx_channel->dma_mem_base) {
        free(tx_channel->dma_mem_base);
    }
    if (tx_channel->base.group) {
        // de-register channel from RMT group
        rmt_tx_unregister_from_group(&tx_channel->base, tx_channel->base.group);
    }
    if (tx_channel->dma_nodes) {
        free(tx_channel->dma_nodes);
    }
    free(tx_channel);
    return ESP_OK;
}

esp_err_t rmt_new_tx_channel(const rmt_tx_channel_config_t *config, rmt_channel_handle_t *ret_chan)
{
#if CONFIG_RMT_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    rmt_tx_channel_t *tx_channel = NULL;
    // Check if priority is valid
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE((config->intr_priority) > 0, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", config->intr_priority);
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & RMT_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", config->intr_priority);
    }
    ESP_RETURN_ON_FALSE(config && ret_chan && config->resolution_hz && config->trans_queue_depth, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, TAG, "invalid GPIO number %d", config->gpio_num);
    ESP_RETURN_ON_FALSE((config->mem_block_symbols & 0x01) == 0 && config->mem_block_symbols >= SOC_RMT_MEM_WORDS_PER_CHANNEL,
                        ESP_ERR_INVALID_ARG, TAG, "mem_block_symbols must be even and at least %d", SOC_RMT_MEM_WORDS_PER_CHANNEL);
#if !SOC_RMT_SUPPORT_DMA
    ESP_RETURN_ON_FALSE(config->flags.with_dma == 0, ESP_ERR_NOT_SUPPORTED, TAG, "DMA not supported");
#endif

#if !SOC_RMT_SUPPORT_SLEEP_RETENTION
    ESP_RETURN_ON_FALSE(config->flags.allow_pd == 0, ESP_ERR_NOT_SUPPORTED, TAG, "not able to power down in light sleep");
#endif // SOC_RMT_SUPPORT_SLEEP_RETENTION

    // malloc channel memory
    uint32_t mem_caps = RMT_MEM_ALLOC_CAPS;
    tx_channel = heap_caps_calloc(1, sizeof(rmt_tx_channel_t) + sizeof(rmt_tx_trans_desc_t) * config->trans_queue_depth, mem_caps);
    ESP_GOTO_ON_FALSE(tx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for tx channel");
    // GPIO configuration is not done yet
    tx_channel->base.gpio_num = -1;
    // create DMA descriptors
    if (config->flags.with_dma) {
        // DMA descriptors must be placed in internal SRAM
        mem_caps |= MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
        rmt_dma_descriptor_t *dma_nodes = heap_caps_aligned_calloc(RMT_DMA_DESC_ALIGN, RMT_DMA_NODES_PING_PONG, sizeof(rmt_dma_descriptor_t), mem_caps);
        ESP_GOTO_ON_FALSE(dma_nodes, ESP_ERR_NO_MEM, err, TAG, "no mem for tx DMA nodes");
        tx_channel->dma_nodes = dma_nodes;
        // write back and then invalidate the cached dma_nodes, because later the DMA nodes are accessed by non-cacheable address
        uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
        if (data_cache_line_size) {
            ESP_GOTO_ON_ERROR(esp_cache_msync(dma_nodes, RMT_DMA_NODES_PING_PONG * sizeof(rmt_dma_descriptor_t),
                                              ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE | ESP_CACHE_MSYNC_FLAG_UNALIGNED),
                              err, TAG, "cache sync failed");
        }
        // we will use the non-cached address to manipulate the DMA descriptor, for simplicity
        tx_channel->dma_nodes_nc = (rmt_dma_descriptor_t *)RMT_GET_NON_CACHE_ADDR(dma_nodes);
    }
    // create transaction queues
    ESP_GOTO_ON_ERROR(rmt_tx_create_trans_queue(tx_channel, config), err, TAG, "install trans queues failed");
    // register the channel to group
    ESP_GOTO_ON_ERROR(rmt_tx_register_to_group(tx_channel, config), err, TAG, "register channel failed");
    rmt_group_t *group = tx_channel->base.group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = tx_channel->base.channel_id;
    int group_id = group->group_id;

#if RMT_USE_RETENTION_LINK
    if (config->flags.allow_pd != 0) {
        rmt_create_retention_module(group);
    }
#endif // RMT_USE_RETENTION_LINK

    // reset channel, make sure the TX engine is not working, and events are cleared
    portENTER_CRITICAL(&group->spinlock);
    rmt_hal_tx_channel_reset(&group->hal, channel_id);
    portEXIT_CRITICAL(&group->spinlock);
    // install tx interrupt
    // --- install interrupt service
    // interrupt is mandatory to run basic RMT transactions, so it's not lazy installed in `rmt_tx_register_event_callbacks()`
    // 1-- Set user specified priority to `group->intr_priority`
    bool priority_conflict = rmt_set_intr_priority_to_group(group, config->intr_priority);
    ESP_GOTO_ON_FALSE(!priority_conflict, ESP_ERR_INVALID_ARG, err, TAG, "intr_priority conflict");
    // 2-- Get interrupt allocation flag
    int isr_flags = rmt_get_isr_flags(group);
    // 3-- Allocate interrupt using isr_flag
    ret = esp_intr_alloc_intrstatus(rmt_periph_signals.groups[group_id].irq, isr_flags,
                                    (uint32_t) rmt_ll_get_interrupt_status_reg(hal->regs),
                                    RMT_LL_EVENT_TX_MASK(channel_id), rmt_tx_default_isr, tx_channel,
                                    &tx_channel->base.intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install tx interrupt failed");
    // install DMA service
#if SOC_RMT_SUPPORT_DMA
    if (config->flags.with_dma) {
        ESP_GOTO_ON_ERROR(rmt_tx_init_dma_link(tx_channel, config), err, TAG, "install tx DMA failed");
    }
#endif
    // select the clock source
    ESP_GOTO_ON_ERROR(rmt_select_periph_clock(&tx_channel->base, config->clk_src), err, TAG, "set group clock failed");
    // set channel clock resolution
    uint32_t real_div = group->resolution_hz / config->resolution_hz;
    rmt_ll_tx_set_channel_clock_div(hal->regs, channel_id, real_div);
    // resolution lost due to division, calculate the real resolution
    tx_channel->base.resolution_hz = group->resolution_hz / real_div;
    if (tx_channel->base.resolution_hz != config->resolution_hz) {
        ESP_LOGW(TAG, "channel resolution loss, real=%"PRIu32, tx_channel->base.resolution_hz);
    }

    rmt_ll_tx_set_mem_blocks(hal->regs, channel_id, tx_channel->base.mem_block_num);
    // set limit threshold, after transmit ping_pong_symbols size, an interrupt event would be generated
    rmt_ll_tx_set_limit(hal->regs, channel_id, tx_channel->ping_pong_symbols);
    // disable carrier modulation by default, can re-enable by `rmt_apply_carrier()`
    rmt_ll_tx_enable_carrier_modulation(hal->regs, channel_id, false);
    // idle level is determined by register value
    rmt_ll_tx_fix_idle_level(hal->regs, channel_id, 0, true);
    // always enable tx wrap, both DMA mode and ping-pong mode rely this feature
    rmt_ll_tx_enable_wrap(hal->regs, channel_id, true);

    // reserve the GPIO output path, because we don't expect another peripheral to signal to the same GPIO
    uint64_t old_gpio_rsv_mask = esp_gpio_reserve(BIT64(config->gpio_num));
    // check if the GPIO is already used by others, RMT TX channel only uses the output path of the GPIO
    if (old_gpio_rsv_mask & BIT64(config->gpio_num)) {
        ESP_LOGW(TAG, "GPIO %d is not usable, maybe conflict with others", config->gpio_num);
    }
    // GPIO Matrix/MUX configuration
    gpio_func_sel(config->gpio_num, PIN_FUNC_GPIO);
    // connect the signal to the GPIO by matrix, it will also enable the output path properly
    esp_rom_gpio_connect_out_signal(config->gpio_num,
                                    rmt_periph_signals.groups[group_id].channels[channel_id + RMT_TX_CHANNEL_OFFSET_IN_GROUP].tx_sig,
                                    config->flags.invert_out, false);
    tx_channel->base.gpio_num = config->gpio_num;

    // deprecated, to be removed in in esp-idf v6.0
    if (config->flags.io_loop_back) {
        gpio_ll_input_enable(&GPIO, config->gpio_num);
    }
    if (config->flags.io_od_mode) {
        gpio_ll_od_enable(&GPIO, config->gpio_num);
    }

    portMUX_INITIALIZE(&tx_channel->base.spinlock);
    atomic_init(&tx_channel->base.fsm, RMT_FSM_INIT);
    tx_channel->base.direction = RMT_CHANNEL_DIRECTION_TX;
    tx_channel->base.hw_mem_base = &RMTMEM.channels[channel_id + RMT_TX_CHANNEL_OFFSET_IN_GROUP].symbols[0];
    // polymorphic methods
    tx_channel->base.del = rmt_del_tx_channel;
    tx_channel->base.set_carrier_action = rmt_tx_modulate_carrier;
    tx_channel->base.enable = rmt_tx_enable;
    tx_channel->base.disable = rmt_tx_disable;
    // return general channel handle
    *ret_chan = &tx_channel->base;
    ESP_LOGD(TAG, "new tx channel(%d,%d) at %p, gpio=%d, res=%"PRIu32"Hz, hw_mem_base=%p, dma_mem_base=%p, dma_nodes=%p, ping_pong_size=%zu, queue_depth=%zu",
             group_id, channel_id, tx_channel, config->gpio_num, tx_channel->base.resolution_hz,
             tx_channel->base.hw_mem_base, tx_channel->dma_mem_base, tx_channel->dma_nodes, tx_channel->ping_pong_symbols, tx_channel->queue_size);
    return ESP_OK;

err:
    if (tx_channel) {
        rmt_tx_destroy(tx_channel);
    }
    return ret;
}

static esp_err_t rmt_del_tx_channel(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(atomic_load(&channel->fsm) == RMT_FSM_INIT,
                        ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_group_t *group = channel->group;
    int group_id = group->group_id;
    int channel_id = channel->channel_id;
    ESP_LOGD(TAG, "del tx channel(%d,%d)", group_id, channel_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(rmt_tx_destroy(tx_chan), TAG, "destroy tx channel failed");
    return ESP_OK;
}

esp_err_t rmt_new_sync_manager(const rmt_sync_manager_config_t *config, rmt_sync_manager_handle_t *ret_synchro)
{
#if !SOC_RMT_SUPPORT_TX_SYNCHRO
    ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "sync manager not supported");
#else
    esp_err_t ret = ESP_OK;
    rmt_sync_manager_t *synchro = NULL;
    ESP_GOTO_ON_FALSE(config && ret_synchro && config->tx_channel_array && config->array_size, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    synchro = heap_caps_calloc(1, sizeof(rmt_sync_manager_t) + sizeof(rmt_channel_handle_t) * config->array_size, RMT_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(synchro, ESP_ERR_NO_MEM, err, TAG, "no mem for sync manager");
    for (size_t i = 0; i < config->array_size; i++) {
        synchro->tx_channel_array[i] = config->tx_channel_array[i];
    }
    synchro->array_size = config->array_size;

    int group_id = config->tx_channel_array[0]->group->group_id;
    // acquire group handle, increase reference count
    rmt_group_t *group = rmt_acquire_group_handle(group_id);
    // sanity check
    assert(group);
    synchro->group = group;
    // calculate the mask of the channels to be managed
    uint32_t channel_mask = 0;
    rmt_channel_handle_t channel = NULL;
    for (size_t i = 0; i < config->array_size; i++) {
        channel = config->tx_channel_array[i];
        ESP_GOTO_ON_FALSE(channel->direction == RMT_CHANNEL_DIRECTION_TX, ESP_ERR_INVALID_ARG, err, TAG, "sync manager supports TX channel only");
        ESP_GOTO_ON_FALSE(channel->group == group, ESP_ERR_INVALID_ARG, err, TAG, "channels to be managed should locate in the same group");
        ESP_GOTO_ON_FALSE(atomic_load(&channel->fsm) == RMT_FSM_ENABLE, ESP_ERR_INVALID_STATE, err, TAG, "channel not in enable state");
        channel_mask |= 1 << channel->channel_id;
    }
    synchro->channel_mask = channel_mask;

    // search and register sync manager to group
    bool new_synchro = false;
    portENTER_CRITICAL(&group->spinlock);
    if (group->sync_manager == NULL) {
        group->sync_manager = synchro;
        new_synchro = true;
    }
    portEXIT_CRITICAL(&group->spinlock);
    ESP_GOTO_ON_FALSE(new_synchro, ESP_ERR_NOT_FOUND, err, TAG, "no free sync manager in the group");

    // enable sync manager
    portENTER_CRITICAL(&group->spinlock);
    rmt_ll_tx_enable_sync(group->hal.regs, true);
    rmt_ll_tx_sync_group_add_channels(group->hal.regs, channel_mask);
    rmt_ll_tx_reset_channels_clock_div(group->hal.regs, channel_mask);
    // ensure the reading cursor of each channel is pulled back to the starting line
    for (size_t i = 0; i < config->array_size; i++) {
        rmt_ll_tx_reset_pointer(group->hal.regs, config->tx_channel_array[i]->channel_id);
    }
    portEXIT_CRITICAL(&group->spinlock);

    *ret_synchro = synchro;
    ESP_LOGD(TAG, "new sync manager at %p, with channel mask:%02"PRIx32, synchro, synchro->channel_mask);
    return ESP_OK;

err:
    if (synchro) {
        if (synchro->group) {
            rmt_release_group_handle(synchro->group);
        }
        free(synchro);
    }
    return ret;
#endif // !SOC_RMT_SUPPORT_TX_SYNCHRO
}

esp_err_t rmt_sync_reset(rmt_sync_manager_handle_t synchro)
{
#if !SOC_RMT_SUPPORT_TX_SYNCHRO
    ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "sync manager not supported");
#else
    ESP_RETURN_ON_FALSE(synchro, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    rmt_group_t *group = synchro->group;

    portENTER_CRITICAL(&group->spinlock);
    rmt_ll_tx_reset_channels_clock_div(group->hal.regs, synchro->channel_mask);
    for (size_t i = 0; i < synchro->array_size; i++) {
        rmt_ll_tx_reset_pointer(group->hal.regs, synchro->tx_channel_array[i]->channel_id);
    }
    portEXIT_CRITICAL(&group->spinlock);

    return ESP_OK;
#endif // !SOC_RMT_SUPPORT_TX_SYNCHRO
}

esp_err_t rmt_del_sync_manager(rmt_sync_manager_handle_t synchro)
{
#if !SOC_RMT_SUPPORT_TX_SYNCHRO
    ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "sync manager not supported");
#else
    ESP_RETURN_ON_FALSE(synchro, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    rmt_group_t *group = synchro->group;
    int group_id = group->group_id;

    portENTER_CRITICAL(&group->spinlock);
    group->sync_manager = NULL;
    // disable sync manager
    rmt_ll_tx_enable_sync(group->hal.regs, false);
    rmt_ll_tx_sync_group_remove_channels(group->hal.regs, synchro->channel_mask);
    portEXIT_CRITICAL(&group->spinlock);

    free(synchro);
    ESP_LOGD(TAG, "del sync manager in group(%d)", group_id);
    rmt_release_group_handle(group);
    return ESP_OK;
#endif // !SOC_RMT_SUPPORT_TX_SYNCHRO
}

esp_err_t rmt_tx_register_event_callbacks(rmt_channel_handle_t channel, const rmt_tx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(channel && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->direction == RMT_CHANNEL_DIRECTION_TX, ESP_ERR_INVALID_ARG, TAG, "invalid channel direction");
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);

#if CONFIG_RMT_ISR_IRAM_SAFE
    if (cbs->on_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_trans_done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    tx_chan->on_trans_done = cbs->on_trans_done;
    tx_chan->user_data = user_data;
    return ESP_OK;
}

esp_err_t rmt_transmit(rmt_channel_handle_t channel, rmt_encoder_t *encoder, const void *payload, size_t payload_bytes, const rmt_transmit_config_t *config)
{
    ESP_RETURN_ON_FALSE(channel && encoder && payload && payload_bytes && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->direction == RMT_CHANNEL_DIRECTION_TX, ESP_ERR_INVALID_ARG, TAG, "invalid channel direction");
#if !SOC_RMT_SUPPORT_TX_LOOP_COUNT
    ESP_RETURN_ON_FALSE(config->loop_count <= 0, ESP_ERR_NOT_SUPPORTED, TAG, "loop count is not supported");
#endif // !SOC_RMT_SUPPORT_TX_LOOP_COUNT
#if CONFIG_RMT_ISR_IRAM_SAFE
    // payload is retrieved by the encoder, we should make sure it's still accessible even when the cache is disabled
    ESP_RETURN_ON_FALSE(esp_ptr_internal(payload), ESP_ERR_INVALID_ARG, TAG, "payload not in internal RAM");
#endif
    TickType_t queue_wait_ticks = portMAX_DELAY;
    if (config->flags.queue_nonblocking) {
        queue_wait_ticks = 0;
    }
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_tx_trans_desc_t *t = NULL;
    // acquire one transaction description from ready queue or complete queue
    if (xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_READY], &t, 0) != pdTRUE) {
        if (xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &t, queue_wait_ticks) == pdTRUE) {
            tx_chan->num_trans_inflight--;
        }
    }
    ESP_RETURN_ON_FALSE(t, ESP_ERR_INVALID_STATE, TAG, "no free transaction descriptor, please consider increasing trans_queue_depth");

    // fill in the transaction descriptor
    memset(t, 0, sizeof(rmt_tx_trans_desc_t));
    t->encoder = encoder;
    t->payload = payload;
    t->payload_bytes = payload_bytes;
    t->loop_count = config->loop_count;
    t->remain_loop_count = t->loop_count;
    t->flags.eot_level = config->flags.eot_level;

    // send the transaction descriptor to queue
    if (xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &t, 0) == pdTRUE) {
        tx_chan->num_trans_inflight++;
    } else {
        // put the trans descriptor back to ready_queue
        ESP_RETURN_ON_FALSE(xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
    }

    // check if we need to start one pending transaction
    rmt_fsm_t expected_fsm = RMT_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_RUN_WAIT)) {
        // check if we need to start one transaction
        if (xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &t, 0) == pdTRUE) {
            atomic_store(&channel->fsm, RMT_FSM_RUN);
            rmt_tx_do_transaction(tx_chan, t);
        } else {
            atomic_store(&channel->fsm, RMT_FSM_ENABLE);
        }
    }

    return ESP_OK;
}

esp_err_t rmt_tx_wait_all_done(rmt_channel_handle_t channel, int timeout_ms)
{
    ESP_RETURN_ON_FALSE(channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    TickType_t wait_ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    // recycle all transaction that are on the fly
    rmt_tx_trans_desc_t *t = NULL;
    size_t num_trans_inflight = tx_chan->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &t, wait_ticks) == pdTRUE,
                            ESP_ERR_TIMEOUT, TAG, "flush timeout");
        ESP_RETURN_ON_FALSE(xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        tx_chan->num_trans_inflight--;
    }
    return ESP_OK;
}

static void IRAM_ATTR rmt_tx_mark_eof(rmt_tx_channel_t *tx_chan)
{
    rmt_channel_t *channel = &tx_chan->base;
    rmt_group_t *group = channel->group;
    int channel_id = channel->channel_id;
    rmt_symbol_word_t *mem_to_nc = NULL;
    rmt_tx_trans_desc_t *cur_trans = tx_chan->cur_trans;
    rmt_dma_descriptor_t *desc_nc = NULL;
    if (channel->dma_chan) {
        mem_to_nc = tx_chan->dma_mem_base_nc;
    } else {
        mem_to_nc = channel->hw_mem_base;
    }

    // a RMT word whose duration is zero means a "stop" pattern
    mem_to_nc[tx_chan->mem_off++] = (rmt_symbol_word_t) {
        .duration0 = 0,
        .level0 = cur_trans->flags.eot_level,
        .duration1 = 0,
        .level1 = cur_trans->flags.eot_level,
    };

    size_t off = 0;
    if (channel->dma_chan) {
        if (tx_chan->mem_off <= tx_chan->ping_pong_symbols) {
            desc_nc = &tx_chan->dma_nodes_nc[0];
            off = tx_chan->mem_off;
        } else {
            desc_nc = &tx_chan->dma_nodes_nc[1];
            off = tx_chan->mem_off - tx_chan->ping_pong_symbols;
        }
        desc_nc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc_nc->dw0.length = off * sizeof(rmt_symbol_word_t);
        // break down the DMA descriptor link
        desc_nc->next = NULL;
    } else {
        portENTER_CRITICAL_ISR(&group->spinlock);
        // This is the end of a sequence of encoding sessions, disable the threshold interrupt as no more data will be put into RMT memory block
        rmt_ll_enable_interrupt(group->hal.regs, RMT_LL_EVENT_TX_THRES(channel_id), false);
        portEXIT_CRITICAL_ISR(&group->spinlock);
    }
}

static size_t IRAM_ATTR rmt_encode_check_result(rmt_tx_channel_t *tx_chan, rmt_tx_trans_desc_t *t)
{
    rmt_encode_state_t encode_state = RMT_ENCODING_RESET;
    rmt_encoder_handle_t encoder = t->encoder;
    size_t encoded_symbols = encoder->encode(encoder, &tx_chan->base, t->payload, t->payload_bytes, &encode_state);

    if (encode_state & RMT_ENCODING_COMPLETE) {
        t->flags.encoding_done = true;
        // inserting EOF symbol if there's extra space
        if (!(encode_state & RMT_ENCODING_MEM_FULL)) {
            rmt_tx_mark_eof(tx_chan);
            encoded_symbols += 1;
        }
    }

    // for loop transaction, the memory block should accommodate all encoded RMT symbols
    if (t->loop_count != 0) {
        if (unlikely(encoded_symbols > tx_chan->base.mem_block_num * SOC_RMT_MEM_WORDS_PER_CHANNEL)) {
            ESP_DRAM_LOGE(TAG, "encoding artifacts can't exceed hw memory block for loop transmission");
        }
    }

    return encoded_symbols;
}

static void IRAM_ATTR rmt_tx_do_transaction(rmt_tx_channel_t *tx_chan, rmt_tx_trans_desc_t *t)
{
    rmt_channel_t *channel = &tx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

    // update current transaction
    tx_chan->cur_trans = t;

#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        gdma_reset(channel->dma_chan);
        // chain the descriptors into a ring, and will break it in `rmt_encode_eof()`
        for (int i = 0; i < RMT_DMA_NODES_PING_PONG; i++) {
            tx_chan->dma_nodes_nc[i].next = &tx_chan->dma_nodes[i + 1]; // note, we must use the cache address for the next pointer
            tx_chan->dma_nodes_nc[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        }
        tx_chan->dma_nodes_nc[RMT_DMA_NODES_PING_PONG - 1].next = &tx_chan->dma_nodes[0];
    }
#endif // SOC_RMT_SUPPORT_DMA

    // set transaction specific parameters
    portENTER_CRITICAL_SAFE(&channel->spinlock);
    rmt_ll_tx_reset_pointer(hal->regs, channel_id); // reset pointer for new transaction
    rmt_ll_tx_enable_loop(hal->regs, channel_id, t->loop_count != 0);
#if SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP
    rmt_ll_tx_enable_loop_autostop(hal->regs, channel_id, true);
#endif // SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_tx_reset_loop_count(hal->regs, channel_id);
    rmt_ll_tx_enable_loop_count(hal->regs, channel_id, t->loop_count > 0);
    // transfer loops in batches
    if (t->remain_loop_count > 0) {
        uint32_t this_loop_count = MIN(t->remain_loop_count, RMT_LL_MAX_LOOP_COUNT_PER_BATCH);
        rmt_ll_tx_set_loop_count(hal->regs, channel_id, this_loop_count);
        t->remain_loop_count -= this_loop_count;
    }
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT
    portEXIT_CRITICAL_SAFE(&channel->spinlock);

    // enable/disable specific interrupts
    portENTER_CRITICAL_SAFE(&group->spinlock);
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel_id), t->loop_count > 0);
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT
    // in DMA mode, DMA eof event plays the similar functionality to this threshold interrupt, so only enable it for non-DMA mode
    if (!channel->dma_chan) {
        // don't enable threshold interrupt with loop mode on
        // threshold interrupt will be disabled in `rmt_encode_eof()`
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_THRES(channel_id), t->loop_count == 0);
        // Threshold interrupt will be generated by accident, clear it before starting new transmission
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_THRES(channel_id));
    }
    // don't generate trans done event for loop transmission
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id), t->loop_count == 0);
    portEXIT_CRITICAL_SAFE(&group->spinlock);

    // at the beginning of a new transaction, encoding memory offset should start from zero.
    // It will increase in the encode function e.g. `rmt_encode_copy()`
    tx_chan->mem_off = 0;
    // use the full memory block for the beginning encoding session
    tx_chan->mem_end = tx_chan->ping_pong_symbols * 2;
    // perform the encoding session, return the number of encoded symbols
    t->transmitted_symbol_num = rmt_encode_check_result(tx_chan, t);
    // we're going to perform ping-pong operation, so the next encoding end position is the middle
    tx_chan->mem_end = tx_chan->ping_pong_symbols;

#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        gdma_start(channel->dma_chan, (intptr_t)tx_chan->dma_nodes); // note, we must use the cached descriptor address to start the DMA
        // delay a while, wait for DMA data going to RMT memory block
        esp_rom_delay_us(1);
    }
#endif
    // turn on the TX machine
    portENTER_CRITICAL_SAFE(&channel->spinlock);
    rmt_ll_tx_fix_idle_level(hal->regs, channel_id, t->flags.eot_level, true);
    rmt_ll_tx_start(hal->regs, channel_id);
    portEXIT_CRITICAL_SAFE(&channel->spinlock);
}

static esp_err_t rmt_tx_enable(rmt_channel_handle_t channel)
{
    // can enable the channel when it's in "init" state
    rmt_fsm_t expected_fsm = RMT_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_ENABLE_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);

    // acquire power manager lock
    if (channel->pm_lock) {
        esp_pm_lock_acquire(channel->pm_lock);
    }

#if SOC_RMT_SUPPORT_DMA
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;
    if (channel->dma_chan) {
        // enable the DMA access mode
        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_tx_enable_dma(hal->regs, channel_id, true);
        portEXIT_CRITICAL(&channel->spinlock);

        gdma_connect(channel->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_RMT, 0));
    }
#endif // SOC_RMT_SUPPORT_DMA

    atomic_store(&channel->fsm, RMT_FSM_ENABLE);

    // check if we need to start one pending transaction
    rmt_tx_trans_desc_t *t = NULL;
    expected_fsm = RMT_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_RUN_WAIT)) {
        if (xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &t, 0) == pdTRUE) {
            // sanity check
            assert(t);
            atomic_store(&channel->fsm, RMT_FSM_RUN);
            rmt_tx_do_transaction(tx_chan, t);
        } else {
            atomic_store(&channel->fsm, RMT_FSM_ENABLE);
        }
    }

    return ESP_OK;
}

static esp_err_t rmt_tx_disable(rmt_channel_handle_t channel)
{
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

    // can disable the channel when it's in `enable` or `run` state
    bool valid_state = false;
    rmt_fsm_t expected_fsm = RMT_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_INIT_WAIT)) {
        valid_state = true;
    }
    expected_fsm = RMT_FSM_RUN;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_INIT_WAIT)) {
        valid_state = true;
        // disable the hardware
        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_tx_enable_loop(hal->regs, channel->channel_id, false);
#if SOC_RMT_SUPPORT_TX_ASYNC_STOP
        rmt_ll_tx_stop(hal->regs, channel->channel_id);
#endif
        portEXIT_CRITICAL(&channel->spinlock);

        portENTER_CRITICAL(&group->spinlock);
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_MASK(channel_id), false);
#if !SOC_RMT_SUPPORT_TX_ASYNC_STOP
        // we do a trick to stop the undergoing transmission
        // stop interrupt, insert EOF marker to the RMT memory, polling the trans_done event
        channel->hw_mem_base[0].val = 0;
        while (!(rmt_ll_tx_get_interrupt_status_raw(hal->regs, channel_id) & RMT_LL_EVENT_TX_DONE(channel_id))) {}
#endif
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_MASK(channel_id));
        portEXIT_CRITICAL(&group->spinlock);
    }
    ESP_RETURN_ON_FALSE(valid_state, ESP_ERR_INVALID_STATE, TAG, "channel can't be disabled in state %d", expected_fsm);

    // disable the DMA
#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        gdma_stop(channel->dma_chan);
        gdma_disconnect(channel->dma_chan);

        // disable DMA access mode
        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_tx_enable_dma(hal->regs, channel_id, false);
        portEXIT_CRITICAL(&channel->spinlock);
    }
#endif

    // recycle the interrupted transaction
    if (tx_chan->cur_trans) {
        xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &tx_chan->cur_trans, 0);
        // reset corresponding encoder
        rmt_encoder_reset(tx_chan->cur_trans->encoder);
    }
    tx_chan->cur_trans = NULL;

    // release power manager lock
    if (channel->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(channel->pm_lock), TAG, "release pm_lock failed");
    }

    // finally we switch to the INIT state
    atomic_store(&channel->fsm, RMT_FSM_INIT);

    return ESP_OK;
}

static esp_err_t rmt_tx_modulate_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config)
{
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int channel_id = channel->channel_id;
    uint32_t real_frequency = 0;

    if (config && config->frequency_hz) {
        // carrier module works base on group clock
        uint32_t total_ticks = group->resolution_hz / config->frequency_hz; // Note this division operation will lose precision
        uint32_t high_ticks = total_ticks * config->duty_cycle;
        uint32_t low_ticks = total_ticks - high_ticks;

        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_tx_set_carrier_level(hal->regs, channel_id, !config->flags.polarity_active_low);
        rmt_ll_tx_set_carrier_high_low_ticks(hal->regs, channel_id, high_ticks, low_ticks);
#if SOC_RMT_SUPPORT_TX_CARRIER_DATA_ONLY
        rmt_ll_tx_enable_carrier_always_on(hal->regs, channel_id, config->flags.always_on);
#endif
        portEXIT_CRITICAL(&channel->spinlock);
        // save real carrier frequency
        real_frequency = group->resolution_hz / total_ticks;
    }

    // enable/disable carrier modulation
    portENTER_CRITICAL(&channel->spinlock);
    rmt_ll_tx_enable_carrier_modulation(hal->regs, channel_id, real_frequency > 0);
    portEXIT_CRITICAL(&channel->spinlock);

    if (real_frequency > 0) {
        ESP_LOGD(TAG, "enable carrier modulation for channel(%d,%d), freq=%"PRIu32"Hz", group_id, channel_id, real_frequency);
    } else {
        ESP_LOGD(TAG, "disable carrier modulation for channel(%d,%d)", group_id, channel_id);
    }
    return ESP_OK;
}

static bool IRAM_ATTR rmt_isr_handle_tx_threshold(rmt_tx_channel_t *tx_chan)
{
    // continue ping-pong transmission
    rmt_tx_trans_desc_t *t = tx_chan->cur_trans;
    size_t encoded_symbols = t->transmitted_symbol_num;
    // encoding finished, only need to send the EOF symbol
    if (t->flags.encoding_done) {
        rmt_tx_mark_eof(tx_chan);
        encoded_symbols += 1;
    } else {
        encoded_symbols += rmt_encode_check_result(tx_chan, t);
    }
    t->transmitted_symbol_num = encoded_symbols;
    tx_chan->mem_end = tx_chan->ping_pong_symbols * 3 - tx_chan->mem_end; // mem_end equals to either ping_pong_symbols or ping_pong_symbols*2

    return false;
}

static bool IRAM_ATTR rmt_isr_handle_tx_done(rmt_tx_channel_t *tx_chan)
{
    rmt_channel_t *channel = &tx_chan->base;
    BaseType_t awoken = pdFALSE;
    rmt_tx_trans_desc_t *trans_desc = NULL;
    bool need_yield = false;

    rmt_fsm_t expected_fsm = RMT_FSM_RUN;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_ENABLE_WAIT)) {
        trans_desc = tx_chan->cur_trans;
        // move current finished transaction to the complete queue
        xQueueSendFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &trans_desc, &awoken);
        if (awoken == pdTRUE) {
            need_yield = true;
        }
        tx_chan->cur_trans = NULL;
        atomic_store(&channel->fsm, RMT_FSM_ENABLE);

        // invoke callback
        rmt_tx_done_callback_t done_cb = tx_chan->on_trans_done;
        if (done_cb) {
            rmt_tx_done_event_data_t edata = {
                .num_symbols = trans_desc->transmitted_symbol_num,
            };
            if (done_cb(channel, &edata, tx_chan->user_data)) {
                need_yield = true;
            }
        }
    }

    // let's try start the next pending transaction
    expected_fsm = RMT_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_RUN_WAIT)) {
        if (xQueueReceiveFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &trans_desc, &awoken) == pdTRUE) {
            // sanity check
            assert(trans_desc);
            atomic_store(&channel->fsm, RMT_FSM_RUN);
            // begin a new transaction
            rmt_tx_do_transaction(tx_chan, trans_desc);
            if (awoken == pdTRUE) {
                need_yield = true;
            }
        } else {
            atomic_store(&channel->fsm, RMT_FSM_ENABLE);
        }
    }

    return need_yield;
}

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
static bool IRAM_ATTR rmt_isr_handle_tx_loop_end(rmt_tx_channel_t *tx_chan)
{
    rmt_channel_t *channel = &tx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    BaseType_t awoken = pdFALSE;
    rmt_tx_trans_desc_t *trans_desc = NULL;
    bool need_yield = false;

    trans_desc = tx_chan->cur_trans;
    if (trans_desc) {
#if !SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP
        portENTER_CRITICAL_ISR(&channel->spinlock);
        // This is a workaround for chips that don't support loop auto stop
        // Although we stop the transaction immediately in ISR handler, it's still possible that some rmt symbols have sneaked out
        rmt_ll_tx_stop(hal->regs, channel_id);
        portEXIT_CRITICAL_ISR(&channel->spinlock);
#endif // SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP

        // continue unfinished loop transaction
        if (trans_desc->remain_loop_count) {
            uint32_t this_loop_count = MIN(trans_desc->remain_loop_count, RMT_LL_MAX_LOOP_COUNT_PER_BATCH);
            trans_desc->remain_loop_count -= this_loop_count;
            portENTER_CRITICAL_ISR(&channel->spinlock);
            rmt_ll_tx_set_loop_count(hal->regs, channel_id, this_loop_count);
            rmt_ll_tx_reset_pointer(hal->regs, channel_id);
            // continue the loop transmission, don't need to fill the RMT symbols again, just restart the engine
            rmt_ll_tx_start(hal->regs, channel_id);
            portEXIT_CRITICAL_ISR(&channel->spinlock);
            return need_yield;
        }

        // loop transaction finished
        rmt_fsm_t expected_fsm = RMT_FSM_RUN;
        if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_ENABLE_WAIT)) {
            // move current finished transaction to the complete queue
            xQueueSendFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &trans_desc, &awoken);
            if (awoken == pdTRUE) {
                need_yield = true;
            }
            tx_chan->cur_trans = NULL;
            atomic_store(&channel->fsm, RMT_FSM_ENABLE);

            // invoke callback
            rmt_tx_done_callback_t done_cb = tx_chan->on_trans_done;
            if (done_cb) {
                rmt_tx_done_event_data_t edata = {
                    .num_symbols = trans_desc->transmitted_symbol_num,
                };
                if (done_cb(channel, &edata, tx_chan->user_data)) {
                    need_yield = true;
                }
            }
        }

        // let's try start the next pending transaction
        expected_fsm = RMT_FSM_ENABLE;
        if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_RUN_WAIT)) {
            if (xQueueReceiveFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &trans_desc, &awoken) == pdTRUE) {
                // sanity check
                assert(trans_desc);
                atomic_store(&channel->fsm, RMT_FSM_RUN);
                // begin a new transaction
                rmt_tx_do_transaction(tx_chan, trans_desc);
                if (awoken == pdTRUE) {
                    need_yield = true;
                }
            } else {
                atomic_store(&channel->fsm, RMT_FSM_ENABLE);
            }
        }
    }

    if (awoken == pdTRUE) {
        need_yield = true;
    }
    return need_yield;
}
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT

static void IRAM_ATTR rmt_tx_default_isr(void *args)
{
    rmt_tx_channel_t *tx_chan = (rmt_tx_channel_t *)args;
    rmt_channel_t *channel = &tx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    bool need_yield = false;

    uint32_t status = rmt_ll_tx_get_interrupt_status(hal->regs, channel_id);
    rmt_ll_clear_interrupt_status(hal->regs, status);

    // Tx threshold interrupt
    if (status & RMT_LL_EVENT_TX_THRES(channel_id)) {
        if (rmt_isr_handle_tx_threshold(tx_chan)) {
            need_yield = true;
        }
    }

    // Tx end interrupt
    if (status & RMT_LL_EVENT_TX_DONE(channel_id)) {
        if (rmt_isr_handle_tx_done(tx_chan)) {
            need_yield = true;
        }
    }

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    // Tx loop end interrupt
    if (status & RMT_LL_EVENT_TX_LOOP_END(channel_id)) {
        if (rmt_isr_handle_tx_loop_end(tx_chan)) {
            need_yield = true;
        }
    }
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

#if SOC_RMT_SUPPORT_DMA
static bool IRAM_ATTR rmt_dma_tx_eof_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    rmt_tx_channel_t *tx_chan = (rmt_tx_channel_t *)user_data;
    // tx_eof_desc_addr must be non-zero, guaranteed by the hardware
    rmt_dma_descriptor_t *eof_desc_nc = (rmt_dma_descriptor_t *)RMT_GET_NON_CACHE_ADDR(event_data->tx_eof_desc_addr);
    if (!eof_desc_nc->next) {
        return false;
    }
    // next points to a cache address, convert it to a non-cached one
    rmt_dma_descriptor_t *n = (rmt_dma_descriptor_t *)RMT_GET_NON_CACHE_ADDR(eof_desc_nc->next);
    if (!n->next) {
        return false;
    }
    // if the DMA descriptor link is still a ring (i.e. hasn't broken down by `rmt_tx_mark_eof()`), then we treat it as a valid ping-pong event
    // continue ping-pong transmission
    rmt_tx_trans_desc_t *t = tx_chan->cur_trans;
    size_t encoded_symbols = t->transmitted_symbol_num;
    if (t->flags.encoding_done) {
        rmt_tx_mark_eof(tx_chan);
        encoded_symbols += 1;
    } else {
        encoded_symbols += rmt_encode_check_result(tx_chan, t);
    }
    t->transmitted_symbol_num = encoded_symbols;
    tx_chan->mem_end = tx_chan->ping_pong_symbols * 3 - tx_chan->mem_end; // mem_end equals to either ping_pong_symbols or ping_pong_symbols*2
    // tell DMA that we have a new descriptor attached
    gdma_append(dma_chan);
    return false;
}
#endif // SOC_RMT_SUPPORT_DMA
