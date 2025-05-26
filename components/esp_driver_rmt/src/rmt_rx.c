/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_memory_utils.h"
#include "esp_cache.h"
#include "esp_rom_gpio.h"
#include "soc/rmt_periph.h"
#include "driver/gpio.h"
#include "driver/rmt_rx.h"
#include "rmt_private.h"

static esp_err_t rmt_del_rx_channel(rmt_channel_handle_t channel);
static esp_err_t rmt_rx_demodulate_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config);
static esp_err_t rmt_rx_enable(rmt_channel_handle_t channel);
static esp_err_t rmt_rx_disable(rmt_channel_handle_t channel);
static void rmt_rx_default_isr(void *args);

#if SOC_RMT_SUPPORT_DMA
static bool rmt_dma_rx_one_block_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

__attribute__((always_inline))
static inline void rmt_rx_mount_dma_buffer(rmt_rx_channel_t *rx_chan, const void *buffer, size_t buffer_size, size_t per_block_size, size_t last_block_size)
{
    uint8_t *data = (uint8_t *)buffer;
    for (int i = 0; i < rx_chan->num_dma_nodes; i++) {
        rmt_dma_descriptor_t *desc_nc = &rx_chan->dma_nodes_nc[i];
        desc_nc->buffer = data + i * per_block_size;
        desc_nc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc_nc->dw0.suc_eof = 0;
        desc_nc->dw0.length = 0;
        desc_nc->dw0.size = per_block_size;
    }
    rx_chan->dma_nodes_nc[rx_chan->num_dma_nodes - 1].dw0.size = last_block_size;
}

static esp_err_t rmt_rx_init_dma_link(rmt_rx_channel_t *rx_channel, const rmt_rx_channel_config_t *config)
{
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
#if CONFIG_RMT_RX_ISR_CACHE_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&dma_chan_config, &rx_channel->base.dma_chan), TAG, "allocate RX DMA channel failed");
    gdma_transfer_config_t transfer_cfg = {
        .access_ext_mem = false, // [IDF-8997]: PSRAM is not supported yet
        .max_data_burst_size = 32,
    };
    ESP_RETURN_ON_ERROR(gdma_config_transfer(rx_channel->base.dma_chan, &transfer_cfg), TAG, "config DMA transfer failed");
    // get the alignment requirement from DMA
    gdma_get_alignment_constraints(rx_channel->base.dma_chan, &rx_channel->dma_int_mem_alignment, NULL);

    // register event callbacks
    gdma_rx_event_callbacks_t cbs = {
        .on_recv_done = rmt_dma_rx_one_block_cb,
    };
    // register the DMA callbacks may fail if the interrupt service can not be installed successfully
    ESP_RETURN_ON_ERROR(gdma_register_rx_event_callbacks(rx_channel->base.dma_chan, &cbs, rx_channel), TAG, "register DMA callbacks failed");

    // circular DMA descriptor
    for (int i = 0; i < rx_channel->num_dma_nodes - 1; i++) {
        rx_channel->dma_nodes_nc[i].next = &rx_channel->dma_nodes[i + 1];
    }
    rx_channel->dma_nodes_nc[rx_channel->num_dma_nodes - 1].next = &rx_channel->dma_nodes[0];
    return ESP_OK;
}
#endif // SOC_RMT_SUPPORT_DMA

static esp_err_t rmt_rx_register_to_group(rmt_rx_channel_t *rx_channel, const rmt_rx_channel_config_t *config)
{
    size_t mem_block_num = 0;
    // start to search for a free channel
    // a channel can take up its neighbour's memory block, so the neighbour channel won't work, we should skip these "invaded" ones
    int channel_scan_start = RMT_RX_CHANNEL_OFFSET_IN_GROUP;
    int channel_scan_end = RMT_RX_CHANNEL_OFFSET_IN_GROUP + SOC_RMT_RX_CANDIDATES_PER_GROUP;
    if (config->flags.with_dma) {
        // for DMA mode, the memory block number is always 1; for non-DMA mode, memory block number is configured by user
        mem_block_num = 1;
        // Only the last channel has the DMA capability
        channel_scan_start = RMT_RX_CHANNEL_OFFSET_IN_GROUP + SOC_RMT_RX_CANDIDATES_PER_GROUP - 1;
        rx_channel->ping_pong_symbols = 0; // with DMA, we don't need to do ping-pong
    } else {
        // one channel can occupy multiple memory blocks
        mem_block_num = config->mem_block_symbols / SOC_RMT_MEM_WORDS_PER_CHANNEL;
        if (mem_block_num * SOC_RMT_MEM_WORDS_PER_CHANNEL < config->mem_block_symbols) {
            mem_block_num++;
        }
        rx_channel->ping_pong_symbols = mem_block_num * SOC_RMT_MEM_WORDS_PER_CHANNEL / 2;
    }
    rx_channel->base.mem_block_num = mem_block_num;

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
                channel_id = j - RMT_RX_CHANNEL_OFFSET_IN_GROUP;
                group->rx_channels[channel_id] = rx_channel;
                break;
            }
        }
        portEXIT_CRITICAL(&group->spinlock);
        if (channel_id < 0) {
            // didn't find a capable channel in the group, don't forget to release the group handle
            rmt_release_group_handle(group);
        } else {
            rx_channel->base.channel_id = channel_id;
            rx_channel->base.channel_mask = channel_mask;
            rx_channel->base.group = group;
            break;
        }
    }
    ESP_RETURN_ON_FALSE(channel_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free rx channels");
    return ESP_OK;
}

