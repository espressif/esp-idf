/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "esp_intr_alloc.h"
#if CONFIG_UHCI_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/uhci.h"
#include "driver/uhci_types.h"
#include "hal/uhci_periph.h"
#include "soc/soc_caps.h"
#include "hal/uhci_hal.h"
#include "hal/uhci_ll.h"
#include "hal/dma_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/gdma.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/esp_psram_mspi.h"
#include "uhci_private.h"
#include "esp_memory_utils.h"
#include "esp_cache.h"

static const char* TAG = "uhci";

typedef struct uhci_platform_t {
    _lock_t mutex;                                      // platform level mutex lock.
    uhci_controller_handle_t controller[SOC_UHCI_NUM];  // array of UHCI instances.
} uhci_platform_t;

static uhci_platform_t s_uhci_platform = {}; // singleton platform

static bool uhci_ctrl_occupied(int port_num)
{
    return s_uhci_platform.controller[port_num] != NULL;
}

static void uhci_do_transmit(uhci_controller_handle_t uhci_ctrl, uhci_transaction_desc_t *trans);

static bool uhci_gdma_tx_callback_eof(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    BaseType_t do_yield = pdFALSE;
    uhci_controller_handle_t uhci_ctrl = (uhci_controller_handle_t) user_data;
    uhci_transaction_desc_t *trans_desc = NULL;
    bool need_yield = false;

    uhci_tx_fsm_t expected_fsm = UHCI_TX_FSM_RUN;
    if (atomic_compare_exchange_strong(&uhci_ctrl->tx_dir.tx_fsm, &expected_fsm, UHCI_TX_FSM_ENABLE_WAIT)) {
        trans_desc = uhci_ctrl->tx_dir.cur_trans;
        xQueueSendFromISR(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_COMPLETE], &trans_desc, &do_yield);
        if (do_yield) {
            need_yield = true;
        }
        atomic_store(&uhci_ctrl->tx_dir.tx_fsm, UHCI_TX_FSM_ENABLE);
    }

#if CONFIG_PM_ENABLE
    if (uhci_ctrl->pm_lock) {
        esp_pm_lock_release(uhci_ctrl->pm_lock);
    }
#endif

    if (uhci_ctrl->tx_dir.on_tx_trans_done) {
        uhci_tx_done_event_data_t evt_data = {
            .buffer = uhci_ctrl->tx_dir.cur_trans->buffer,
            .sent_size = uhci_ctrl->tx_dir.cur_trans->buffer_size,
        };
        if (uhci_ctrl->tx_dir.on_tx_trans_done(uhci_ctrl, &evt_data, uhci_ctrl->user_data)) {
            need_yield |= true;
        }
    }

    expected_fsm = UHCI_TX_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&uhci_ctrl->tx_dir.tx_fsm, &expected_fsm, UHCI_TX_FSM_RUN_WAIT)) {
        if (xQueueReceiveFromISR(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_PROGRESS], &trans_desc, &do_yield) == pdTRUE) {
            // sanity check
            assert(trans_desc);
            atomic_store(&uhci_ctrl->tx_dir.tx_fsm, UHCI_TX_FSM_RUN);
            uhci_do_transmit(uhci_ctrl, trans_desc);
            if (do_yield) {
                need_yield |= true;
            }
        } else {
            atomic_store(&uhci_ctrl->tx_dir.tx_fsm, UHCI_TX_FSM_ENABLE);
        }
    }

    return need_yield;
}

