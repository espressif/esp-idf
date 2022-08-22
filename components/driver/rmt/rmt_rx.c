/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp_memory_utils.h"
#include "esp_rom_gpio.h"
#include "soc/rmt_periph.h"
#include "soc/rtc.h"
#include "hal/rmt_ll.h"
#include "hal/gpio_hal.h"
#include "driver/gpio.h"
#include "driver/rmt_rx.h"
#include "rmt_private.h"

#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

static const char *TAG = "rmt";

static esp_err_t rmt_del_rx_channel(rmt_channel_handle_t channel);
static esp_err_t rmt_rx_demodulate_carrier(rmt_channel_handle_t channel, const rmt_carrier_config_t *config);
static esp_err_t rmt_rx_enable(rmt_channel_handle_t channel);
static esp_err_t rmt_rx_disable(rmt_channel_handle_t channel);
static void rmt_rx_default_isr(void *args);

#if SOC_RMT_SUPPORT_DMA
static bool rmt_dma_rx_eof_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

static void rmt_rx_mount_dma_buffer(dma_descriptor_t *desc_array, size_t array_size, const void *buffer, size_t buffer_size)
{
    size_t prepared_length = 0;
    uint8_t *data = (uint8_t *)buffer;
    int dma_node_i = 0;
    dma_descriptor_t *desc = NULL;
    while (buffer_size > RMT_DMA_DESC_BUF_MAX_SIZE) {
        desc = &desc_array[dma_node_i];
        desc->dw0.suc_eof = 0;
        desc->dw0.size = RMT_DMA_DESC_BUF_MAX_SIZE;
        desc->dw0.length = 0;
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc->buffer = &data[prepared_length];
        desc->next = &desc_array[dma_node_i + 1];
        prepared_length += RMT_DMA_DESC_BUF_MAX_SIZE;
        buffer_size -= RMT_DMA_DESC_BUF_MAX_SIZE;
        dma_node_i++;
    }
    if (buffer_size) {
        desc = &desc_array[dma_node_i];
        desc->dw0.suc_eof = 0;
        desc->dw0.size = buffer_size;
        desc->dw0.length = 0;
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc->buffer = &data[prepared_length];
        prepared_length += buffer_size;
    }
    desc->next = NULL; // one-off DMA chain
}

static esp_err_t rmt_rx_init_dma_link(rmt_rx_channel_t *rx_channel, const rmt_rx_channel_config_t *config)
{
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
    };
    ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_chan_config, &rx_channel->base.dma_chan), TAG, "allocate RX DMA channel failed");
    gdma_strategy_config_t gdma_strategy_conf = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    gdma_apply_strategy(rx_channel->base.dma_chan, &gdma_strategy_conf);
    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = rmt_dma_rx_eof_cb,
    };
    gdma_register_rx_event_callbacks(rx_channel->base.dma_chan, &cbs, rx_channel);
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
            group = NULL;
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

static esp_err_t rmt_rx_destory(rmt_rx_channel_t *rx_channel)
{
    if (rx_channel->base.intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(rx_channel->base.intr), TAG, "delete interrupt service failed");
    }
    if (rx_channel->base.pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(rx_channel->base.pm_lock), TAG, "delete pm_lock failed");
    }
#if SOC_RMT_SUPPORT_DMA
    if (rx_channel->base.dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_del_channel(rx_channel->base.dma_chan), TAG, "delete dma channel failed");
    }
#endif // SOC_RMT_SUPPORT_DMA
    if (rx_channel->base.group) {
        // de-register channel from RMT group
        rmt_rx_unregister_from_group(&rx_channel->base, rx_channel->base.group);
    }
    free(rx_channel);
    return ESP_OK;
}