static void rmt_rx_unregister_from_group(rmt_channel_t *channel, rmt_group_t *group)
{
    portENTER_CRITICAL(&group->spinlock);
    group->rx_channels[channel->channel_id] = NULL;
    group->occupy_mask &= ~(channel->channel_mask << (channel->channel_id + RMT_RX_CHANNEL_OFFSET_IN_GROUP));
    portEXIT_CRITICAL(&group->spinlock);
    // channel has a reference on group, release it now
    rmt_release_group_handle(group);
}

static esp_err_t rmt_rx_destroy(rmt_rx_channel_t *rx_channel)
{
    if (rx_channel->base.gpio_num >= 0) {
        int group_id = rx_channel->base.group->group_id;
        int channel_id = rx_channel->base.channel_id;
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT,
                                       rmt_periph_signals.groups[group_id].channels[channel_id + RMT_RX_CHANNEL_OFFSET_IN_GROUP].rx_sig,
                                       false);
    }
    if (rx_channel->base.intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(rx_channel->base.intr), TAG, "delete interrupt service failed");
    }
#if CONFIG_PM_ENABLE
    if (rx_channel->base.pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(rx_channel->base.pm_lock), TAG, "delete pm_lock failed");
    }
#endif
#if SOC_RMT_SUPPORT_DMA
    if (rx_channel->base.dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_del_channel(rx_channel->base.dma_chan), TAG, "delete dma channel failed");
    }
#endif // SOC_RMT_SUPPORT_DMA
    if (rx_channel->base.group) {
        // de-register channel from RMT group
        rmt_rx_unregister_from_group(&rx_channel->base, rx_channel->base.group);
    }
    if (rx_channel->dma_nodes) {
        free(rx_channel->dma_nodes);
    }
    free(rx_channel);
    return ESP_OK;
}