static bool uhci_gdma_rx_callback_done(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    bool need_yield = false;
    uhci_controller_handle_t uhci_ctrl = (uhci_controller_handle_t) user_data;
    bool is_buf_from_psram = esp_ptr_external_ram(uhci_ctrl->rx_dir.buffer_pointers[uhci_ctrl->rx_dir.node_index]);
    // If the data is not all received, handle it in not normal_eof block. Otherwise, in eof block.
    if (!event_data->flags.normal_eof) {
        size_t rx_size = uhci_ctrl->rx_dir.buffer_size_per_desc_node[uhci_ctrl->rx_dir.node_index];
        uhci_rx_event_data_t evt_data = {
            .data = uhci_ctrl->rx_dir.buffer_pointers[uhci_ctrl->rx_dir.node_index],
            .recv_size = rx_size,
            .flags.totally_received = false,
        };

        if (is_buf_from_psram) {
            esp_psram_mspi_mb();
        }
        if (uhci_ctrl->rx_dir.on_rx_trans_event) {
            need_yield |= uhci_ctrl->rx_dir.on_rx_trans_event(uhci_ctrl, &evt_data, uhci_ctrl->user_data);
        }
        uhci_ctrl->rx_dir.node_index++;
        // Go back to 0 as its a circle descriptor link
        if (uhci_ctrl->rx_dir.node_index >= uhci_ctrl->rx_dir.rx_num_dma_nodes) {
            uhci_ctrl->rx_dir.node_index = 0;
        }

    } else {
        // eof event
        size_t rx_size = gdma_link_count_buffer_size_till_eof(uhci_ctrl->rx_dir.dma_link, uhci_ctrl->rx_dir.node_index);
        uhci_rx_event_data_t evt_data = {
            .data = uhci_ctrl->rx_dir.buffer_pointers[uhci_ctrl->rx_dir.node_index],
            .recv_size = rx_size,
            .flags.totally_received = true,
        };

        if (is_buf_from_psram) {
            esp_psram_mspi_mb();
        }
#if CONFIG_PM_ENABLE
        // release power manager lock
        if (uhci_ctrl->pm_lock) {
            esp_pm_lock_release(uhci_ctrl->pm_lock);
        }
#endif
        if (uhci_ctrl->rx_dir.on_rx_trans_event) {
            need_yield |= uhci_ctrl->rx_dir.on_rx_trans_event(uhci_ctrl, &evt_data, uhci_ctrl->user_data);
        }

        // Stop the transaction when EOF is detected. In case for length EOF, there is no further more callback to be invoked.
        gdma_stop(uhci_ctrl->rx_dir.dma_chan);
        gdma_reset(uhci_ctrl->rx_dir.dma_chan);

        uhci_ctrl->rx_dir.rx_fsm = UHCI_RX_FSM_ENABLE;
    }

    if (event_data->flags.abnormal_eof) {
        esp_rom_printf(DRAM_STR("An abnormal eof on uhci detected\n"));
    }

    return need_yield;
}