esp_err_t rmt_new_rx_channel(const rmt_rx_channel_config_t *config, rmt_channel_handle_t *ret_chan)
{
#if CONFIG_RMT_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    rmt_rx_channel_t *rx_channel = NULL;
    ESP_GOTO_ON_FALSE(config && ret_chan && config->resolution_hz, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(GPIO_IS_VALID_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, err, TAG, "invalid GPIO number");
    ESP_GOTO_ON_FALSE((config->mem_block_symbols & 0x01) == 0 && config->mem_block_symbols >= SOC_RMT_MEM_WORDS_PER_CHANNEL,
                      ESP_ERR_INVALID_ARG, err, TAG, "mem_block_symbols must be even and at least %d", SOC_RMT_MEM_WORDS_PER_CHANNEL);
#if !SOC_RMT_SUPPORT_DMA
    ESP_GOTO_ON_FALSE(config->flags.with_dma == 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "DMA not supported");
#endif // SOC_RMT_SUPPORT_DMA

    size_t num_dma_nodes = 0;
    if (config->flags.with_dma) {
        num_dma_nodes = config->mem_block_symbols * sizeof(rmt_symbol_word_t) / RMT_DMA_DESC_BUF_MAX_SIZE + 1;
    }
    // malloc channel memory
    uint32_t mem_caps = RMT_MEM_ALLOC_CAPS;
    if (config->flags.with_dma) {
        // DMA descriptors must be placed in internal SRAM
        mem_caps |= MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
    }
    rx_channel = heap_caps_calloc(1, sizeof(rmt_rx_channel_t) + num_dma_nodes * sizeof(dma_descriptor_t), mem_caps);
    ESP_GOTO_ON_FALSE(rx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for rx channel");
    rx_channel->num_dma_nodes = num_dma_nodes;
    // register the channel to group
    ESP_GOTO_ON_ERROR(rmt_rx_register_to_group(rx_channel, config), err, TAG, "register channel failed");
    rmt_group_t *group = rx_channel->base.group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = rx_channel->base.channel_id;
    int group_id = group->group_id;
    // select the clock source
    ESP_GOTO_ON_ERROR(rmt_select_periph_clock(&rx_channel->base, config->clk_src), err, TAG, "set group clock failed");

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
        int isr_flags = RMT_INTR_ALLOC_FLAG;
        ret = esp_intr_alloc_intrstatus(rmt_periph_signals.groups[group_id].irq, isr_flags,
                                        (uint32_t)rmt_ll_get_interrupt_status_reg(hal->regs),
                                        RMT_LL_EVENT_RX_MASK(channel_id), rmt_rx_default_isr, rx_channel, &rx_channel->base.intr);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "install rx interrupt failed");
    }

    // set channel clock resolution
    uint32_t real_div = group->resolution_hz / config->resolution_hz;
    rmt_ll_rx_set_channel_clock_div(hal->regs, channel_id, real_div);
    // resolution loss due to division, calculate the real resolution
    rx_channel->base.resolution_hz = group->resolution_hz / real_div;
    if (rx_channel->base.resolution_hz != config->resolution_hz) {
        ESP_LOGW(TAG, "channel resolution loss, real=%"PRIu32, rx_channel->base.resolution_hz);
    }

    rmt_ll_rx_set_mem_blocks(hal->regs, channel_id, rx_channel->base.mem_block_num);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
#if SOC_RMT_SUPPORT_RX_PINGPONG
    rmt_ll_rx_set_limit(hal->regs, channel_id, rx_channel->ping_pong_symbols);
    // always enable rx wrap, both DMA mode and ping-pong mode rely this feature
    rmt_ll_rx_enable_wrap(hal->regs, channel_id, true);
#endif
#if SOC_RMT_SUPPORT_RX_DEMODULATION
    // disable carrier demodulation by default, can reenable by `rmt_apply_carrier()`
    rmt_ll_rx_enable_carrier_demodulation(hal->regs, channel_id, false);
#endif

    // GPIO Matrix/MUX configuration
    rx_channel->base.gpio_num = config->gpio_num;
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        // also enable the input path is `io_loop_back` is on, this is useful for debug
        .mode = GPIO_MODE_INPUT | (config->flags.io_loop_back ? GPIO_MODE_OUTPUT : 0),
        .pull_down_en = false,
        .pull_up_en = true,
        .pin_bit_mask = 1ULL << config->gpio_num,
    };
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_conf), err, TAG, "config GPIO failed");
    esp_rom_gpio_connect_in_signal(config->gpio_num,
                                   rmt_periph_signals.groups[group_id].channels[channel_id + RMT_RX_CHANNEL_OFFSET_IN_GROUP].rx_sig,
                                   config->flags.invert_in);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[config->gpio_num], PIN_FUNC_GPIO);

    // initialize other members of rx channel
    rx_channel->base.direction = RMT_CHANNEL_DIRECTION_RX;
    rx_channel->base.fsm = RMT_FSM_INIT;
    rx_channel->base.hw_mem_base = &RMTMEM.channels[channel_id + RMT_RX_CHANNEL_OFFSET_IN_GROUP].symbols[0];
    rx_channel->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
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
        rmt_rx_destory(rx_channel);
    }
    return ret;
}