esp_err_t rmt_new_rx_channel(const rmt_rx_channel_config_t *config, rmt_channel_handle_t *ret_chan)
{
    esp_err_t ret = ESP_OK;
    rmt_rx_channel_t *rx_channel = NULL;
    // Check if priority is valid
    if (config->intr_priority) {
        ESP_RETURN_ON_FALSE((config->intr_priority) > 0, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", config->intr_priority);
        ESP_RETURN_ON_FALSE(1 << (config->intr_priority) & RMT_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", config->intr_priority);
    }
    ESP_RETURN_ON_FALSE(config && ret_chan && config->resolution_hz, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, TAG, "invalid GPIO number %d", config->gpio_num);
    ESP_RETURN_ON_FALSE((config->mem_block_symbols & 0x01) == 0 && config->mem_block_symbols >= SOC_RMT_MEM_WORDS_PER_CHANNEL,
                        ESP_ERR_INVALID_ARG, TAG, "mem_block_symbols must be even and at least %d", SOC_RMT_MEM_WORDS_PER_CHANNEL);
#if SOC_RMT_SUPPORT_DMA
    if (config->flags.with_dma) {
#if CONFIG_RMT_RX_ISR_CACHE_SAFE && (!CONFIG_GDMA_ISR_HANDLER_IN_IRAM || !CONFIG_GDMA_CTRL_FUNC_IN_IRAM)
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "CONFIG_GDMA_ISR_HANDLER_IN_IRAM and CONFIG_GDMA_CTRL_FUNC_IN_IRAM must be enabled");
#endif
    }
#else
    ESP_RETURN_ON_FALSE(config->flags.with_dma == 0, ESP_ERR_NOT_SUPPORTED, TAG, "DMA not supported");
#endif // SOC_RMT_SUPPORT_DMA

#if !SOC_RMT_SUPPORT_SLEEP_RETENTION
    ESP_RETURN_ON_FALSE(config->flags.allow_pd == 0, ESP_ERR_NOT_SUPPORTED, TAG, "not able to power down in light sleep");
#endif // SOC_RMT_SUPPORT_SLEEP_RETENTION

    // malloc channel memory
    uint32_t mem_caps = RMT_MEM_ALLOC_CAPS;
    rx_channel = heap_caps_calloc(1, sizeof(rmt_rx_channel_t), mem_caps);
    ESP_GOTO_ON_FALSE(rx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for rx channel");
    // gpio is not configured yet
    rx_channel->base.gpio_num = -1;

#if SOC_RMT_SUPPORT_DMA
    // create DMA descriptor
    size_t num_dma_nodes = 0;
    if (config->flags.with_dma) {
        // DMA descriptors must be placed in internal SRAM
        mem_caps |= MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
        num_dma_nodes = config->mem_block_symbols * sizeof(rmt_symbol_word_t) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE + 1;
        num_dma_nodes = MAX(2, num_dma_nodes); // at least 2 DMA nodes for ping-pong
        rmt_dma_descriptor_t *dma_nodes =  heap_caps_aligned_calloc(RMT_DMA_DESC_ALIGN, num_dma_nodes, sizeof(rmt_dma_descriptor_t), mem_caps);
        ESP_GOTO_ON_FALSE(dma_nodes, ESP_ERR_NO_MEM, err, TAG, "no mem for rx channel DMA nodes");
        rx_channel->dma_nodes = dma_nodes;
        // do memory sync only when the data cache exists
        uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
        if (data_cache_line_size) {
            // write back and then invalidate the cached dma_nodes, because later the DMA nodes are accessed by non-cacheable address
            ESP_GOTO_ON_ERROR(esp_cache_msync(dma_nodes, num_dma_nodes * sizeof(rmt_dma_descriptor_t),
                                              ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE | ESP_CACHE_MSYNC_FLAG_UNALIGNED),
                              err, TAG, "cache sync failed");
        }
        // we will use the non-cached address to manipulate the DMA descriptor, for simplicity
        rx_channel->dma_nodes_nc = (rmt_dma_descriptor_t *)RMT_GET_NON_CACHE_ADDR(dma_nodes);
    }
    rx_channel->num_dma_nodes = num_dma_nodes;
#endif // SOC_RMT_SUPPORT_DMA

    // register the channel to group
    ESP_GOTO_ON_ERROR(rmt_rx_register_to_group(rx_channel, config), err, TAG, "register channel failed");
    rmt_group_t *group = rx_channel->base.group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = rx_channel->base.channel_id;
    int group_id = group->group_id;

#if RMT_USE_RETENTION_LINK
    if (config->flags.allow_pd != 0) {
        rmt_create_retention_module(group);
    }
#endif // RMT_USE_RETENTION_LINK

    // reset channel, make sure the RX engine is not working, and events are cleared
    portENTER_CRITICAL(&group->spinlock);
    rmt_hal_rx_channel_reset(&group->hal, channel_id);
    portEXIT_CRITICAL(&group->spinlock);

    // When channel receives an end-maker, a DMA in_suc_eof interrupt will be generated
    // So we don't rely on RMT interrupt any more, GDMA event callback is sufficient
    if (config->flags.with_dma) {
#if SOC_RMT_SUPPORT_DMA
        ESP_GOTO_ON_ERROR(rmt_rx_init_dma_link(rx_channel, config), err, TAG, "install rx DMA failed");
#endif // SOC_RMT_SUPPORT_DMA
    } else {
        // RMT interrupt is mandatory if the channel doesn't use DMA
        // --- install interrupt service
        // interrupt is mandatory to run basic RMT transactions, so it's not lazy installed in `rmt_tx_register_event_callbacks()`
        // 1-- Set user specified priority to `group->intr_priority`
        bool priority_conflict = rmt_set_intr_priority_to_group(group, config->intr_priority);
        ESP_GOTO_ON_FALSE(!priority_conflict, ESP_ERR_INVALID_ARG, err, TAG, "intr_priority conflict");
        // 2-- Get interrupt allocation flag
        int isr_flags = rmt_isr_priority_to_flags(group) | RMT_RX_INTR_ALLOC_FLAG;
        // 3-- Allocate interrupt using isr_flag
        ret = esp_intr_alloc_intrstatus(rmt_periph_signals.groups[group_id].irq, isr_flags,
                                        (uint32_t)rmt_ll_get_interrupt_status_reg(hal->regs),
                                        RMT_LL_EVENT_RX_MASK(channel_id), rmt_rx_default_isr, rx_channel, &rx_channel->base.intr);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "install rx interrupt failed");
    }

    rx_channel->base.direction = RMT_CHANNEL_DIRECTION_RX;
    // select the clock source and set clock resolution
    ESP_GOTO_ON_ERROR(rmt_select_periph_clock(&rx_channel->base, config->clk_src, config->resolution_hz), err, TAG, "set clock resolution failed");

    rx_channel->filter_clock_resolution_hz = group->resolution_hz;
    // On esp32 and esp32s2, the counting clock used by the RX filter always comes from APB clock
    // no matter what the clock source is used by the RMT channel as the "core" clock
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_APB, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &rx_channel->filter_clock_resolution_hz);
#endif

    rmt_ll_rx_set_mem_blocks(hal->regs, channel_id, rx_channel->base.mem_block_num);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
#if SOC_RMT_SUPPORT_RX_PINGPONG
    rmt_ll_rx_set_limit(hal->regs, channel_id, rx_channel->ping_pong_symbols);
    // always enable rx wrap, both DMA mode and ping-pong mode rely this feature
    rmt_ll_rx_enable_wrap(hal->regs, channel_id, true);
#endif
#if SOC_RMT_SUPPORT_RX_DEMODULATION
    // disable carrier demodulation by default, can re-enable by `rmt_apply_carrier()`
    rmt_ll_rx_enable_carrier_demodulation(hal->regs, channel_id, false);
#endif

    // GPIO Matrix/MUX configuration
    gpio_func_sel(config->gpio_num, PIN_FUNC_GPIO);
    gpio_input_enable(config->gpio_num);
    esp_rom_gpio_connect_in_signal(config->gpio_num,
                                   rmt_periph_signals.groups[group_id].channels[channel_id + RMT_RX_CHANNEL_OFFSET_IN_GROUP].rx_sig,
                                   config->flags.invert_in);
    rx_channel->base.gpio_num = config->gpio_num;

    // initialize other members of rx channel
    portMUX_INITIALIZE(&rx_channel->base.spinlock);
    atomic_init(&rx_channel->base.fsm, RMT_FSM_INIT);
    rx_channel->base.hw_mem_base = &RMTMEM.channels[channel_id + RMT_RX_CHANNEL_OFFSET_IN_GROUP].symbols[0];
    // polymorphic methods
    rx_channel->base.del = rmt_del_rx_channel;
    rx_channel->base.set_carrier_action = rmt_rx_demodulate_carrier;
    rx_channel->base.enable = rmt_rx_enable;
    rx_channel->base.disable = rmt_rx_disable;
    // return general channel handle
    *ret_chan = &rx_channel->base;
    ESP_LOGD(TAG, "new rx channel(%d,%d) at %p, gpio=%d, res=%"PRIu32"Hz, hw_mem_base=%p, ping_pong_size=%d",
             group_id, channel_id, rx_channel, config->gpio_num, rx_channel->base.resolution_hz,
             rx_channel->base.hw_mem_base, rx_channel->ping_pong_symbols);
    return ESP_OK;

err:
    if (rx_channel) {
        rmt_rx_destroy(rx_channel);
    }
    return ret;
}

