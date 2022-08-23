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
#include "esp_rom_gpio.h"
#include "soc/rmt_periph.h"
#include "soc/rtc.h"
#include "hal/rmt_ll.h"
#include "hal/gpio_hal.h"
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

#if SOC_RMT_SUPPORT_DMA
static bool rmt_dma_tx_eof_cb(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

static esp_err_t rmt_tx_init_dma_link(rmt_tx_channel_t *tx_channel, const rmt_tx_channel_config_t *config)
{
    rmt_symbol_word_t *dma_mem_base = heap_caps_calloc(1, sizeof(rmt_symbol_word_t) * config->mem_block_symbols, RMT_MEM_ALLOC_CAPS | MALLOC_CAP_DMA);
    ESP_RETURN_ON_FALSE(dma_mem_base, ESP_ERR_NO_MEM, TAG, "no mem for tx DMA buffer");
    tx_channel->base.dma_mem_base = dma_mem_base;
    for (int i = 0; i < RMT_DMA_NODES_PING_PONG; i++) {
        // each descriptor shares half of the DMA buffer
        tx_channel->dma_nodes[i].buffer = dma_mem_base + tx_channel->ping_pong_symbols * i;
        tx_channel->dma_nodes[i].dw0.size = tx_channel->ping_pong_symbols * sizeof(rmt_symbol_word_t);
        // the ownership will be switched to DMA in `rmt_tx_do_transaction()`
        tx_channel->dma_nodes[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        // each node can generate the DMA eof interrupt, and the driver will do a ping-pong trick in the eof callback
        tx_channel->dma_nodes[i].dw0.suc_eof = 1;
    }
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_RETURN_ON_ERROR(gdma_new_channel(&dma_chan_config, &tx_channel->base.dma_chan), TAG, "allocate TX DMA channel failed");
    gdma_strategy_config_t gdma_strategy_conf = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    gdma_apply_strategy(tx_channel->base.dma_chan, &gdma_strategy_conf);
    gdma_tx_event_callbacks_t cbs = {
        .on_trans_eof = rmt_dma_tx_eof_cb,
    };
    gdma_register_tx_event_callbacks(tx_channel->base.dma_chan, &cbs, tx_channel);
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
        tx_channel->ping_pong_symbols = config->mem_block_symbols / 2;
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
            group = NULL;
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
    tx_channel->queue_size = config->trans_queue_depth;
    // the queue only saves transaction description pointers
    tx_channel->queues_storage = heap_caps_calloc(config->trans_queue_depth * RMT_TX_QUEUE_MAX, sizeof(rmt_tx_trans_desc_t *), RMT_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(tx_channel->queues_storage, ESP_ERR_NO_MEM, TAG, "no mem for queue storage");
    rmt_tx_trans_desc_t **pp_trans_desc = (rmt_tx_trans_desc_t **)tx_channel->queues_storage;
    for (int i = 0; i < RMT_TX_QUEUE_MAX; i++) {
        tx_channel->trans_queues[i] = xQueueCreateStatic(config->trans_queue_depth, sizeof(rmt_tx_trans_desc_t *),
                                      (uint8_t *)pp_trans_desc, &tx_channel->trans_queue_structs[i]);
        pp_trans_desc += config->trans_queue_depth;
        // sanity check
        assert(tx_channel->trans_queues[i]);
    }
    // initialize the ready queue
    rmt_tx_trans_desc_t *p_trans_desc = NULL;
    for (int i = 0; i < config->trans_queue_depth; i++) {
        p_trans_desc = &tx_channel->trans_desc_pool[i];
        ESP_RETURN_ON_FALSE(xQueueSend(tx_channel->trans_queues[RMT_TX_QUEUE_READY], &p_trans_desc, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
    }
    return ESP_OK;
}

static esp_err_t rmt_tx_destory(rmt_tx_channel_t *tx_channel)
{
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
            vQueueDelete(tx_channel->trans_queues[i]);
        }
    }
    if (tx_channel->queues_storage) {
        free(tx_channel->queues_storage);
    }
    if (tx_channel->base.dma_mem_base) {
        free(tx_channel->base.dma_mem_base);
    }
    if (tx_channel->base.group) {
        // de-register channel from RMT group
        rmt_tx_unregister_from_group(&tx_channel->base, tx_channel->base.group);
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
    ESP_GOTO_ON_FALSE(config && ret_chan && config->resolution_hz && config->trans_queue_depth, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(GPIO_IS_VALID_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, err, TAG, "invalid GPIO number");
    ESP_GOTO_ON_FALSE((config->mem_block_symbols & 0x01) == 0 && config->mem_block_symbols >= SOC_RMT_MEM_WORDS_PER_CHANNEL,
                      ESP_ERR_INVALID_ARG, err, TAG, "mem_block_symbols must be even and at least %d", SOC_RMT_MEM_WORDS_PER_CHANNEL);
#if SOC_RMT_SUPPORT_DMA
    // we only support 2 nodes ping-pong, if the configured memory block size needs more than two DMA descriptors, should treat it as invalid
    ESP_GOTO_ON_FALSE(config->mem_block_symbols <= RMT_DMA_DESC_BUF_MAX_SIZE * RMT_DMA_NODES_PING_PONG, ESP_ERR_INVALID_ARG, err, TAG,
                      "mem_block_symbols can't exceed %d", RMT_DMA_DESC_BUF_MAX_SIZE * RMT_DMA_NODES_PING_PONG);
#else
    ESP_GOTO_ON_FALSE(config->flags.with_dma == 0, ESP_ERR_NOT_SUPPORTED, err, TAG, "DMA not supported");
#endif

    // malloc channel memory
    uint32_t mem_caps = RMT_MEM_ALLOC_CAPS;
    if (config->flags.with_dma) {
        // DMA descriptors must be placed in internal SRAM
        mem_caps |= MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
    }
    tx_channel = heap_caps_calloc(1, sizeof(rmt_tx_channel_t) + sizeof(rmt_tx_trans_desc_t) * config->trans_queue_depth, mem_caps);
    ESP_GOTO_ON_FALSE(tx_channel, ESP_ERR_NO_MEM, err, TAG, "no mem for tx channel");
    // create transaction queues
    ESP_GOTO_ON_ERROR(rmt_tx_create_trans_queue(tx_channel, config), err, TAG, "install trans queues failed");
    // register the channel to group
    ESP_GOTO_ON_ERROR(rmt_tx_register_to_group(tx_channel, config), err, TAG, "register channel failed");
    rmt_group_t *group = tx_channel->base.group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = tx_channel->base.channel_id;
    int group_id = group->group_id;
    // select the clock source
    ESP_GOTO_ON_ERROR(rmt_select_periph_clock(&tx_channel->base, config->clk_src), err, TAG, "set group clock failed");

    // reset channel, make sure the TX engine is not working, and events are cleared
    portENTER_CRITICAL(&group->spinlock);
    rmt_hal_tx_channel_reset(&group->hal, channel_id);
    portEXIT_CRITICAL(&group->spinlock);

    // install interrupt service
    // interrupt is mandatory to run basic RMT transactions, so it's not lazy installed in `rmt_tx_register_event_callbacks()`
    int isr_flags = RMT_INTR_ALLOC_FLAG;
    ret = esp_intr_alloc_intrstatus(rmt_periph_signals.groups[group_id].irq, isr_flags,
                                    (uint32_t)rmt_ll_get_interrupt_status_reg(hal->regs),
                                    RMT_LL_EVENT_TX_MASK(channel_id), rmt_tx_default_isr, tx_channel, &tx_channel->base.intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install tx interrupt failed");
    // install DMA service
#if SOC_RMT_SUPPORT_DMA
    if (config->flags.with_dma) {
        ESP_GOTO_ON_ERROR(rmt_tx_init_dma_link(tx_channel, config), err, TAG, "install tx DMA failed");
    }
#endif
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
    // disable carrier modulation by default, can reenable by `rmt_apply_carrier()`
    rmt_ll_tx_enable_carrier_modulation(hal->regs, channel_id, false);
    // idle level is determined by register value
    rmt_ll_tx_fix_idle_level(hal->regs, channel_id, 0, true);
    // always enable tx wrap, both DMA mode and ping-pong mode rely this feature
    rmt_ll_tx_enable_wrap(hal->regs, channel_id, true);

    // GPIO Matrix/MUX configuration
    tx_channel->base.gpio_num = config->gpio_num;
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        // also enable the input path is `io_loop_back` is on, this is useful for bi-directional buses
        .mode = (config->flags.io_od_mode ? GPIO_MODE_OUTPUT_OD : GPIO_MODE_OUTPUT) | (config->flags.io_loop_back ? GPIO_MODE_INPUT : 0),
        .pull_down_en = false,
        .pull_up_en = true,
        .pin_bit_mask = 1ULL << config->gpio_num,
    };
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_conf), err, TAG, "config GPIO failed");
    esp_rom_gpio_connect_out_signal(config->gpio_num,
                                    rmt_periph_signals.groups[group_id].channels[channel_id + RMT_TX_CHANNEL_OFFSET_IN_GROUP].tx_sig,
                                    config->flags.invert_out, false);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[config->gpio_num], PIN_FUNC_GPIO);

    tx_channel->base.direction = RMT_CHANNEL_DIRECTION_TX;
    tx_channel->base.fsm = RMT_FSM_INIT;
    tx_channel->base.hw_mem_base = &RMTMEM.channels[channel_id + RMT_TX_CHANNEL_OFFSET_IN_GROUP].symbols[0];
    tx_channel->base.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    // polymorphic methods
    tx_channel->base.del = rmt_del_tx_channel;
    tx_channel->base.set_carrier_action = rmt_tx_modulate_carrier;
    tx_channel->base.enable = rmt_tx_enable;
    tx_channel->base.disable = rmt_tx_disable;
    // return general channel handle
    *ret_chan = &tx_channel->base;
    ESP_LOGD(TAG, "new tx channel(%d,%d) at %p, gpio=%d, res=%"PRIu32"Hz, hw_mem_base=%p, dma_mem_base=%p, ping_pong_size=%zu, queue_depth=%zu",
             group_id, channel_id, tx_channel, config->gpio_num, tx_channel->base.resolution_hz,
             tx_channel->base.hw_mem_base, tx_channel->base.dma_mem_base, tx_channel->ping_pong_symbols, tx_channel->queue_size);
    return ESP_OK;

err:
    if (tx_channel) {
        rmt_tx_destory(tx_channel);
    }
    return ret;
}

static esp_err_t rmt_del_tx_channel(rmt_channel_handle_t channel)
{
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_group_t *group = channel->group;
    int group_id = group->group_id;
    int channel_id = channel->channel_id;
    ESP_LOGD(TAG, "del tx channel(%d,%d)", group_id, channel_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(rmt_tx_destory(tx_chan), TAG, "destory tx channel failed");
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
        ESP_GOTO_ON_FALSE(channel->fsm == RMT_FSM_ENABLE, ESP_ERR_INVALID_STATE, err, TAG, "channel should be started before creating sync manager");
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
    ESP_RETURN_ON_FALSE(channel->fsm == RMT_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "channel not in enable state");
#if !SOC_RMT_SUPPORT_TX_LOOP_COUNT
    ESP_RETURN_ON_FALSE(config->loop_count <= 0, ESP_ERR_NOT_SUPPORTED, TAG, "loop count is not supported");
#endif // !SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_tx_trans_desc_t *t = NULL;
    // acquire one transaction description from ready_queue or done_queue
    if (tx_chan->num_trans_inflight < tx_chan->queue_size) {
        ESP_RETURN_ON_FALSE(xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_READY], &t, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "no transaction in the ready queue");
    } else {
        ESP_RETURN_ON_FALSE(xQueueReceive(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &t, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle transaction from done queue failed");
        tx_chan->num_trans_inflight--;
    }
    // sanity check
    assert(t);
    // fill in the transaction descriptor
    memset(t, 0, sizeof(rmt_tx_trans_desc_t));
    t->encoder = encoder;
    t->payload = payload;
    t->payload_bytes = payload_bytes;
    t->loop_count = config->loop_count;
    t->remain_loop_count = t->loop_count;
    t->flags.eot_level = config->flags.eot_level;

    // send the transaction descriptor to queue
    if (xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &t, portMAX_DELAY) == pdTRUE) {
        tx_chan->num_trans_inflight++;
    } else {
        // put the trans descriptor back to ready_queue
        ESP_RETURN_ON_FALSE(xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
    }

    // we don't know which "transmission complete" event will be triggered, but must be one of them: trans_done, loop_done
    // when we run at here, the interrupt status bit for tx_done or loop_end should already up (ensured by `rmt_tx_enable()`)
    // that's why we can go into ISR as soon as we enable the interrupt bit
    // in the ISR, we will fetch the transactions from trans_queue and start it
    portENTER_CRITICAL(&group->spinlock);
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id) | RMT_LL_EVENT_TX_LOOP_END(channel_id), true);
    portEXIT_CRITICAL(&group->spinlock);
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
    rmt_symbol_word_t *mem_to = channel->dma_chan ? channel->dma_mem_base : channel->hw_mem_base;
    rmt_tx_trans_desc_t *cur_trans = tx_chan->cur_trans;
    dma_descriptor_t *desc = NULL;

    // a RMT word whose duration is zero means a "stop" pattern
    mem_to[tx_chan->mem_off++] = (rmt_symbol_word_t) {
        .duration0 = 0,
        .level0 = cur_trans->flags.eot_level,
        .duration1 = 0,
        .level1 = cur_trans->flags.eot_level,
    };

    size_t off = 0;
    if (channel->dma_chan) {
        if (tx_chan->mem_off <= tx_chan->ping_pong_symbols) {
            desc = &tx_chan->dma_nodes[0];
            off = tx_chan->mem_off;
        } else {
            desc = &tx_chan->dma_nodes[1];
            off = tx_chan->mem_off - tx_chan->ping_pong_symbols;
        }
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc->dw0.length = off * sizeof(rmt_symbol_word_t);
        // break down the DMA descriptor link
        desc->next = NULL;
    } else {
        portENTER_CRITICAL_ISR(&group->spinlock);
        // This is the end of a sequence of encoding sessions, disable the threshold interrupt as no more data will be put into RMT memory block
        rmt_ll_enable_interrupt(group->hal.regs, RMT_LL_EVENT_TX_THRES(channel_id), false);
        portEXIT_CRITICAL_ISR(&group->spinlock);
    }
}

static size_t IRAM_ATTR rmt_encode_check_result(rmt_tx_channel_t *tx_chan, rmt_tx_trans_desc_t *t)
{
    rmt_encode_state_t encode_state = 0;
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

#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        gdma_reset(channel->dma_chan);
        // chain the descritpros into a ring, and will break it in `rmt_encode_eof()`
        for (int i = 0; i < RMT_DMA_NODES_PING_PONG; i++) {
            tx_chan->dma_nodes[i].next = &tx_chan->dma_nodes[i + 1];
            tx_chan->dma_nodes[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        }
        tx_chan->dma_nodes[1].next = &tx_chan->dma_nodes[0];
    }
#endif // SOC_RMT_SUPPORT_DMA

    // set transaction specific parameters
    portENTER_CRITICAL_ISR(&channel->spinlock);
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
    portEXIT_CRITICAL_ISR(&channel->spinlock);

    // enable/disable specific interrupts
    portENTER_CRITICAL_ISR(&group->spinlock);
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
    portEXIT_CRITICAL_ISR(&group->spinlock);

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
        gdma_start(channel->dma_chan, (intptr_t)tx_chan->dma_nodes);
        // delay a while, wait for DMA data going to RMT memory block
        esp_rom_delay_us(1);
    }
#endif
    // turn on the TX machine
    portENTER_CRITICAL_ISR(&channel->spinlock);
    rmt_ll_tx_fix_idle_level(hal->regs, channel_id, t->flags.eot_level, true);
    rmt_ll_tx_start(hal->regs, channel_id);
    portEXIT_CRITICAL_ISR(&channel->spinlock);
}

static esp_err_t rmt_tx_enable(rmt_channel_handle_t channel)
{
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;
    // acquire power manager lock
    if (channel->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(channel->pm_lock), TAG, "acquire pm_lock failed");
    }

    portENTER_CRITICAL(&channel->spinlock);
    rmt_ll_tx_reset_pointer(hal->regs, channel_id);
    rmt_ll_tx_enable_loop(hal->regs, channel_id, false);
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    rmt_ll_tx_reset_loop_count(hal->regs, channel_id);
    rmt_ll_tx_enable_loop_count(hal->regs, channel_id, false);
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT
    // trigger a quick trans done event by sending a EOF symbol, no signal should appear on the GPIO
    tx_chan->cur_trans = NULL;
    channel->hw_mem_base[0].val = 0;
    rmt_ll_tx_start(hal->regs, channel_id);
    portEXIT_CRITICAL(&channel->spinlock);

    // wait the RMT interrupt line goes active, we won't go into the ISR handler until we enable the `RMT_LL_EVENT_TX_DONE` interrupt
    while (!(rmt_ll_tx_get_interrupt_status_raw(hal->regs, channel_id) & RMT_LL_EVENT_TX_DONE(channel_id))) {}
#if SOC_RMT_SUPPORT_DMA
    if (channel->dma_chan) {
        // enable the DMA access mode
        portENTER_CRITICAL(&channel->spinlock);
        rmt_ll_tx_enable_dma(hal->regs, channel_id, true);
        portEXIT_CRITICAL(&channel->spinlock);

        gdma_connect(channel->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_RMT, 0));
    }