static esp_err_t rmt_del_rx_channel(rmt_channel_handle_t channel)
{
    rmt_rx_channel_t *rx_chan = __containerof(channel, rmt_rx_channel_t, base);
    rmt_group_t *group = channel->group;
    int group_id = group->group_id;
    int channel_id = channel->channel_id;
    ESP_LOGD(TAG, "del rx channel(%d,%d)", group_id, channel_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(rmt_rx_destory(rx_chan), TAG, "destory rx channel failed");
    return ESP_OK;
}

esp_err_t rmt_rx_register_event_callbacks(rmt_channel_handle_t channel, const rmt_rx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(channel && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->direction == RMT_CHANNEL_DIRECTION_RX, ESP_ERR_INVALID_ARG, TAG, "invalid channel direction");
    rmt_rx_channel_t *rx_chan = __containerof(channel, rmt_rx_channel_t, base);

#if CONFIG_RMT_ISR_IRAM_SAFE
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
    ESP_RETURN_ON_FALSE(channel && buffer && buffer_size && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(channel->direction == RMT_CHANNEL_DIRECTION_RX, ESP_ERR_INVALID_ARG, TAG, "invalid channel direction");
    ESP_RETURN_ON_FALSE(channel->fsm == RMT_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");
    rmt_rx_channel_t *rx_chan = __containerof(channel, rmt_rx_channel_t, base);

    if (channel->dma_chan) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(buffer), ESP_ERR_INVALID_ARG, TAG, "buffer must locate in internal RAM for DMA use");
    }
    if (channel->dma_chan) {
        ESP_RETURN_ON_FALSE(buffer_size <= rx_chan->num_dma_nodes * RMT_DMA_DESC_BUF_MAX_SIZE,
                            ESP_ERR_INVALID_ARG, TAG, "buffer size exceeds DMA capacity");
    }
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

    // fill in the transaction descriptor
    rmt_rx_trans_desc_t *t = &rx_chan->trans_desc;
    t->buffer = buffer;
    t->buffer_size = buffer_size;
    t->received_symbol_num = 0;
    t->copy_dest_off = 0;

    if (channel->dma_chan) {
#if SOC_RMT_SUPPORT_DMA
        rmt_rx_mount_dma_buffer(rx_chan->dma_nodes, rx_chan->num_dma_nodes, buffer, buffer_size);
        gdma_reset(channel->dma_chan);
        gdma_start(channel->dma_chan, (intptr_t)rx_chan->dma_nodes);
#endif
    }

    rx_chan->mem_off = 0;
    portENTER_CRITICAL(&channel->spinlock);
    // reset memory writer offset
    rmt_ll_rx_reset_pointer(hal->regs, channel_id);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
    // set sampling parameters of incoming signals
    rmt_ll_rx_set_filter_thres(hal->regs, channel_id, ((uint64_t)group->resolution_hz * config->signal_range_min_ns) / 1000000000UL);
    rmt_ll_rx_enable_filter(hal->regs, channel_id, config->signal_range_min_ns != 0);
    rmt_ll_rx_set_idle_thres(hal->regs, channel_id, ((uint64_t)channel->resolution_hz * config->signal_range_max_ns) / 1000000000UL);
    // turn on RMT RX machine
    rmt_ll_rx_enable(hal->regs, channel_id, true);
    portEXIT_CRITICAL(&channel->spinlock);
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
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

    // acquire power manager lock
    if (channel->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(channel->pm_lock), TAG, "acquire pm_lock failed");
    }
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
    channel->fsm = RMT_FSM_ENABLE;
    return ESP_OK;
}

static esp_err_t rmt_rx_disable(rmt_channel_handle_t channel)
{
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

    // release power manager lock
    if (channel->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(channel->pm_lock), TAG, "release pm_lock failed");
    }
    channel->fsm = RMT_FSM_INIT;
    return ESP_OK;
}

static size_t IRAM_ATTR rmt_copy_symbols(rmt_symbol_word_t *symbol_stream, size_t symbol_num, void *buffer, size_t offset, size_t buffer_size)
{
    size_t mem_want = symbol_num * sizeof(rmt_symbol_word_t);
    size_t mem_have = buffer_size - offset;
    size_t copy_size = MIN(mem_want, mem_have);
    // do memory copy
    memcpy(buffer + offset, symbol_stream, copy_size);
    return copy_size;
}