static esp_err_t uhci_gdma_initialize(uhci_controller_handle_t uhci_ctrl, const uhci_controller_config_t *config)
{
    // Initialize DMA TX channel
    gdma_channel_alloc_config_t tx_alloc_config = {
#if CONFIG_UHCI_ISR_CACHE_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&tx_alloc_config, &uhci_ctrl->tx_dir.dma_chan, NULL), TAG, "DMA tx channel alloc failed");
    gdma_connect(uhci_ctrl->tx_dir.dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));

    gdma_transfer_config_t transfer_cfg = {
        .access_ext_mem = true,
        .max_data_burst_size = config->dma_burst_size,
    };
    ESP_RETURN_ON_ERROR(gdma_config_transfer(uhci_ctrl->tx_dir.dma_chan, &transfer_cfg), TAG, "Config DMA tx channel transfer failed");

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true,
        .eof_till_data_popped = true,
    };
    gdma_apply_strategy(uhci_ctrl->tx_dir.dma_chan, &strategy_config);

    // create DMA link list
    gdma_get_alignment_constraints(uhci_ctrl->tx_dir.dma_chan, &uhci_ctrl->tx_dir.int_mem_align, &uhci_ctrl->tx_dir.ext_mem_align);
    size_t buffer_alignment = UHCI_MAX(uhci_ctrl->tx_dir.int_mem_align, uhci_ctrl->tx_dir.ext_mem_align);
    size_t num_dma_nodes = esp_dma_calculate_node_count(config->max_transmit_size, buffer_alignment, DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
    gdma_link_list_config_t dma_link_config = {
        .item_alignment = 4,
        .num_items = num_dma_nodes,
    };
    ESP_RETURN_ON_ERROR(gdma_new_link_list(&dma_link_config, &uhci_ctrl->tx_dir.dma_link), TAG, "DMA tx link list alloc failed");
    ESP_LOGD(TAG, "tx_dma node number is %d", num_dma_nodes);

    // Initialize DMA RX channel
    gdma_channel_alloc_config_t rx_alloc_config = {
#if CONFIG_UHCI_ISR_CACHE_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&rx_alloc_config, NULL, &uhci_ctrl->rx_dir.dma_chan), TAG, "DMA rx channel alloc failed");
    gdma_connect(uhci_ctrl->rx_dir.dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));
    ESP_RETURN_ON_ERROR(gdma_config_transfer(uhci_ctrl->rx_dir.dma_chan, &transfer_cfg), TAG, "Config DMA rx channel transfer failed");

    gdma_get_alignment_constraints(uhci_ctrl->rx_dir.dma_chan, &uhci_ctrl->rx_dir.int_mem_align, &uhci_ctrl->rx_dir.ext_mem_align);
    buffer_alignment = UHCI_MAX(uhci_ctrl->rx_dir.int_mem_align, uhci_ctrl->rx_dir.ext_mem_align);
    uhci_ctrl->rx_dir.rx_num_dma_nodes = esp_dma_calculate_node_count(config->max_receive_internal_mem, buffer_alignment, DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
    dma_link_config.num_items = uhci_ctrl->rx_dir.rx_num_dma_nodes;
    ESP_RETURN_ON_ERROR(gdma_new_link_list(&dma_link_config, &uhci_ctrl->rx_dir.dma_link), TAG, "DMA rx link list alloc failed");
    ESP_LOGD(TAG, "rx_dma node number is %d", uhci_ctrl->rx_dir.rx_num_dma_nodes);

    uhci_ctrl->rx_dir.buffer_size_per_desc_node = heap_caps_calloc(uhci_ctrl->rx_dir.rx_num_dma_nodes, sizeof(*uhci_ctrl->rx_dir.buffer_size_per_desc_node), UHCI_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(uhci_ctrl->rx_dir.buffer_size_per_desc_node, ESP_ERR_NO_MEM, TAG, "no memory for recording buffer size for desc node");
    uhci_ctrl->rx_dir.buffer_pointers = heap_caps_calloc(uhci_ctrl->rx_dir.rx_num_dma_nodes, sizeof(*uhci_ctrl->rx_dir.buffer_pointers), UHCI_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(uhci_ctrl->rx_dir.buffer_pointers, ESP_ERR_NO_MEM, TAG, "no memory for recording buffer pointers for desc node");

    // Register callbacks
    gdma_tx_event_callbacks_t tx_cbk = {
        .on_trans_eof = uhci_gdma_tx_callback_eof,
    };
    ESP_RETURN_ON_ERROR(gdma_register_tx_event_callbacks(uhci_ctrl->tx_dir.dma_chan, &tx_cbk, uhci_ctrl), TAG, "register DMA callbacks failed");

    gdma_rx_event_callbacks_t rx_cbk = {
        .on_recv_done = uhci_gdma_rx_callback_done,
    };
    ESP_RETURN_ON_ERROR(gdma_register_rx_event_callbacks(uhci_ctrl->rx_dir.dma_chan, &rx_cbk, uhci_ctrl), TAG, "register DMA callbacks failed");

    return ESP_OK;
}

static esp_err_t uhci_gdma_deinitialize(uhci_controller_handle_t uhci_ctrl)
{
    if (uhci_ctrl->tx_dir.dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_disconnect(uhci_ctrl->tx_dir.dma_chan), TAG, "disconnect tx dma channel failed");
        ESP_RETURN_ON_ERROR(gdma_del_channel(uhci_ctrl->tx_dir.dma_chan), TAG, "delete tx dma channel failed");
    }

    if (uhci_ctrl->rx_dir.dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_disconnect(uhci_ctrl->rx_dir.dma_chan), TAG, "disconnect rx dma channel failed");
        ESP_RETURN_ON_ERROR(gdma_del_channel(uhci_ctrl->rx_dir.dma_chan), TAG, "delete rx dma channel failed");
    }
    if (uhci_ctrl->tx_dir.dma_link) {
        ESP_RETURN_ON_ERROR(gdma_del_link_list(uhci_ctrl->tx_dir.dma_link), TAG, "dma delete link list failed");
    }
    if (uhci_ctrl->rx_dir.dma_link) {
        ESP_RETURN_ON_ERROR(gdma_del_link_list(uhci_ctrl->rx_dir.dma_link), TAG, "dma delete link list failed");
    }

    return ESP_OK;
}

static void uhci_do_transmit(uhci_controller_handle_t uhci_ctrl, uhci_transaction_desc_t *trans)
{
    uhci_ctrl->tx_dir.cur_trans = trans;
    size_t buffer_alignment = esp_ptr_internal(trans->buffer) ? uhci_ctrl->tx_dir.int_mem_align : uhci_ctrl->tx_dir.ext_mem_align;
    gdma_buffer_mount_config_t mount_config = {
        .buffer = trans->buffer,
        .buffer_alignment = buffer_alignment,
        .length = trans->buffer_size,
        .flags = {
            .mark_eof = true,
            .mark_final = GDMA_FINAL_LINK_TO_NULL,
        }
    };

#if CONFIG_PM_ENABLE
    // acquire power manager lock
    if (uhci_ctrl->pm_lock) {
        esp_pm_lock_acquire(uhci_ctrl->pm_lock);
    }
#endif

    gdma_link_mount_buffers(uhci_ctrl->tx_dir.dma_link, 0, &mount_config, 1, NULL);
    gdma_start(uhci_ctrl->tx_dir.dma_chan, gdma_link_get_head_addr(uhci_ctrl->tx_dir.dma_link));
}

esp_err_t uhci_receive(uhci_controller_handle_t uhci_ctrl, uint8_t *read_buffer, size_t buffer_size)
{
    ESP_RETURN_ON_FALSE(uhci_ctrl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((read_buffer != NULL), ESP_ERR_INVALID_ARG, TAG, "read buffer null");

    uint32_t mem_cache_line_size = esp_ptr_external_ram(read_buffer) ? uhci_ctrl->ext_mem_cache_line_size : uhci_ctrl->int_mem_cache_line_size;

    // Must take cache line into consideration for C2M operation.
    uint32_t max_alignment_needed = UHCI_MAX(UHCI_MAX(uhci_ctrl->rx_dir.int_mem_align, uhci_ctrl->rx_dir.ext_mem_align), mem_cache_line_size);

    // Align the read_buffer pointer to mem_cache_line_size
    if (max_alignment_needed > 0 && (((uintptr_t)read_buffer) & (max_alignment_needed - 1)) != 0) {
        uintptr_t aligned_address = ((uintptr_t)read_buffer + max_alignment_needed - 1) & ~(max_alignment_needed - 1);
        size_t offset = aligned_address - (uintptr_t)read_buffer;

        ESP_RETURN_ON_FALSE(buffer_size > offset, ESP_ERR_INVALID_ARG, TAG, "buffer size too small to align");

        read_buffer = (uint8_t *)aligned_address;
        buffer_size -= offset;
    }

    uhci_ctrl->rx_dir.cache_line = mem_cache_line_size;
    uhci_rx_fsm_t expected_fsm = UHCI_RX_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&uhci_ctrl->rx_dir.rx_fsm, &expected_fsm, UHCI_RX_FSM_RUN_WAIT), ESP_ERR_INVALID_STATE, TAG, "controller not in enable state");

    size_t node_count = uhci_ctrl->rx_dir.rx_num_dma_nodes;

    // Initialize the mount configurations for each DMA node, making sure every no
    size_t usable_size = (max_alignment_needed == 0) ? buffer_size : (buffer_size / max_alignment_needed) * max_alignment_needed;
    size_t base_size = (max_alignment_needed == 0) ? usable_size / node_count : (usable_size / node_count / max_alignment_needed) * max_alignment_needed;
    size_t remaining_size = usable_size - (base_size * node_count);

    gdma_buffer_mount_config_t mount_configs[node_count];
    memset(mount_configs, 0, node_count);

    for (size_t i = 0; i < node_count; i++) {
        uhci_ctrl->rx_dir.buffer_size_per_desc_node[i] = base_size;
        uhci_ctrl->rx_dir.buffer_pointers[i] = read_buffer;
        size_t buffer_alignment = esp_ptr_internal(read_buffer) ? uhci_ctrl->rx_dir.int_mem_align : uhci_ctrl->rx_dir.ext_mem_align;
        // Distribute the remaining size to the first few nodes
        if (remaining_size >= max_alignment_needed) {
            uhci_ctrl->rx_dir.buffer_size_per_desc_node[i] += max_alignment_needed;
            remaining_size -= max_alignment_needed;
        }

        mount_configs[i] = (gdma_buffer_mount_config_t) {
            .buffer = read_buffer,
            .buffer_alignment = buffer_alignment,
            .length = uhci_ctrl->rx_dir.buffer_size_per_desc_node[i],
            .flags = {
                .mark_final = GDMA_FINAL_LINK_TO_DEFAULT,
            }
        };
        ESP_LOGD(TAG, "The DMA node %d has %d byte", i, uhci_ctrl->rx_dir.buffer_size_per_desc_node[i]);
        ESP_RETURN_ON_FALSE(uhci_ctrl->rx_dir.buffer_size_per_desc_node[i] != 0, ESP_ERR_INVALID_STATE, TAG, "Allocate dma node length is 0, please reconfigure the buffer_size");
        read_buffer += uhci_ctrl->rx_dir.buffer_size_per_desc_node[i];
    }

#if CONFIG_PM_ENABLE
    // acquire power manager lock
    if (uhci_ctrl->pm_lock) {
        esp_pm_lock_acquire(uhci_ctrl->pm_lock);
    }
#endif

    gdma_link_mount_buffers(uhci_ctrl->rx_dir.dma_link, 0, mount_configs, node_count, NULL);

    // Invalidate cache before DMA starts to ensure no dirty cache lines.
    // All DMA nodes (mount_configs) share the same contiguous user buffer, so checking mount_configs[0].buffer is sufficient.
    bool need_cache_sync = esp_ptr_internal(mount_configs[0].buffer) ? (uhci_ctrl->int_mem_cache_line_size > 0) : (uhci_ctrl->ext_mem_cache_line_size > 0);
    if (need_cache_sync) {
        ESP_RETURN_ON_ERROR(esp_cache_msync(mount_configs[0].buffer, usable_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C), TAG, "cache sync failed");
    }

    gdma_reset(uhci_ctrl->rx_dir.dma_chan);
    gdma_start(uhci_ctrl->rx_dir.dma_chan, gdma_link_get_head_addr(uhci_ctrl->rx_dir.dma_link));

    atomic_store(&uhci_ctrl->rx_dir.rx_fsm, UHCI_RX_FSM_RUN);
    return ESP_OK;
}