#endif // SOC_RMT_SUPPORT_DMA

    channel->fsm = RMT_FSM_ENABLE;

    // enable channel interrupt, dispatch transactions in ISR (in case there're transaction descriptors in the queue, then we should start them)
    portENTER_CRITICAL(&group->spinlock);
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id), true);
    portEXIT_CRITICAL(&group->spinlock);
    return ESP_OK;
}

static esp_err_t rmt_tx_disable(rmt_channel_handle_t channel)
{
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    int channel_id = channel->channel_id;

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
        xQueueSend(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &tx_chan->cur_trans, portMAX_DELAY);
        // reset corresponding encoder
        rmt_encoder_reset(tx_chan->cur_trans->encoder);
    }
    tx_chan->cur_trans = NULL;

    // release power manager lock
    if (channel->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(channel->pm_lock), TAG, "release pm_lock failed");
    }

    channel->fsm = RMT_FSM_INIT;
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
    rmt_channel_t *channel = &tx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;

    // continue pingpong transmission
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

    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_THRES(channel_id));

    return false;
}

static bool IRAM_ATTR rmt_isr_handle_tx_done(rmt_tx_channel_t *tx_chan)
{
    rmt_channel_t *channel = &tx_chan->base;
    rmt_group_t *group = channel->group;
    rmt_hal_context_t *hal = &group->hal;
    uint32_t channel_id = channel->channel_id;
    BaseType_t awoken = pdFALSE;
    rmt_tx_trans_desc_t *trans_desc = NULL;
    bool need_yield = false;

    portENTER_CRITICAL_ISR(&group->spinlock);
    // disable interrupt temporarily, re-enable it when there is transaction unhandled in the queue
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id), false);
    portEXIT_CRITICAL_ISR(&group->spinlock);

    trans_desc = tx_chan->cur_trans;
    // process finished transaction
    if (trans_desc) {
        // don't care of the tx done event for any undergoing loop transaction
        // mostly it's triggered when a loop transmission is undergoing and user calls `rmt_transmit()` where tx done interrupt is generated by accident
        if (trans_desc->loop_count != 0) {
            rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id));
            return need_yield;
        }
        if (tx_chan->on_trans_done) {
            rmt_tx_done_event_data_t edata = {
                .num_symbols = trans_desc->transmitted_symbol_num,
            };
            if (tx_chan->on_trans_done(channel, &edata, tx_chan->user_data)) {
                need_yield = true;
            }
        }
        // move transaction to done_queue
        xQueueSendFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &trans_desc, &awoken);
        if (awoken == pdTRUE) {
            need_yield = true;
        }
    }
    // fetch new transaction description from trans_queue
    if (xQueueReceiveFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &trans_desc, &awoken) == pdTRUE) {
        // sanity check
        assert(trans_desc);
        // update current transaction
        tx_chan->cur_trans = trans_desc;

        portENTER_CRITICAL_ISR(&group->spinlock);
        // only clear the trans done status when we're sure there still remains transaction to handle
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id));
        // enable interrupt again, because the new transaction can trigger another trans done event
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id), trans_desc->loop_count == 0);
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel_id), trans_desc->loop_count > 0);
        portEXIT_CRITICAL_ISR(&group->spinlock);

        // begin a new transaction
        rmt_tx_do_transaction(tx_chan, trans_desc);
    } else { // No transactions left in the queue
        // don't clear interrupt status, so when next time user push new transaction to the queue and call esp_intr_enable,
        // we can go to this ISR handler again
        tx_chan->cur_trans = NULL;
    }
    if (awoken == pdTRUE) {
        need_yield = true;
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
        // This is a workaround for chips that don't support auto stop
        // Although we stop the transaction immediately in ISR handler, it's still possible that some rmt symbols have sneaked out
        rmt_ll_tx_stop(hal->regs, channel_id);
        portEXIT_CRITICAL_ISR(&channel->spinlock);
#endif // SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP
        // continue unfinished loop transaction
        if (trans_desc->remain_loop_count) {
            uint32_t this_loop_count = MIN(trans_desc->remain_loop_count, RMT_LL_MAX_LOOP_COUNT_PER_BATCH);
            trans_desc->remain_loop_count -= this_loop_count;
            rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel_id));
            portENTER_CRITICAL_ISR(&channel->spinlock);
            rmt_ll_tx_set_loop_count(hal->regs, channel_id, this_loop_count);
            rmt_ll_tx_reset_pointer(hal->regs, channel_id);
            // continue the loop transmission, don't need to fill the RMT symbols again, just restart the engine
            rmt_ll_tx_start(hal->regs, channel_id);
            portEXIT_CRITICAL_ISR(&channel->spinlock);
            return need_yield;
        } else {
            if (tx_chan->on_trans_done) {
                rmt_tx_done_event_data_t edata = {
                    .num_symbols = trans_desc->transmitted_symbol_num,
                };
                if (tx_chan->on_trans_done(channel, &edata, tx_chan->user_data)) {
                    need_yield = true;
                }
            }
            // move transaction to done_queue
            xQueueSendFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_COMPLETE], &trans_desc, &awoken);
            if (awoken == pdTRUE) {
                need_yield = true;
            }
        }
    }
    // trans_done and loop_done should be considered as one "transmission complete"
    // but sometimes the trans done event might also be triggered together with loop done event, by accident, so clear it first
    rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id));
    portENTER_CRITICAL_ISR(&group->spinlock);
    // disable interrupt temporarily, re-enable it when there is transaction unhandled in the queue
    rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel_id), false);
    portEXIT_CRITICAL_ISR(&group->spinlock);

    // fetch new transaction description from trans_queue
    if (xQueueReceiveFromISR(tx_chan->trans_queues[RMT_TX_QUEUE_PROGRESS], &trans_desc, &awoken) == pdTRUE) {
        // sanity check
        assert(trans_desc);
        tx_chan->cur_trans = trans_desc;
        // clear the loop end status when we're sure there still remains transaction to handle
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel_id));

        portENTER_CRITICAL_ISR(&group->spinlock);
        // enable interrupt again, because the new transaction can trigger new trans done event
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_DONE(channel_id), trans_desc->loop_count == 0);
        rmt_ll_enable_interrupt(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel_id), trans_desc->loop_count > 0);
        portEXIT_CRITICAL_ISR(&group->spinlock);

        // begin a new transaction
        rmt_tx_do_transaction(tx_chan, trans_desc);
    } else { // No transactions left in the queue
        // don't clear interrupt status, so when next time user push new transaction to the queue and call esp_intr_enable,
        // we can go into ISR handler again
        tx_chan->cur_trans = NULL;
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
    dma_descriptor_t *eof_desc = (dma_descriptor_t *)event_data->tx_eof_desc_addr;
    // if the DMA descriptor link is still a ring (i.e. hasn't broken down by `rmt_tx_mark_eof()`), then we treat it as a valid ping-pong event
    if (eof_desc->next && eof_desc->next->next) {
        // continue pingpong transmission
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
    }
    return false;
}
#endif // SOC_RMT_SUPPORT_DMA