static esp_err_t rmt_del_rx_channel(rmt_channel_handle_t channel)
{
    ESP_RETURN_ON_FALSE(atomic_load(&channel->fsm) == RMT_FSM_INIT,
                        ESP_ERR_INVALID_STATE, TAG, "channel not in init state");
    rmt_rx_channel_t *rx_chan = __containerof(channel, rmt_rx_channel_t, base);
    rmt_group_t *group = channel->group;
    int group_id = group->group_id;
    int channel_id = channel->channel_id;
    ESP_LOGD(TAG, "del rx channel(%d,%d)", group_id, channel_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(rmt_rx_destroy(rx_chan), TAG, "destroy rx channel failed");
    return ESP_OK;
}

esp_err_t rmt_rx_register_event_callbacks(rmt_channel_handle_t channel, const rmt_rx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(channel && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->direction == RMT_CHANNEL_DIRECTION_RX, ESP_ERR_INVALID_ARG, TAG, "invalid channel direction");
    rmt_rx_channel_t *rx_chan = __containerof(channel, rmt_rx_channel_t, base);

#if CONFIG_RMT_RX_ISR_CACHE_SAFE
    if (cbs->on_recv_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_recv_done), ESP_ERR_INVALID_ARG, TAG, "on_recv_done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    rx_chan->on_recv_done = cbs->on_recv_done;
    rx_chan->user_data = user_data;
    return ESP_OK;
}

esp_err_t rmt_receive(rmt_channel_handle_t channel, void *buffer, size_t buffer_size, const rmt_receive_config_t *config)
{
    ESP_RETURN_ON_FALSE_ISR(channel && buffer && buffer_size && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(channel->direction == RMT_CHANNEL_DIRECTION_RX, ESP_ERR_INVALID_ARG, TAG, "invalid channel direction");
#if !SOC_RMT_SUPPORT_RX_PINGPONG
    ESP_RETURN_ON_FALSE_ISR(!config->flags.en_partial_rx, ESP_ERR_NOT_SUPPORTED, TAG, "partial receive not supported");
#endif
    rmt_rx_channel_t *rx_chan = __containerof(channel, rmt_rx_channel_t, base);
    size_t mem_alignment = sizeof(rmt_symbol_word_t);

#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        // append the alignment requirement from the DMA
        mem_alignment = MAX(mem_alignment, rx_chan->dma_int_mem_alignment);
        // [IDF-8997]: Currently we assume the user buffer is allocated from internal RAM, PSRAM is not supported yet.
        ESP_RETURN_ON_FALSE_ISR(esp_ptr_internal(buffer), ESP_ERR_INVALID_ARG, TAG, "user buffer not in the internal RAM");
        size_t max_buf_sz_per_dma_node = ALIGN_DOWN(DMA_DESCRIPTOR_BUFFER_MAX_SIZE, mem_alignment);
        ESP_RETURN_ON_FALSE_ISR(buffer_size <= rx_chan->num_dma_nodes * max_buf_sz_per_dma_node,
                                ESP_ERR_INVALID_ARG, TAG, "buffer size exceeds DMA capacity: %zu", rx_chan->num_dma_nodes * max_buf_sz_per_dma_node);
    }
#endif // SOC_RMT_SUPPORT_DMA

    // check buffer alignment
    uint32_t align_check_mask = mem_alignment - 1;
    ESP_RETURN_ON_FALSE_ISR(((((uintptr_t)buffer) & align_check_mask) == 0) && (((buffer_size) & align_check_mask) == 0), ESP_ERR_INVALID_ARG,
                            TAG, "buffer address or size are not %"PRIu32 "bytes aligned", mem_alignment);

    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

    uint32_t filter_reg_value = ((uint64_t)rx_chan->filter_clock_resolution_hz * config->signal_range_min_ns) / 1000000000UL;
    uint32_t idle_reg_value = ((uint64_t)channel->resolution_hz * config->signal_range_max_ns) / 1000000000UL;
    ESP_RETURN_ON_FALSE_ISR(filter_reg_value <= RMT_LL_MAX_FILTER_VALUE, ESP_ERR_INVALID_ARG, TAG, "signal_range_min_ns too big, should be less than %"PRIu32" ns", (uint32_t)((uint64_t)RMT_LL_MAX_FILTER_VALUE * 1000000000UL / rx_chan->filter_clock_resolution_hz));
    ESP_RETURN_ON_FALSE_ISR(idle_reg_value <= RMT_LL_MAX_IDLE_VALUE, ESP_ERR_INVALID_ARG, TAG, "signal_range_max_ns too big, should be less than %"PRIu32" ns", (uint32_t)((uint64_t)RMT_LL_MAX_IDLE_VALUE * 1000000000UL / channel->resolution_hz));

    // check if we're in a proper state to start the receiver
    rmt_fsm_t expected_fsm = RMT_FSM_ENABLE;
    ESP_RETURN_ON_FALSE_ISR(atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_WAIT),
                            ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");

    // fill in the transaction descriptor
    rmt_rx_trans_desc_t *t = &rx_chan->trans_desc;
    memset(t, 0, sizeof(rmt_rx_trans_desc_t));
    t->buffer = buffer;
    t->buffer_size = buffer_size;
    t->received_symbol_num = 0;
    t->copy_dest_off = 0;
    t->dma_desc_index = 0;
    t->flags.en_partial_rx = config->flags.en_partial_rx;

#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        // invalidate the user buffer, in case cache auto-write back happens and breaks the data just written by the DMA
        uint32_t int_mem_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
        if (int_mem_cache_line_size) {
            // this function will also check the alignment of the buffer and size, against the cache line size
            ESP_RETURN_ON_ERROR_ISR(esp_cache_msync(buffer, buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C), TAG, "cache sync failed");
        }
        // we will mount the buffer to multiple DMA nodes, in a balanced way
        size_t per_dma_block_size = buffer_size / rx_chan->num_dma_nodes;
        per_dma_block_size = ALIGN_DOWN(per_dma_block_size, mem_alignment);
        size_t last_dma_block_size = buffer_size - per_dma_block_size * (rx_chan->num_dma_nodes - 1);
        rmt_rx_mount_dma_buffer(rx_chan, buffer, buffer_size, per_dma_block_size, last_dma_block_size);
        gdma_reset(channel->dma_chan);
        gdma_start(channel->dma_chan, (intptr_t)rx_chan->dma_nodes); // note, we must use the cached descriptor address to start the DMA
    }
#endif

    rx_chan->mem_off = 0;
    portENTER_CRITICAL_SAFE(&channel->spinlock);
    // reset memory writer offset
    rmt_ll_rx_reset_pointer(hal->regs, channel_id);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
    // set sampling parameters of incoming signals
    rmt_ll_rx_set_filter_thres(hal->regs, channel_id, filter_reg_value);
    rmt_ll_rx_enable_filter(hal->regs, channel_id, config->signal_range_min_ns != 0);
    rmt_ll_rx_set_idle_thres(hal->regs, channel_id, idle_reg_value);
    // turn on RMT RX machine
    rmt_ll_rx_enable(hal->regs, channel_id, true);

    // saying we're in running state, this state will last until the receiving is done
    // i.e., we will switch back to the enable state in the receive done interrupt handler
    atomic_store(&channel->fsm, RMT_FSM_RUN);
    portEXIT_CRITICAL_SAFE(&channel->spinlock);

    return ESP_OK;
}