esp_err_t uhci_transmit(uhci_controller_handle_t uhci_ctrl, uint8_t *write_buffer, size_t write_size)
{
    ESP_RETURN_ON_FALSE(uhci_ctrl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((write_buffer != NULL), ESP_ERR_INVALID_ARG, TAG, "write buffer null");

    size_t alignment = 0;
    size_t cache_line_size = 0;
    esp_ptr_external_ram(write_buffer) ? (alignment = uhci_ctrl->tx_dir.ext_mem_align, cache_line_size = uhci_ctrl->ext_mem_cache_line_size) : (alignment = uhci_ctrl->tx_dir.int_mem_align, cache_line_size = uhci_ctrl->int_mem_cache_line_size);

    ESP_RETURN_ON_FALSE(((((uintptr_t)write_buffer) & (alignment - 1)) == 0) && (((write_size) & (alignment - 1)) == 0), ESP_ERR_INVALID_ARG,
                        TAG, "buffer address or size are not %d bytes aligned", alignment);

    if (cache_line_size > 0) {
        // Write back to cache to synchronize the cache before DMA start
        ESP_RETURN_ON_ERROR(esp_cache_msync((void *)write_buffer, write_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED), TAG, "cache sync failed");
    }

    uhci_transaction_desc_t *t = NULL;

    if (xQueueReceive(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_READY], &t, 0) != pdTRUE) {
        if (xQueueReceive(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_COMPLETE], &t, 0) == pdTRUE) {
            atomic_fetch_sub(&uhci_ctrl->tx_dir.num_trans_inflight, 1);
        }
    }
    ESP_RETURN_ON_FALSE(t, ESP_ERR_INVALID_STATE, TAG, "no free transaction descriptor, please consider increasing trans_queue_depth");

    memset(t, 0, sizeof(uhci_transaction_desc_t));
    t->buffer = write_buffer;
    t->buffer_size = write_size;

    ESP_RETURN_ON_FALSE(xQueueSend(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_PROGRESS], &t, 0) == pdTRUE, ESP_ERR_NO_MEM, TAG, "uhci tx transaction queue full");
    atomic_fetch_add(&uhci_ctrl->tx_dir.num_trans_inflight, 1);

    uhci_tx_fsm_t expected_fsm = UHCI_TX_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&uhci_ctrl->tx_dir.tx_fsm, &expected_fsm, UHCI_TX_FSM_RUN_WAIT)) {
        if (xQueueReceive(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_PROGRESS], &t, 0)) {
            atomic_store(&uhci_ctrl->tx_dir.tx_fsm, UHCI_TX_FSM_RUN);
            uhci_do_transmit(uhci_ctrl, t);
        } else {
            atomic_store(&uhci_ctrl->tx_dir.tx_fsm, UHCI_TX_FSM_ENABLE);
        }
    }

    return ESP_OK;
}