static bool IRAM_ATTR rmt_isr_handle_rx_done(rmt_rx_channel_t *rx_chan)
{
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    rmt_rx_trans_desc_t *trans_desc = &rx_chan->trans_desc;
    bool need_yield = false;

    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_DONE(channel_id));

    portENTER_CRITICAL_ISR(&channel->spinlock);
    // disable the RX engine, it will be enabled again when next time user calls `rmt_receive()`
    rmt_ll_rx_enable(hal->regs, channel_id, false);
    uint32_t offset = rmt_ll_rx_get_memory_writer_offset(hal->regs, channel_id);
    // sanity check
    assert(offset > rx_chan->mem_off);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_SW);
    // copy the symbols to user space
    size_t stream_symbols = offset - rx_chan->mem_off;
    size_t copy_size = rmt_copy_symbols(channel->hw_mem_base + rx_chan->mem_off, stream_symbols,
                                        trans_desc->buffer, trans_desc->copy_dest_off, trans_desc->buffer_size);
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
        ESP_DRAM_LOGE(TAG, "hw buffer too small, received symbols truncated");
    }
#endif // !SOC_RMT_SUPPORT_RX_PINGPONG

    // check whether all symbols are copied
    if (copy_size != stream_symbols * sizeof(rmt_symbol_word_t)) {
        ESP_DRAM_LOGE(TAG, "user buffer too small, received symbols truncated");
    }
    trans_desc->copy_dest_off += copy_size;
    trans_desc->received_symbol_num += copy_size / sizeof(rmt_symbol_word_t);
    // notify the user with receive RMT symbols
    if (rx_chan->on_recv_done) {
        rmt_rx_done_event_data_t edata = {
            .received_symbols = trans_desc->buffer,
            .num_symbols = trans_desc->received_symbol_num,
        };
        if (rx_chan->on_recv_done(channel, &edata, rx_chan->user_data)) {
            need_yield = true;
        }
    }
    return need_yield;
}

#if SOC_RMT_SUPPORT_RX_PINGPONG
static bool IRAM_ATTR rmt_isr_handle_rx_threshold(rmt_rx_channel_t *rx_chan)
{
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    rmt_rx_trans_desc_t *trans_desc = &rx_chan->trans_desc;

    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_THRES(channel_id));

    portENTER_CRITICAL_ISR(&channel->spinlock);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_SW);
    // copy the symbols to user space
    size_t copy_size = rmt_copy_symbols(channel->hw_mem_base + rx_chan->mem_off, rx_chan->ping_pong_symbols,
                                        trans_desc->buffer, trans_desc->copy_dest_off, trans_desc->buffer_size);
    rmt_ll_rx_set_mem_owner(hal->regs, channel_id, RMT_LL_MEM_OWNER_HW);
    portEXIT_CRITICAL_ISR(&channel->spinlock);

    // check whether all symbols are copied
    if (copy_size != rx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t)) {
        ESP_DRAM_LOGE(TAG, "received symbols truncated");
    }
    trans_desc->copy_dest_off += copy_size;
    trans_desc->received_symbol_num += copy_size / sizeof(rmt_symbol_word_t);
    // update the hw memory offset, where stores the next RMT symbols to copy
    rx_chan->mem_off = rx_chan->ping_pong_symbols - rx_chan->mem_off;

    return false;
}
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

static void IRAM_ATTR rmt_rx_default_isr(void *args)
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
static size_t IRAM_ATTR rmt_rx_get_received_symbol_num_from_dma(dma_descriptor_t *desc)
{
    size_t received_bytes = 0;
    while (desc) {
        received_bytes += desc->dw0.length;
        desc = desc->next;
    }
    received_bytes = ALIGN_UP(received_bytes, sizeof(rmt_symbol_word_t));
    return received_bytes / sizeof(rmt_symbol_word_t);
}

static bool IRAM_ATTR rmt_dma_rx_eof_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    rmt_rx_channel_t *rx_chan = (rmt_rx_channel_t *)user_data;
    rmt_channel_t *channel = &rx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    rmt_rx_trans_desc_t *trans_desc = &rx_chan->trans_desc;
    uint32_t channel_id = channel->channel_id;

    portENTER_CRITICAL_ISR(&channel->spinlock);
    // disable the RX engine, it will be enabled again in the next `rmt_receive()`
    rmt_ll_rx_enable(hal->regs, channel_id, false);
    portEXIT_CRITICAL_ISR(&channel->spinlock);

    if (rx_chan->on_recv_done) {
        rmt_rx_done_event_data_t edata = {
            .received_symbols = trans_desc->buffer,
            .num_symbols = rmt_rx_get_received_symbol_num_from_dma(rx_chan->dma_nodes),
        };
        if (rx_chan->on_recv_done(channel, &edata, rx_chan->user_data)) {
            need_yield = true;
        }
    }
    return need_yield;
}
#endif // SOC_RMT_SUPPORT_DMA