static esp_err_t rmt_rx_demodulate_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config)
{
#if !SOC_RMT_SUPPORT_RX_DEMODULATION
    ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "rx demodulation not supported");
#else
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int group_id = group->group_id;
    int channel_id = channel->channel_id;
    uint32_t real_frequency = 0;

    if (config && config->frequency_hz) {
        // carrier demodulation module works base on channel clock (this is different from TX carrier modulation mode)
        uint32_t total_ticks = channel->resolution_hz / config->frequency_hz; // Note this division operation will lose precision
        uint32_t high_ticks = total_ticks * config->duty_cycle;
        uint32_t low_ticks = total_ticks - high_ticks;

        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_rx_set_carrier_level(hal->regs, channel_id, !config->flags.polarity_active_low);
        rmt_ll_rx_set_carrier_high_low_ticks(hal->regs, channel_id, high_ticks, low_ticks);
        portEXIT_CRITICAL(&channel->spinlock);
        // save real carrier frequency
        real_frequency = channel->resolution_hz / (high_ticks + low_ticks);
    }

    // enable/disable carrier demodulation
    portENTER_CRITICAL(&channel->spinlock);
    rmt_ll_rx_enable_carrier_demodulation(hal->regs, channel_id, real_frequency > 0);
    portEXIT_CRITICAL(&channel->spinlock);

    if (real_frequency > 0) {
        ESP_LOGD(TAG, "enable carrier demodulation for channel(%d,%d), freq=%"PRIu32"Hz", group_id, channel_id, real_frequency);
    } else {
        ESP_LOGD(TAG, "disable carrier demodulation for channel(%d, %d)", group_id, channel_id);
    }
    return ESP_OK;