esp_err_t uhci_del_controller(uhci_controller_handle_t uhci_ctrl)
{
    ESP_RETURN_ON_FALSE(uhci_ctrl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    if (uhci_ctrl->rx_dir.rx_fsm != UHCI_RX_FSM_ENABLE) {
        ESP_LOGE(TAG, "RX transaction is not finished, delete controller failed");
        return ESP_ERR_INVALID_STATE;
    }

    if (uhci_ctrl->tx_dir.tx_fsm != UHCI_TX_FSM_ENABLE) {
        ESP_LOGE(TAG, "TX transaction is not finished, delete controller failed");
        return ESP_ERR_INVALID_STATE;
    }

    UHCI_RCC_ATOMIC() {
        uhci_ll_enable_bus_clock(uhci_ctrl->uhci_num, false);
    }

    for (int i = 0; i < UHCI_TRANS_QUEUE_MAX; i++) {
        if (uhci_ctrl->tx_dir.trans_queues[i]) {
            vQueueDeleteWithCaps(uhci_ctrl->tx_dir.trans_queues[i]);
        }
    }

    if (uhci_ctrl->tx_dir.trans_desc_pool) {
        heap_caps_free(uhci_ctrl->tx_dir.trans_desc_pool);
    }

    if (uhci_ctrl->rx_dir.buffer_size_per_desc_node) {
        free(uhci_ctrl->rx_dir.buffer_size_per_desc_node);
    }
    if (uhci_ctrl->rx_dir.buffer_pointers) {
        free(uhci_ctrl->rx_dir.buffer_pointers);
    }

#if CONFIG_PM_ENABLE
    if (uhci_ctrl->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(uhci_ctrl->pm_lock), TAG, "delete rx pm_lock failed");
    }
#endif

    ESP_RETURN_ON_ERROR(uhci_gdma_deinitialize(uhci_ctrl), TAG, "deinitialize uhci dam channel failed");

    uhci_hal_deinit(&uhci_ctrl->hal);

    s_uhci_platform.controller[uhci_ctrl->uhci_num] = NULL;

    heap_caps_free(uhci_ctrl);

    return ESP_OK;
}