#endif
}

static esp_err_t rmt_rx_enable(rmt_channel_handle_t channel)
{
    // can only enable the channel when it's in "init" state
    rmt_fsm_t expected_fsm = RMT_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_WAIT),
                        ESP_ERR_INVALID_STATE, TAG, "channel not in init state");

    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

#if CONFIG_PM_ENABLE
    // acquire power manager lock
    if (channel->pm_lock) {
        esp_pm_lock_acquire(channel->pm_lock);
    }
#endif

    if (channel->dma_chan) {
#if SOC_RMT_SUPPORT_DMA
        // enable the DMA access mode
        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_rx_enable_dma(hal->regs, channel_id, true);
        portEXIT_CRITICAL(&channel->spinlock);

        gdma_connect(channel->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_RMT, 0));
#endif // SOC_RMT_SUPPORT_DMA
    } else {
        portENTER_CRITICAL(&group->spinlock);
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_RX_MASK(channel_id), true);
        portEXIT_CRITICAL(&group->spinlock);
    }

    atomic_store(&channel->fsm, RMT_FSM_ENABLE);

    return ESP_OK;
}

static esp_err_t rmt_rx_disable(rmt_channel_handle_t channel)
{
    // can disable the channel when it's in `enable` or `run` state
    bool valid_state = false;
    rmt_fsm_t expected_fsm = RMT_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_WAIT)) {
        valid_state = true;
    }
    expected_fsm = RMT_FSM_RUN;
    if (atomic_compare_exchange_strong(&channel->fsm, &expected_fsm, RMT_FSM_WAIT)) {
        valid_state = true;
    }
    ESP_RETURN_ON_FALSE(valid_state, ESP_ERR_INVALID_STATE, TAG, "channel not in enable or run state");

    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

    portENTER_CRITICAL(&channel->spinlock);
    rmt_ll_rx_enable(hal->regs, channel_id, false);
    portEXIT_CRITICAL(&channel->spinlock);

    if (channel->dma_chan) {
#if SOC_RMT_SUPPORT_DMA
        gdma_stop(channel->dma_chan);
        gdma_disconnect(channel->dma_chan);
        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_rx_enable_dma(hal->regs, channel_id, false);
        portEXIT_CRITICAL(&channel->spinlock);
#endif
    } else {
        portENTER_CRITICAL(&group->spinlock);
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_RX_MASK(channel_id), false);
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_MASK(channel_id));
        portEXIT_CRITICAL(&group->spinlock);
    }

#if CONFIG_PM_ENABLE
    // release power manager lock
    if (channel->pm_lock) {
        esp_pm_lock_release(channel->pm_lock);
    }
#endif

    // now we can switch the state to init
    atomic_store(&channel->fsm, RMT_FSM_INIT);
    return ESP_OK;
}