esp_err_t uhci_new_controller(const uhci_controller_config_t *config, uhci_controller_handle_t *ret_uhci_ctrl)
{
    ESP_RETURN_ON_FALSE((config && ret_uhci_ctrl), ESP_ERR_INVALID_ARG, TAG, "UHCI invalid argument");

    esp_err_t ret = ESP_OK;

    uhci_controller_handle_t uhci_ctrl = (uhci_controller_handle_t)heap_caps_calloc(1, sizeof(uhci_controller_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(uhci_ctrl, ESP_ERR_NO_MEM, TAG, "no mem for uhci controller handle");
    if (config->rx_eof_flags.length_eof) {
        ESP_RETURN_ON_FALSE(config->max_packet_receive < UHCI_LL_MAX_RECEIVE_PACKET_THRESHOLD, ESP_ERR_INVALID_ARG, TAG, "max receive packet is over threshold");
    }

    atomic_init(&uhci_ctrl->tx_dir.tx_fsm, UHCI_TX_FSM_ENABLE);
    atomic_init(&uhci_ctrl->rx_dir.rx_fsm, UHCI_TX_FSM_ENABLE);

    // Auto search a free controller
    bool ctrl_found = false;
    _lock_acquire(&s_uhci_platform.mutex);
    for (int i = 0; i < SOC_UHCI_NUM; i++) {
        if (uhci_ctrl_occupied(i) == false) {
            s_uhci_platform.controller[i] = uhci_ctrl;
            ctrl_found = true;
            uhci_ctrl->uhci_num = i;
            break;
        }
    }
    _lock_release(&s_uhci_platform.mutex);
    ESP_GOTO_ON_FALSE(ctrl_found, ESP_ERR_NOT_FOUND, err, TAG, "no free controller");

    for (int i = 0; i < UHCI_TRANS_QUEUE_MAX; i++) {
        uhci_ctrl->tx_dir.trans_queues[i] = xQueueCreateWithCaps(config->tx_trans_queue_depth, sizeof(uhci_transaction_desc_t *), UHCI_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(uhci_ctrl->tx_dir.trans_queues[i], ESP_ERR_NO_MEM, err, TAG, "no mem for transaction queue");
    }

    uhci_ctrl->tx_dir.trans_desc_pool = heap_caps_calloc(config->tx_trans_queue_depth, sizeof(uhci_transaction_desc_t), UHCI_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(uhci_ctrl->tx_dir.trans_desc_pool, ESP_ERR_NO_MEM, err, TAG, "no mem for transaction desc pool");
    uhci_transaction_desc_t *p_trans_desc = NULL;

    for (int i = 0; i < config->tx_trans_queue_depth; i++) {
        p_trans_desc = &uhci_ctrl->tx_dir.trans_desc_pool[i];
        xQueueSend(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_READY], &p_trans_desc, 0);
    }

    UHCI_RCC_ATOMIC() {
        uhci_ll_enable_bus_clock(uhci_ctrl->uhci_num, true);
        uhci_ll_reset_register(uhci_ctrl->uhci_num);
    }

    uhci_hal_init(&uhci_ctrl->hal, uhci_ctrl->uhci_num);
    uhci_ll_attach_uart_port(uhci_ctrl->hal.dev, config->uart_port);

#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

    sprintf(uhci_ctrl->pm_lock_name, "uhci_%d", uhci_ctrl->uhci_num); // e.g. uhci_0
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, uhci_ctrl->pm_lock_name, &uhci_ctrl->pm_lock), err, TAG, "create pm lock failed");
#endif

    // Must set separate character as 0, otherwise uhci will lose data.
    uhci_seper_chr_t seper_chr = {
        .sub_chr_en = 0,
    };
    uhci_ll_set_seper_chr(uhci_ctrl->hal.dev, &seper_chr);

    if (config->rx_eof_flags.idle_eof) {
        uhci_ll_rx_set_eof_mode(uhci_ctrl->hal.dev, UHCI_RX_IDLE_EOF);
    }
    if (config->rx_eof_flags.length_eof) {
        uhci_ll_rx_set_eof_mode(uhci_ctrl->hal.dev, UHCI_RX_LEN_EOF);
        uhci_ll_rx_set_packet_threshold(uhci_ctrl->hal.dev, config->max_packet_receive);
    }
    if (config->rx_eof_flags.rx_brk_eof) {
        uhci_ll_rx_set_eof_mode(uhci_ctrl->hal.dev, UHCI_RX_BREAK_CHR_EOF);
    }

    esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &uhci_ctrl->ext_mem_cache_line_size);
    esp_cache_get_alignment(MALLOC_CAP_INTERNAL, &uhci_ctrl->int_mem_cache_line_size);

    ESP_GOTO_ON_ERROR(uhci_gdma_initialize(uhci_ctrl, config), err, TAG, "uhci gdma initialize failed");

    *ret_uhci_ctrl = uhci_ctrl;
    return ESP_OK;
err:
    uhci_del_controller(uhci_ctrl);
    return ret;
}