bool rmt_isr_handle_rx_done(rmt_rx_channel_t *rx_chan)
{
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    rmt_rx_trans_desc_t *trans_desc = &rx_chan->trans_desc;
    rmt_rx_done_callback_t cb = rx_chan->on_recv_done;
    bool need_yield = false;

    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_DONE(channel_id));
    portENTER_CRITICAL_ISR(&channel->spinlock);
    // disable the RX engine, it will be enabled again when next time user calls `rmt_receive()`
    rmt_ll_rx_enable(hal->regs, channel_id, false);
    portEXIT_CRITICAL_ISR(&channel->spinlock);

    uint32_t offset = rmt_ll_rx_get_memory_writer_offset(hal->regs, channel_id);

    // Start from C6, the actual pulse count is the number of input pulses N - 1.
    // Resulting in the last threshold interrupts may not be triggered correctly when the number of received symbols is a multiple of the memory block size.
    // As shown in the figure below, So we special handle the offset

    //  mem_off (should be updated here in the last threshold interrupt, but interrupt lost)
    //     |
    //     V
    //     |________|________|
    //     |        |
    //   offset   mem_off(actually here now)

    size_t mem_want = (offset >= rx_chan->mem_off ? offset - rx_chan->mem_off : rx_chan->mem_off - offset);
    mem_want *= sizeof(rmt_symbol_word_t);
    size_t mem_have = trans_desc->buffer_size - trans_desc->copy_dest_off;
    size_t copy_size = mem_want;
    if (mem_want > mem_have) {
        if (trans_desc->flags.en_partial_rx) { // check partial receive is enabled or not
            // notify the user to process the received symbols if the buffer is going to be full
            if (trans_desc->received_symbol_num) {
                if (cb) {
                    rmt_rx_done_event_data_t edata = {
                        .received_symbols = trans_desc->buffer,
                        .num_symbols = trans_desc->received_symbol_num,
                        .flags.is_last = false,
                    };
                    if (cb(channel, &edata, rx_chan->user_data)) {
                        need_yield = true;
                    }
                }
                trans_desc->copy_dest_off = 0;
                trans_desc->received_symbol_num = 0;
                mem_have = trans_desc->buffer_size;

                // even user process the partial received data, the remain buffer may still be insufficient
                if (mem_want > mem_have) {
                    ESP_DRAM_LOGD(TAG, "user buffer too small, received symbols truncated");
                    copy_size = mem_have;
                }
            }
        } else {
            ESP_DRAM_LOGD(TAG, "user buffer too small, received symbols truncated");
            copy_size = mem_have;
        }
    }

    portENTER_CRITICAL_ISR(&channel->spinlock);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_SW);
    // copy the symbols to the user buffer
    memcpy((uint8_t *)trans_desc->buffer + trans_desc->copy_dest_off, channel->hw_mem_base + rx_chan->mem_off, copy_size);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
    portEXIT_CRITICAL_ISR(&channel->spinlock);

#if !SOC_RMT_SUPPORT_RX_PINGPONG
    // for chips doesn't support ping-pong RX, we should check whether the receiver has encountered with a long frame,
    // whose length is longer than the channel capacity
    if (rmt_ll_rx_get_interrupt_status_raw(hal->regs, channel_id) & RMT_LL_EVENT_RX_ERROR(channel_id)) {
        portENTER_CRITICAL_ISR(&channel->spinlock);
        rmt_ll_rx_reset_pointer(hal->regs, channel_id);
        portEXIT_CRITICAL_ISR(&channel->spinlock);
        // this clear operation can only take effect after we copy out the received data and reset the pointer
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_ERROR(channel_id));
        ESP_DRAM_LOGD(TAG, "hw buffer too small, received symbols truncated");
    }
#endif // !SOC_RMT_SUPPORT_RX_PINGPONG

    trans_desc->copy_dest_off += copy_size;
    trans_desc->received_symbol_num += copy_size / sizeof(rmt_symbol_word_t);
    // switch back to the enable state, then user can call `rmt_receive` to start a new receive
    atomic_store(&channel->fsm, RMT_FSM_ENABLE);

    // notify the user that all RMT symbols are received done
    if (cb) {
        rmt_rx_done_event_data_t edata = {
            .received_symbols = trans_desc->buffer,
            .num_symbols = trans_desc->received_symbol_num,
            .flags.is_last = true,
        };
        if (cb(channel, &edata, rx_chan->user_data)) {
            need_yield = true;
        }
    }
    return need_yield;
}

#if SOC_RMT_SUPPORT_RX_PINGPONG
bool rmt_isr_handle_rx_threshold(rmt_rx_channel_t *rx_chan)
{
    bool need_yield = false;
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    rmt_rx_trans_desc_t *trans_desc = &rx_chan->trans_desc;

    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_THRES(channel_id));

    size_t mem_want = rx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t);
    size_t mem_have = trans_desc->buffer_size - trans_desc->copy_dest_off;
    size_t copy_size = mem_want;
    if (mem_want > mem_have) {
        if (trans_desc->flags.en_partial_rx) {
            // notify the user to process the received symbols if the buffer is going to be full
            if (trans_desc->received_symbol_num) {
                rmt_rx_done_callback_t cb = rx_chan->on_recv_done;
                if (cb) {
                    rmt_rx_done_event_data_t edata = {
                        .received_symbols = trans_desc->buffer,
                        .num_symbols = trans_desc->received_symbol_num,
                        .flags.is_last = false,
                    };
                    if (cb(channel, &edata, rx_chan->user_data)) {
                        need_yield = true;
                    }
                }
                trans_desc->copy_dest_off = 0;
                trans_desc->received_symbol_num = 0;
                mem_have = trans_desc->buffer_size;

                // even user process the partial received data, the remain buffer size still insufficient
                if (mem_want > mem_have) {
                    ESP_DRAM_LOGD(TAG, "user buffer too small, received symbols truncated");
                    copy_size = mem_have;
                }
            }
        } else {
            ESP_DRAM_LOGD(TAG, "user buffer too small, received symbols truncated");
            copy_size = mem_have;
        }
    }

    portENTER_CRITICAL_ISR(&channel->spinlock);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_SW);
    // copy the symbols to the user buffer
    memcpy((uint8_t *)trans_desc->buffer + trans_desc->copy_dest_off, channel->hw_mem_base + rx_chan->mem_off, copy_size);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
    portEXIT_CRITICAL_ISR(&channel->spinlock);

    trans_desc->copy_dest_off += copy_size;
    trans_desc->received_symbol_num += copy_size / sizeof(rmt_symbol_word_t);

    // update the hw memory offset, where stores the next RMT symbols to copy
    rx_chan->mem_off = rx_chan->ping_pong_symbols - rx_chan->mem_off;

    return need_yield;
}
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

static void rmt_rx_default_isr(void *args)
{
    rmt_rx_channel_t *rx_chan = (rmt_rx_channel_t *)args;
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    bool need_yield = false;

    uint32_t status = rmt_ll_rx_get_interrupt_status(hal->regs, channel_id);

#if SOC_RMT_SUPPORT_RX_PINGPONG
    // RX threshold interrupt
    if (status & RMT_LL_EVENT_RX_THRES(channel_id)) {
        if (rmt_isr_handle_rx_threshold(rx_chan)) {
            need_yield = true;
        }
    }
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

    // RX end interrupt
    if (status & RMT_LL_EVENT_RX_DONE(channel_id)) {
        if (rmt_isr_handle_rx_done(rx_chan)) {
            need_yield = true;
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

#if SOC_RMT_SUPPORT_DMA
__attribute__((always_inline))
static inline size_t rmt_rx_count_symbols_until_eof(rmt_rx_channel_t *rx_chan, int start_index)
{
    size_t received_bytes = 0;
    for (int i = 0; i < rx_chan->num_dma_nodes; i++) {
        received_bytes += rx_chan->dma_nodes_nc[start_index].dw0.length;
        if (rx_chan->dma_nodes_nc[start_index].dw0.suc_eof) {
            break;
        }
        start_index++;
        start_index %= rx_chan->num_dma_nodes;
    }
    received_bytes = ALIGN_UP(received_bytes, sizeof(rmt_symbol_word_t));
    return received_bytes / sizeof(rmt_symbol_word_t);
}

__attribute__((always_inline))
static inline size_t rmt_rx_count_symbols_for_single_block(rmt_rx_channel_t *rx_chan, int desc_index)
{
    size_t received_bytes = rx_chan->dma_nodes_nc[desc_index].dw0.length;
    received_bytes = ALIGN_UP(received_bytes, sizeof(rmt_symbol_word_t));
    return received_bytes / sizeof(rmt_symbol_word_t);
}

static bool rmt_dma_rx_one_block_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    rmt_rx_channel_t *rx_chan = (rmt_rx_channel_t *)user_data;
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    rmt_rx_trans_desc_t *trans_desc = &rx_chan->trans_desc;
    uint32_t channel_id = channel->channel_id;

    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    if (data_cache_line_size) {
        // invalidate the user buffer, so that the DMA modified data can be seen by CPU
        esp_cache_msync(trans_desc->buffer, trans_desc->buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
    }

    if (event_data->flags.normal_eof) {
        // if the DMA received an EOF, it means the RMT peripheral has received an "end marker"
        portENTER_CRITICAL_ISR(&channel->spinlock);
        // disable the RX engine, it will be enabled again in the next `rmt_receive()`
        rmt_ll_rx_enable(hal->regs, channel_id, false);
        portEXIT_CRITICAL_ISR(&channel->spinlock);

        // switch back to the enable state, then user can call `rmt_receive` to start a new receive
        atomic_store(&channel->fsm, RMT_FSM_ENABLE);

        if (rx_chan->on_recv_done) {
            int recycle_start_index = trans_desc->dma_desc_index;
            rmt_rx_done_event_data_t edata = {
                .received_symbols = rx_chan->dma_nodes_nc[recycle_start_index].buffer,
                .num_symbols = rmt_rx_count_symbols_until_eof(rx_chan, recycle_start_index),
                .flags.is_last = true,
            };
            if (rx_chan->on_recv_done(channel, &edata, rx_chan->user_data)) {
                need_yield = true;
            }
        }
    } else {
        // it's a partial receive done event
        if (trans_desc->flags.en_partial_rx) {
            if (rx_chan->on_recv_done) {
                size_t dma_desc_index = trans_desc->dma_desc_index;
                rmt_rx_done_event_data_t edata = {
                    .received_symbols = rx_chan->dma_nodes_nc[dma_desc_index].buffer,
                    .num_symbols = rmt_rx_count_symbols_for_single_block(rx_chan, dma_desc_index),
                    .flags.is_last = false,
                };
                if (rx_chan->on_recv_done(channel, &edata, rx_chan->user_data)) {
                    need_yield = true;
                }

                dma_desc_index++;
                trans_desc->dma_desc_index = dma_desc_index % rx_chan->num_dma_nodes;
            }
        }
    }

    return need_yield;
}

#endif // SOC_RMT_SUPPORT_DMA