esp_err_t uhci_register_event_callbacks(uhci_controller_handle_t uhci_ctrl, const uhci_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(uhci_ctrl && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

#if CONFIG_UHCI_ISR_HANDLER_IN_IRAM
    if (cbs->on_rx_trans_event) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_rx_trans_event), ESP_ERR_INVALID_ARG, TAG, "on_rx_trans_event callback not in IRAM");
    }
    if (cbs->on_tx_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_tx_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_tx_trans_done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    uhci_ctrl->rx_dir.on_rx_trans_event = cbs->on_rx_trans_event;
    uhci_ctrl->tx_dir.on_tx_trans_done = cbs->on_tx_trans_done;
    uhci_ctrl->user_data = user_data;
    return ESP_OK;
}

esp_err_t uhci_wait_all_tx_transaction_done(uhci_controller_handle_t uhci_ctrl, int timeout_ms)
{
    ESP_RETURN_ON_FALSE(uhci_ctrl, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    TickType_t wait_ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    uhci_transaction_desc_t *t = NULL;

    size_t cnt = uhci_ctrl->tx_dir.num_trans_inflight;
    for (size_t i = 0; i < cnt; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_COMPLETE], &t, wait_ticks) == pdTRUE,
                            ESP_ERR_TIMEOUT, TAG, "flush timeout");
        ESP_RETURN_ON_FALSE(xQueueSend(uhci_ctrl->tx_dir.trans_queues[UHCI_TRANS_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        atomic_fetch_sub(&uhci_ctrl->tx_dir.num_trans_inflight, 1);
    }
    return ESP_OK;
}

#if CONFIG_UHCI_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void uhci_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
