/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_clk_tree.h"
#include "esp_cache.h"
#include "esp_pm.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_private/gpio.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/gdma.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/esp_cache_private.h"
#include "driver/gpio.h"
#include "driver/i3c_master.h"
#include "driver/i3c_master_i2c.h"
#include "hal/gpio_hal.h"
#include "hal/i3c_master_hal.h"
#include "hal/dma_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "hal/i3c_master_ll.h"
#include "i3c_master_private.h"
#include "soc/clk_tree_defs.h"
#include "hal/i3c_master_periph.h"

static const char *TAG = "i3c.master";

typedef struct i3c_master_platform_t {
    portMUX_TYPE spinlock;                        // platform level spinlock.
    i3c_master_bus_handle_t buses[I3C_MASTER_LL_PERIPH_NUM];  // array of I3C master bus instances.
} i3c_master_platform_t;

static i3c_master_platform_t s_i3c_master_platform = {}; // singleton platform

static bool i3c_master_bus_occupied(i3c_master_bus_num_t bus_num)
{
    return s_i3c_master_platform.buses[bus_num] != NULL;
}

// Forward declaration for DMA transaction handler
static esp_err_t do_dma_transaction_handler(i3c_master_bus_handle_t bus_handle, i3c_transaction_desc_t *trans);

static esp_err_t do_fifo_transaction_handler(i3c_master_bus_handle_t bus_handle, i3c_transaction_desc_t *trans)
{
    bus_handle->cur_trans = trans;
    bus_handle->read_buffer_left_size = trans->read_buffer ? trans->command_table[0].cmd_h.regular.dl : 0;
    bus_handle->read_fifo_buffer_pointer = trans->read_buffer;

    size_t actual_write_size = 0;

    if (trans->scl_freq_hz > 400 * 1000) {
        i3c_master_ll_set_i2c_fast_mode_plus_timing(bus_handle->hal.dev, bus_handle->clock_source_freq, trans->scl_freq_hz);
    } else {
        i3c_master_ll_set_i2c_fast_mode_timing(bus_handle->hal.dev, bus_handle->clock_source_freq, trans->scl_freq_hz);
    }

    i3c_master_ll_reset_command_buf(bus_handle->hal.dev);
    i3c_master_ll_set_device_address_table(bus_handle->hal.dev, trans->addr_table, trans->addr_table_num);
    i3c_master_ll_set_command(bus_handle->hal.dev, trans->command_table, trans->command_table_num);

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    i3c_master_ll_enable_intr_mask(bus_handle->hal.dev, I3C_LL_MASTER_TRANSMIT_INTR | I3C_LL_MASTER_RECEIVE_INTR);
    i3c_master_ll_clear_intr_mask(bus_handle->hal.dev, I3C_LL_MASTER_TRANSMIT_INTR | I3C_LL_MASTER_RECEIVE_INTR);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    if (trans->write_buffer && trans->command_table[0].cmd_h.regular.dl >= I3C_LL_MASTER_DATA_BUFFER_SIZE) {
        actual_write_size = I3C_LL_MASTER_DATA_BUFFER_SIZE;
    } else {
        actual_write_size = trans->write_buffer ? trans->command_table[0].cmd_h.regular.dl : 0;
    }

    i3c_master_ll_write_tx_port(bus_handle->hal.dev, trans->write_buffer, actual_write_size);

    bus_handle->write_fifo_buffer_pointer = trans->write_buffer;
    bus_handle->write_buffer_left_size = trans->write_buffer ? trans->command_table[0].cmd_h.regular.dl : 0;

    bus_handle->write_fifo_buffer_pointer += actual_write_size;
    bus_handle->write_buffer_left_size -= actual_write_size;

    i3c_master_ll_start_transaction(bus_handle->hal.dev);

    return ESP_OK;
}

/**
    Handle tx buffer.
    If data to be sent is larger than fifo, we can add data and continue
    transfer with buf threshold interrupt.
*/
static bool handle_tx_data_buf_threshold_int(i3c_master_bus_handle_t i3c_master, uint32_t int_mask)
{
    portENTER_CRITICAL_SAFE(&i3c_master->spinlock);
    size_t empty_size = i3c_master_ll_get_tx_fifo_empty_count(i3c_master->hal.dev) * 4;
    size_t actual_write_size = 0;

    if (i3c_master->write_buffer_left_size >= empty_size) {
        actual_write_size = empty_size;
    } else {
        actual_write_size = i3c_master->write_buffer_left_size;
    }

    i3c_master_ll_write_tx_port(i3c_master->hal.dev, i3c_master->write_fifo_buffer_pointer, actual_write_size);
    i3c_master->write_fifo_buffer_pointer += actual_write_size;
    i3c_master->write_buffer_left_size -= actual_write_size;
    if (i3c_master->write_buffer_left_size > 0) {
        i3c_master_ll_clear_intr_mask(i3c_master->hal.dev, int_mask);
        i3c_master_ll_enable_intr_mask(i3c_master->hal.dev, int_mask);
    }
    portEXIT_CRITICAL_SAFE(&i3c_master->spinlock);
    return false;
}

/**
    Handle complete interrupt
    If there is still some remaining data to be received, receive data to buffer.
    If dma async transaction is enabled, start another transaction from queue.
*/
static bool handle_transfer_complete_int(i3c_master_bus_handle_t i3c_master)
{
    i3c_master_event_t event = I3C_MASTER_EVENT_TRANS_DONE;
    BaseType_t do_yield = pdFALSE;
    bool need_yield = false;

    if (i3c_master->use_dma_transaction == false) {
        portENTER_CRITICAL_SAFE(&i3c_master->spinlock);
        size_t rx_fifo_content = i3c_master_ll_get_rx_fifo_full_count(i3c_master->hal.dev) * 4;
        i3c_master_ll_read_rx_port(i3c_master->hal.dev, i3c_master->read_fifo_buffer_pointer, rx_fifo_content);

        i3c_master->read_buffer_left_size -= rx_fifo_content;
        i3c_master->read_fifo_buffer_pointer += rx_fifo_content;
        portEXIT_CRITICAL_SAFE(&i3c_master->spinlock);
    }

    i3c_master_ll_response_descriptor_t response_data;
    response_data = i3c_master_ll_get_response_data(i3c_master->hal.dev);
    if (response_data.err_sts == I3C_MASTER_LL_ADDRESS_NACK_OR_DYNAMIC_ADDRESS_NACK) {
        event = I3C_MASTER_EVENT_NACK;
    }

    if (i3c_master->async_transaction == false) {
        xQueueSendFromISR(i3c_master->event_queue, (void *)&event, &do_yield);
        if (do_yield) {
            need_yield |= true;
        }
    } else {
        i3c_transaction_desc_t *trans_desc = NULL;

        i3c_fsm_t expected_fsm = I3C_FSM_RUN;
        if (atomic_compare_exchange_strong(&i3c_master->fsm, &expected_fsm, I3C_FSM_WAIT)) {
            trans_desc = i3c_master->cur_trans;
            xQueueSendFromISR(i3c_master->trans_queues[I3C_TRANS_QUEUE_COMPLETE], &trans_desc, &do_yield);
            if (do_yield) {
                need_yield = true;
            }
            atomic_store(&i3c_master->fsm, I3C_FSM_ENABLE);
        }

        if (trans_desc->i2c_trans) {
            i3c_master_i2c_device_handle_t i2c_dev = (i3c_master_i2c_device_handle_t)i3c_master->cur_trans->dev_handle;
            if (i3c_master->cur_trans->read_buffer != NULL) {
                size_t dma_rcv_size = gdma_link_count_buffer_size_till_eof(i3c_master->rx_dma_link, 0);
                size_t c2m_aligned_size = I3C_ALIGN_UP(dma_rcv_size, i3c_master->cache_line_size);
                esp_cache_msync(i3c_master->cur_trans->read_buffer, c2m_aligned_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
                i3c_master_i2c_device_event_data_t evt_data = {
                    .data = i3c_master->cur_trans->read_buffer,
                    .data_size = dma_rcv_size,
                    .event = event,
                };

                if (i2c_dev->on_trans_done) {
                    if (i2c_dev->on_trans_done(i2c_dev, &evt_data, i2c_dev->user_ctx)) {
                        need_yield = true;
                    }
                }
            }
        } else {
            i3c_master_i3c_device_handle_t i3c_dev = (i3c_master_i3c_device_handle_t)i3c_master->cur_trans->dev_handle;
            if (i3c_master->cur_trans->read_buffer != NULL) {
                size_t dma_rcv_size = gdma_link_count_buffer_size_till_eof(i3c_master->rx_dma_link, 0);
                size_t c2m_aligned_size = I3C_ALIGN_UP(dma_rcv_size, i3c_master->cache_line_size);
                esp_cache_msync(i3c_master->cur_trans->read_buffer, c2m_aligned_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
                i3c_master_i3c_device_event_data_t evt_data = {
                    .data = i3c_master->cur_trans->read_buffer,
                    .data_size = dma_rcv_size,
                    .event = event,
                };

                if (i3c_dev->on_trans_done) {
                    if (i3c_dev->on_trans_done(i3c_dev, &evt_data, i3c_dev->user_ctx)) {
                        need_yield = true;
                    }
                }
            }
        }

        expected_fsm = I3C_FSM_ENABLE;
        if (atomic_compare_exchange_strong(&i3c_master->fsm, &expected_fsm, I3C_FSM_WAIT)) {
            if (xQueueReceiveFromISR(i3c_master->trans_queues[I3C_TRANS_QUEUE_PROGRESS], &trans_desc, &do_yield) == pdTRUE) {
                atomic_store(&i3c_master->fsm, I3C_FSM_RUN);
                i3c_master->transaction_handler(i3c_master, trans_desc);
                if (do_yield) {
                    need_yield = true;
                }
            } else {
                atomic_store(&i3c_master->fsm, I3C_FSM_ENABLE);
            }
        }
    }
    return need_yield;
}

static void handle_rx_data_buf_threshold_int(i3c_master_bus_handle_t i3c_master)
{
    if (i3c_master->use_dma_transaction == false) {
        portENTER_CRITICAL_SAFE(&i3c_master->spinlock);
        size_t rx_fifo_content = i3c_master_ll_get_rx_fifo_full_count(i3c_master->hal.dev) * 4;
        i3c_master_ll_read_rx_port(i3c_master->hal.dev, i3c_master->read_fifo_buffer_pointer, rx_fifo_content);

        i3c_master->read_buffer_left_size -= rx_fifo_content;
        i3c_master->read_fifo_buffer_pointer += rx_fifo_content;
        portEXIT_CRITICAL_SAFE(&i3c_master->spinlock);
    }
}

static bool handle_ibi_status_thld_int(i3c_master_bus_handle_t i3c_master)
{
    bool need_yield = false;
    i3c_master_ll_ibi_status_descriptor_t ibi_status;
    i3c_master_ll_get_ibi_status(i3c_master->hal.dev, &ibi_status);

    i3c_master_i3c_dev_t *matched_dev = NULL;
    i3c_master_i3c_dev_t *it = NULL;
    SLIST_FOREACH(it, &i3c_master->i3c_device_list, next) {
        uint8_t da = it->dynamic_addr;
        if (da == (ibi_status.ibi_identifier >> 1)) {
            matched_dev = it;
            break;
        }
    }

    if (matched_dev && matched_dev->on_ibi) {
        i3c_master_ibi_info_t evt = {};
        evt.ibi_id = ibi_status.ibi_identifier;
        evt.ibi_sts = (uint8_t)ibi_status.ibi_sts;
        evt.data_length = ibi_status.ibi_data_length;
        if (evt.data_length > 0) {
            size_t read_len = MIN((size_t)evt.data_length, sizeof(evt.ibi_data));
            i3c_master_ll_get_ibi_data(i3c_master->hal.dev, evt.ibi_data, read_len);
            evt.data_length = (size_t)read_len;
        }
        if (matched_dev->on_ibi(matched_dev, &evt, matched_dev->user_ctx)) {
            need_yield = true;
        }
    }

    return need_yield;
}

static void i3c_master_isr_handler_default(void *arg)
{
    i3c_master_bus_handle_t i3c_master = (i3c_master_bus_t*) arg;
    uint32_t int_mask = 0;
    bool need_yield = false;
    portENTER_CRITICAL_SAFE(&i3c_master->spinlock);
    i3c_master_ll_get_intr_mask(i3c_master->hal.dev, &int_mask);
    i3c_master_ll_clear_intr_mask(i3c_master->hal.dev, int_mask);
    if (int_mask & I3C_MST_TX_DATA_BUF_THLD_INT_ST) {
        i3c_master_ll_disable_intr_mask(i3c_master->hal.dev, I3C_LL_MASTER_TRANSMIT_INTR);
    } else if (int_mask & I3C_MST_RX_DATA_BUF_THLD_INT_ST) {
        i3c_master_ll_disable_intr_mask(i3c_master->hal.dev, I3C_LL_MASTER_RECEIVE_INTR);
    }
    portEXIT_CRITICAL_SAFE(&i3c_master->spinlock);

    if (int_mask & I3C_MST_IBI_HANDLE_DONE_INT_ST_M) {
        if (handle_ibi_status_thld_int(i3c_master)) {
            need_yield = true;
        }
    }

    if (int_mask & I3C_MST_TX_DATA_BUF_THLD_INT_ST) {
        if (handle_tx_data_buf_threshold_int(i3c_master, int_mask)) {
            need_yield = true;
        }
    }

    if (int_mask & I3C_MST_TRANSFER_COMPLETE_INT_ST) {
        if (handle_transfer_complete_int(i3c_master)) {
            need_yield = true;
        }
    }

    if (int_mask & I3C_MST_RX_DATA_BUF_THLD_INT_ST) {
        handle_rx_data_buf_threshold_int(i3c_master);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t i3c_master_async_transaction_preparation(i3c_master_bus_t *i3c_master_handle)
{
    i3c_master_handle->fsm = I3C_FSM_ENABLE;

    for (int i = 0; i < I3C_TRANS_QUEUE_MAX; i++) {
        i3c_master_handle->trans_queues[i] = xQueueCreateWithCaps(i3c_master_handle->queue_depth, sizeof(i3c_transaction_desc_t *), I3C_MASTER_MEM_ALLOC_CAPS);
        ESP_RETURN_ON_FALSE(i3c_master_handle->trans_queues[i], ESP_ERR_NO_MEM, TAG, "no mem for transaction queue");
    }

    i3c_master_handle->trans_desc_pool = heap_caps_calloc(i3c_master_handle->queue_depth, sizeof(i3c_transaction_desc_t), I3C_MASTER_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(i3c_master_handle->trans_desc_pool, ESP_ERR_NO_MEM, TAG, "no mem for transaction desc pool");
    i3c_transaction_desc_t *p_trans_desc = NULL;
    for (int i = 0; i < i3c_master_handle->queue_depth; i++) {
        p_trans_desc = &i3c_master_handle->trans_desc_pool[i];
        ESP_RETURN_ON_FALSE(xQueueSend(i3c_master_handle->trans_queues[I3C_TRANS_QUEUE_READY], &p_trans_desc, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
    }
    i3c_master_handle->i3c_async_addr_table = (i3c_master_ll_device_address_descriptor_t(*)[I3C_MASTER_LL_ADDRESS_TABLE_NUM])heap_caps_calloc(i3c_master_handle->queue_depth, sizeof(*i3c_master_handle->i3c_async_addr_table), I3C_MASTER_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(i3c_master_handle->i3c_async_addr_table, ESP_ERR_NO_MEM, TAG, "no mem for address table");

    i3c_master_handle->i3c_async_command_table = (i3c_master_ll_command_descriptor_t(*)[I3C_MASTER_LL_COMMAND_TABLE_NUM])heap_caps_calloc(i3c_master_handle->queue_depth, sizeof(*i3c_master_handle->i3c_async_command_table), I3C_MASTER_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(i3c_master_handle->i3c_async_command_table, ESP_ERR_NO_MEM, TAG, "no mem for command table");

    i3c_master_handle->ops_prepare_idx = 0;

    i3c_master_handle->async_memory_allocated = true;

    return ESP_OK;
}

static esp_err_t i3c_master_init_dma(i3c_master_bus_t *i3c_master_handle, const i3c_master_dma_config_t *dma_config)
{
    esp_err_t ret = ESP_OK;
    // Initialize i3c-dma connection
    i3c_master_ll_enable_tx_by_dma(i3c_master_handle->hal.dev, true);
    i3c_master_ll_enable_rx_by_dma(i3c_master_handle->hal.dev, true);

    // Initialize DMA TX channel
    gdma_channel_alloc_config_t dma_cfg = {
#if CONFIG_I3C_MASTER_ISR_CACHE_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&dma_cfg, &i3c_master_handle->dma_tx_chan, NULL), TAG, "DMA tx channel alloc failed");
    ESP_GOTO_ON_ERROR(gdma_connect(i3c_master_handle->dma_tx_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_I3C, 0)), err1, TAG, "Connect tx dma channel error");

    gdma_transfer_config_t transfer_cfg = {
        .access_ext_mem = false,
        .max_data_burst_size = dma_config->dma_burst_size ? dma_config->dma_burst_size : 16,
    };
    ESP_GOTO_ON_ERROR(gdma_config_transfer(i3c_master_handle->dma_tx_chan, &transfer_cfg), err2, TAG, "Config DMA tx channel transfer failed");

    // create DMA link list
    size_t int_mem_align = 0;
    gdma_get_alignment_constraints(i3c_master_handle->dma_tx_chan, &int_mem_align, NULL);
    i3c_master_handle->dma_buffer_alignment = I3C_ALIGN_UP(int_mem_align, I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
    size_t num_dma_nodes = esp_dma_calculate_node_count(dma_config->max_transfer_size, i3c_master_handle->dma_buffer_alignment, DMA_DESCRIPTOR_BUFFER_MAX_SIZE);
    gdma_link_list_config_t dma_link_config = {
        .item_alignment = 4,                   // 4 bytes alignment for AHB-DMA
        .num_items = num_dma_nodes,            // only one item in the link list so far
    };

    ESP_GOTO_ON_ERROR(gdma_new_link_list(&dma_link_config, &i3c_master_handle->tx_dma_link), err2, TAG, "DMA tx link list alloc failed");

    // Initialize DMA RX channel
    ESP_GOTO_ON_ERROR(gdma_new_ahb_channel(&dma_cfg, NULL, &i3c_master_handle->dma_rx_chan), err2, TAG, "DMA rx channel alloc failed");
    ESP_GOTO_ON_ERROR(gdma_connect(i3c_master_handle->dma_rx_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_I3C, 0)), err1, TAG, "Connect rx dma channel error");

    ESP_GOTO_ON_ERROR(gdma_config_transfer(i3c_master_handle->dma_rx_chan, &transfer_cfg), err2, TAG, "Config DMA rx channel transfer failed");

    ESP_GOTO_ON_ERROR(gdma_new_link_list(&dma_link_config, &i3c_master_handle->rx_dma_link), err2, TAG, "DMA rx link list alloc failed");

    return ESP_OK;

err2:
    gdma_disconnect(i3c_master_handle->dma_rx_chan);
    gdma_disconnect(i3c_master_handle->dma_tx_chan);
err1:
    gdma_del_channel(i3c_master_handle->dma_rx_chan);
    gdma_del_channel(i3c_master_handle->dma_tx_chan);
    return ret;
}

static esp_err_t i3c_pins_config(const i3c_master_bus_config_t *bus_config)
{
    esp_err_t ret = ESP_OK;
    // SDA pin configurations
    ESP_RETURN_ON_ERROR(gpio_set_level(bus_config->sda_io_num, 1), TAG, "i3c sda pin set level failed");
    gpio_input_enable(bus_config->sda_io_num);
    gpio_func_sel(bus_config->sda_io_num, PIN_FUNC_GPIO);
    gpio_matrix_output(bus_config->sda_io_num, i3c_master_periph_signal->sda_out_sig, 0, 0);
    gpio_matrix_input(bus_config->sda_io_num, i3c_master_periph_signal->sda_in_sig, 0);

    // SCL pin configurations
    ESP_RETURN_ON_ERROR(gpio_set_level(bus_config->scl_io_num, 1), TAG, "i3c scl pin set level failed");
    gpio_input_enable(bus_config->scl_io_num);
    gpio_func_sel(bus_config->scl_io_num, PIN_FUNC_GPIO);
    gpio_matrix_output(bus_config->scl_io_num, i3c_master_periph_signal->scl_out_sig, 0, 0);
    gpio_matrix_input(bus_config->scl_io_num, i3c_master_periph_signal->scl_in_sig, 0);
    return ret;
}

static void i3c_master_deinit_dma(i3c_master_bus_t *i3c_master_handle)
{
    if (i3c_master_handle->dma_tx_chan) {
        gdma_disconnect(i3c_master_handle->dma_tx_chan);
        gdma_del_channel(i3c_master_handle->dma_tx_chan);
    }
    if (i3c_master_handle->dma_rx_chan) {
        gdma_disconnect(i3c_master_handle->dma_rx_chan);
        gdma_del_channel(i3c_master_handle->dma_rx_chan);
    }
    if (i3c_master_handle->tx_dma_link) {
        gdma_del_link_list(i3c_master_handle->tx_dma_link);
    }
    if (i3c_master_handle->rx_dma_link) {
        gdma_del_link_list(i3c_master_handle->rx_dma_link);
    }
}

static void i3c_master_del_async_transaction_source(i3c_master_bus_t *i3c_master_handle)
{
    for (int i = 0; i < I3C_TRANS_QUEUE_MAX; i++) {
        if (i3c_master_handle->trans_queues[i]) {
            vQueueDeleteWithCaps(i3c_master_handle->trans_queues[i]);
        }
    }
    if (i3c_master_handle->trans_desc_pool) {
        heap_caps_free(i3c_master_handle->trans_desc_pool);
    }
    if (i3c_master_handle->i3c_async_addr_table) {
        heap_caps_free(i3c_master_handle->i3c_async_addr_table);
    }
    if (i3c_master_handle->i3c_async_command_table) {
        heap_caps_free(i3c_master_handle->i3c_async_command_table);
    }
}

static esp_err_t i3c_master_bus_destroy(i3c_master_bus_handle_t bus_handle)
{
    ESP_RETURN_ON_FALSE(bus_handle, ESP_ERR_INVALID_ARG, TAG, "no memory for i3c master bus");

    portENTER_CRITICAL_SAFE(&s_i3c_master_platform.spinlock);
    if (s_i3c_master_platform.buses[bus_handle->i3c_num] != NULL) {
        s_i3c_master_platform.buses[bus_handle->i3c_num] = NULL;
    }
    portEXIT_CRITICAL_SAFE(&s_i3c_master_platform.spinlock);

    if (bus_handle->bus_lock_mux) {
        vSemaphoreDeleteWithCaps(bus_handle->bus_lock_mux);
        bus_handle->bus_lock_mux = NULL;
    }

    if (bus_handle->event_queue) {
        vQueueDeleteWithCaps(bus_handle->event_queue);
    }

    if (bus_handle->intr_handle) {
        ESP_RETURN_ON_ERROR(esp_intr_free(bus_handle->intr_handle), TAG, "delete interrupt service failed");
    }

    if (bus_handle->clock_source) {
        esp_clk_tree_enable_src((soc_module_clk_t)bus_handle->clock_source, false);
    }

#if CONFIG_PM_ENABLE
    if (bus_handle->pm_lock) {
        esp_pm_lock_delete(bus_handle->pm_lock);
    }
#endif

    // Free async transaction resources
    if (bus_handle->async_transaction) {
        i3c_master_del_async_transaction_source(bus_handle);
    }

    // Free the handle itself
    heap_caps_free(bus_handle);

    return ESP_OK;
}

esp_err_t i3c_del_master_bus(i3c_master_bus_handle_t bus_handle)
{
    // Check if the device list is empty
    if (!SLIST_EMPTY(&bus_handle->i2c_device_list) || !SLIST_EMPTY(&bus_handle->i3c_device_list)) {
        ESP_LOGE(TAG, "Cannot delete I3C bus: devices are still attached, please remove all devices and then delete bus");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(i3c_master_bus_destroy(bus_handle), TAG, "destroy i3c bus failed");

    return ESP_OK;
}

esp_err_t i3c_master_i3c_device_ibi_config(i3c_master_i3c_device_handle_t dev_handle, const i3c_ibi_config_t *config)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid dev handle argument");
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid config argument");

    portENTER_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);

    i3c_master_ll_set_ibi_accept(dev_handle->base->bus_handle->hal.dev, dev_handle->dynamic_addr, config->enable_ibi);
    i3c_master_ll_set_ibi_payload(dev_handle->base->bus_handle->hal.dev, dev_handle->dynamic_addr, config->enable_ibi_payload);

    portEXIT_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);

    return ESP_OK;
}

// DMA transaction handler implementation - only linked when DMA is enabled
static esp_err_t do_dma_transaction_handler(i3c_master_bus_handle_t bus_handle, i3c_transaction_desc_t *trans)
{
    esp_err_t err = ESP_OK;
    bus_handle->cur_trans = trans;

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    if (trans->scl_freq_hz > 400 * 1000) {
        i3c_master_ll_set_i2c_fast_mode_plus_timing(bus_handle->hal.dev, bus_handle->clock_source_freq, trans->scl_freq_hz);
    } else {
        i3c_master_ll_set_i2c_fast_mode_timing(bus_handle->hal.dev, bus_handle->clock_source_freq, trans->scl_freq_hz);
    }

    i3c_master_ll_reset_command_buf(bus_handle->hal.dev);
    i3c_master_ll_set_device_address_table(bus_handle->hal.dev, trans->addr_table, trans->addr_table_num);
    i3c_master_ll_set_command(bus_handle->hal.dev, trans->command_table, trans->command_table_num);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    for (int i = 0; i < trans->command_table_num; i++) {
        if (trans->command_table[i].cmd_l.regular.rnw == I3C_MASTER_LL_TRANSFER_DIR_WRITE) {
            size_t write_size = trans->command_table[i].cmd_h.regular.dl;
            size_t dma_aligned_size = I3C_ALIGN_UP(write_size, I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
            gdma_buffer_mount_config_t mount_config = {
                .buffer = trans->write_buffer,
                .buffer_alignment = bus_handle->dma_buffer_alignment,
                .length = dma_aligned_size,
                .flags = {
                    .mark_eof = true,
                    .mark_final = GDMA_FINAL_LINK_TO_NULL,
                }
            };

            portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
            i3c_master_ll_enable_intr_mask(bus_handle->hal.dev, I3C_LL_MASTER_TRANSMIT_INTR);
            i3c_master_ll_clear_intr_mask(bus_handle->hal.dev, I3C_LL_MASTER_TRANSMIT_INTR);
            portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);
            gdma_link_mount_buffers(bus_handle->tx_dma_link, 0, &mount_config, 1, NULL);
            err = esp_cache_msync(trans->write_buffer, write_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
            if (err != ESP_OK) {
                ESP_DRAM_LOGE(TAG, "memory sync failed");
                return err;
            }
            gdma_reset(bus_handle->dma_tx_chan);
            gdma_start(bus_handle->dma_tx_chan, gdma_link_get_head_addr(bus_handle->tx_dma_link));
        }
        if (trans->command_table[i].cmd_l.regular.rnw == I3C_MASTER_LL_TRANSFER_DIR_READ) {
            size_t read_size = trans->command_table[i].cmd_h.regular.dl;
            size_t dma_aligned_size = I3C_ALIGN_UP(read_size, I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
            gdma_buffer_mount_config_t mount_config = {
                .buffer = trans->read_buffer,
                .buffer_alignment = bus_handle->dma_buffer_alignment,
                .length = dma_aligned_size,
                .flags = {
                    .mark_eof = true,
                    .mark_final = GDMA_FINAL_LINK_TO_NULL,
                }
            };

            portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
            i3c_master_ll_enable_intr_mask(bus_handle->hal.dev, I3C_LL_MASTER_RECEIVE_INTR);
            i3c_master_ll_clear_intr_mask(bus_handle->hal.dev, I3C_LL_MASTER_RECEIVE_INTR);
            portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);
            gdma_link_mount_buffers(bus_handle->rx_dma_link, 0, &mount_config, 1, NULL);
            gdma_reset(bus_handle->dma_rx_chan);
            gdma_start(bus_handle->dma_rx_chan, gdma_link_get_head_addr(bus_handle->rx_dma_link));
        }
    }

    // transaction start
    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    i3c_master_ll_start_transaction(bus_handle->hal.dev);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);
    return ESP_OK;
}

esp_err_t i3c_master_bus_decorate_dma(i3c_master_bus_handle_t bus_handle, const i3c_master_dma_config_t *dma_config)
{
    ESP_RETURN_ON_FALSE(bus_handle, ESP_ERR_INVALID_ARG, TAG, "invalid bus handle");

    // Initialize DMA using the max transfer size from DMA configuration
    if (dma_config) {
        ESP_RETURN_ON_ERROR(i3c_master_init_dma(bus_handle, dma_config), TAG, "DMA initialization failed");
        // Mark DMA as initialized and enable DMA transactions
        bus_handle->dma_initialized = true;
        bus_handle->use_dma_transaction = true;
        // Switch to DMA transaction handler
        bus_handle->transaction_handler = do_dma_transaction_handler;
    } else {
        i3c_master_deinit_dma(bus_handle);
        bus_handle->dma_initialized = false;
        bus_handle->use_dma_transaction = false;
        // Switch back to FIFO transaction handler
        bus_handle->transaction_handler = do_fifo_transaction_handler;
    }

    return ESP_OK;
}

static void i3c_master_set_address_slot(i3c_master_bus_handle_t bus_handle, uint8_t address, uint8_t slot_status)
{
    uint16_t bit_pos = (uint16_t)address * I3C_BUS_ADDR_SLOTWIDTH;
    uint32_t *slot_ptr;

    if (address > I3C_BUS_MAX_ADDR) {
        return;
    }

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    slot_ptr = &bus_handle->addr_slots[bit_pos / I3C_BUS_ADDR_SLOTDEPTH];
    *slot_ptr &= ~((uint32_t)I3C_BUS_ADDR_SLOTMASK << (bit_pos % I3C_BUS_ADDR_SLOTDEPTH));
    *slot_ptr |= (uint32_t)slot_status << (bit_pos % I3C_BUS_ADDR_SLOTDEPTH);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);
}

static void i3c_master_init_address_slots(i3c_master_bus_handle_t bus_handle)
{
    /* Reserve address 0x0 to 0x7. */
    for (int i = 0; i < 8U; i++) {
        i3c_master_set_address_slot(bus_handle, i, I3C_ADDR_SLOT_RESERVED);
    }

    /* Reserve the conditional restriction I3C address. */
    i3c_master_set_address_slot(bus_handle, 0x78U, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, 0x79U, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, 0x7BU, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, 0x7DU, I3C_ADDR_SLOT_RESERVED);

    /* All are prohibited since I3C targets will interpret an I3C address header with any of these addresses as a
     * broadcast address with a single-bit error. */
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR1, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR2, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR3, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR4, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR5, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR6, I3C_ADDR_SLOT_RESERVED);
    i3c_master_set_address_slot(bus_handle, I3C_BOARDCAST_SINGLE_BIT_ERR_DETECT_ADDR7, I3C_ADDR_SLOT_RESERVED);

    /* Reserve I3C broadcast address. */
    i3c_master_set_address_slot(bus_handle, I3C_BUS_BROADCAST_ADDR, I3C_ADDR_SLOT_RESERVED);
}

static uint8_t i3c_master_get_address_slot_status(i3c_master_bus_handle_t bus_handle, uint8_t check_address)
{
    uint16_t bit_pos = (uint16_t)check_address * 2U;
    uint32_t use_status = 0;

    if (check_address > I3C_BUS_MAX_ADDR) {
        return (uint8_t)I3C_ADDR_SLOT_RESERVED;
    }

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    use_status = bus_handle->addr_slots[bit_pos / I3C_BUS_ADDR_SLOTDEPTH];
    use_status >>= bit_pos % I3C_BUS_ADDR_SLOTDEPTH;
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    return (uint8_t)(use_status & (uint8_t)I3C_BUS_ADDR_SLOTMASK);
}

esp_err_t i3c_master_get_valid_address_slot(i3c_master_bus_handle_t bus_handle, uint8_t start_address, uint8_t *ret_addr)
{
    ESP_RETURN_ON_FALSE(bus_handle, ESP_ERR_INVALID_ARG, TAG, "invalid bus handle");
    ESP_RETURN_ON_FALSE(start_address <= I3C_BUS_MAX_ADDR, ESP_ERR_INVALID_ARG, TAG, "invalid start address");

    for (uint8_t addr = start_address; addr < I3C_BUS_MAX_ADDR; addr++) {
        if (i3c_master_get_address_slot_status(bus_handle, addr) == (uint8_t)I3C_ADDR_SLOT_FREE) {
            *ret_addr = addr;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t i3c_new_master_bus(const i3c_master_bus_config_t *bus_config, i3c_master_bus_handle_t *ret_bus_handle)
{
    ESP_RETURN_ON_FALSE(bus_config && ret_bus_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(bus_config->sda_io_num) && GPIO_IS_VALID_GPIO(bus_config->scl_io_num), ESP_ERR_INVALID_ARG, TAG, "invalid SDA/SCL pin number");

    esp_err_t ret = ESP_OK;
    i3c_master_bus_t *i3c_master_handle = NULL;

    i3c_master_handle = (i3c_master_bus_t*) heap_caps_calloc(1, sizeof(i3c_master_bus_t), I3C_MASTER_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(i3c_master_handle, ESP_ERR_NO_MEM, TAG, "no mem for i3c master bus handle");

    s_i3c_master_platform.spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&s_i3c_master_platform.spinlock);
    bool bus_found = false;
    for (int i = 0; i < I3C_MASTER_LL_PERIPH_NUM; i++) {
        if (i3c_master_bus_occupied(i) == false) {
            s_i3c_master_platform.buses[i] = i3c_master_handle;
            bus_found = true;
            i3c_master_handle->i3c_num = i;
            break;
        }
    }
    portEXIT_CRITICAL_SAFE(&s_i3c_master_platform.spinlock);
    if (bus_found == false) {
        ESP_LOGE(TAG, "acquire i3c master bus failed, no free bus");
        ret = ESP_ERR_NOT_FOUND;
        goto err;
    }

    i3c_master_hal_init(&i3c_master_handle->hal, i3c_master_handle->i3c_num);

    /// clock enable
    PERIPH_RCC_ATOMIC() {
        i3c_master_ll_enable_bus_clock(i3c_master_handle->hal.dev, true);
        i3c_master_ll_reset_register(i3c_master_handle->hal.dev);
    }
    PERIPH_RCC_ATOMIC() {
        i3c_master_ll_enable_controller_clock(i3c_master_handle->hal.dev, true);
    }

    //// clock selection
    uint32_t periph_src_clk_hz = 0;
    i3c_master_handle->clock_source = bus_config->clock_source;

    esp_clk_tree_enable_src((soc_module_clk_t)i3c_master_handle->clock_source, true);
    PERIPH_RCC_ATOMIC() {
        i3c_master_ll_set_source_clk(i3c_master_handle->hal.dev, i3c_master_handle->clock_source);
    }

    if (bus_config->internal_pullup_resistor_val != I3C_MASTER_INTERNAL_PULLUP_RESISTOR_DISABLED) {
        ESP_RETURN_ON_FALSE((BIT64(bus_config->sda_io_num) & I3C_LL_MASTER_INTERNAL_PULLUP_IO_PINS_MASK) && (BIT64(bus_config->scl_io_num) & I3C_LL_MASTER_INTERNAL_PULLUP_IO_PINS_MASK), ESP_ERR_INVALID_ARG, TAG, "The SDA/SCL pin number is not supported for internal pull-up resistor");
        i3c_master_ll_enable_internal_pullup(bus_config->sda_io_num, true);
        i3c_master_ll_set_internal_pullup_value(bus_config->sda_io_num, bus_config->internal_pullup_resistor_val);
        i3c_master_ll_enable_internal_pullup(bus_config->sda_io_num, true);
        i3c_master_ll_set_internal_pullup_value(bus_config->sda_io_num, bus_config->internal_pullup_resistor_val);
    }

    i3c_master_handle->dma_initialized = false;
    i3c_master_handle->use_dma_transaction = false;
    i3c_master_handle->async_transaction = bus_config->flags.enable_async_trans;

    // Set default transaction handler to FIFO mode
    i3c_master_handle->transaction_handler = do_fifo_transaction_handler;

    esp_clk_tree_src_get_freq_hz(i3c_master_handle->clock_source, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &periph_src_clk_hz);

    i3c_master_handle->clock_source_freq = periph_src_clk_hz;
    i3c_master_handle->queue_depth = bus_config->trans_queue_depth;

    // set timing regs
    uint32_t clock_source_period_ns = 1000000000UL / periph_src_clk_hz;
    i3c_master_ll_set_restart_setup_time(i3c_master_handle->hal.dev, I3C_MASTER_LL_DEFAULT_SETUP_TIME, clock_source_period_ns);
    i3c_master_ll_set_start_hold_time(i3c_master_handle->hal.dev, I3C_MASTER_LL_DEFAULT_SETUP_TIME, clock_source_period_ns);
    i3c_master_ll_set_stop_hold_time(i3c_master_handle->hal.dev, I3C_MASTER_LL_DEFAULT_SETUP_TIME, clock_source_period_ns);
    i3c_master_ll_set_stop_setup_time(i3c_master_handle->hal.dev, I3C_MASTER_LL_DEFAULT_SETUP_TIME, clock_source_period_ns);

    // Convert float duty cycle to numerator and denominator for LL function
    // Use 10000 as denominator to support 0.01 precision
    const uint32_t duty_cycle_denom = 100;
    float od_duty_cycle = (bus_config->i3c_scl_od_duty_cycle == 0) ? 0.5 : bus_config->i3c_scl_od_duty_cycle;
    float pp_duty_cycle = (bus_config->i3c_scl_pp_duty_cycle == 0) ? 0.5 : bus_config->i3c_scl_pp_duty_cycle;
    uint32_t od_duty_cycle_num = (uint32_t)(od_duty_cycle * duty_cycle_denom);
    uint32_t pp_duty_cycle_num = (uint32_t)(pp_duty_cycle * duty_cycle_denom);
    i3c_master_ll_set_i3c_open_drain_timing(i3c_master_handle->hal.dev, periph_src_clk_hz, bus_config->i3c_scl_freq_hz_od, od_duty_cycle_num, duty_cycle_denom);
    i3c_master_ll_set_i3c_push_pull_timing(i3c_master_handle->hal.dev, periph_src_clk_hz, bus_config->i3c_scl_freq_hz_pp, pp_duty_cycle_num, duty_cycle_denom);
    // Convert hold time from nanoseconds to clock cycles
    uint16_t od_sda_hold_time_cycles = (uint16_t)(bus_config->i3c_sda_od_hold_time_ns / clock_source_period_ns);
    uint16_t pp_sda_hold_time_cycles = (uint16_t)(bus_config->i3c_sda_pp_hold_time_ns / clock_source_period_ns);
    i3c_master_ll_set_od_sda_hold_time(i3c_master_handle->hal.dev, od_sda_hold_time_cycles);
    i3c_master_ll_set_pp_sda_hold_time(i3c_master_handle->hal.dev, pp_sda_hold_time_cycles);
    i3c_master_handle->entdaa_device_num = bus_config->entdaa_device_num;

    // IBI global configuration
    i3c_master_ll_set_ibi_rstart_trans_en(i3c_master_handle->hal.dev, bus_config->flags.ibi_rstart_trans_en);
    i3c_master_ll_set_ibi_notify_sir_rejected(i3c_master_handle->hal.dev, !bus_config->flags.ibi_silent_sir_rejected);
    i3c_master_ll_set_auto_disable_ibi(i3c_master_handle->hal.dev, !bus_config->flags.ibi_no_auto_disable);

    i3c_master_handle->bus_lock_mux = xSemaphoreCreateBinaryWithCaps(I3C_MASTER_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i3c_master_handle->bus_lock_mux, ESP_ERR_NO_MEM, err, TAG, "No memory for binary semaphore");
    xSemaphoreGive(i3c_master_handle->bus_lock_mux);

    i3c_master_handle->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;

    /// pins configuration
    ESP_GOTO_ON_ERROR(i3c_pins_config(bus_config), err, TAG, "pins initialization failed");

    esp_cache_get_alignment(MALLOC_CAP_INTERNAL, &i3c_master_handle->cache_line_size);

#if CONFIG_PM_ENABLE
    bool need_pm_lock = true;
    // to make the I3C work reliable, the source clock must stay alive and unchanged
    // driver will create different pm lock for that purpose, according to different clock source
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

    if (need_pm_lock) {
        ret = esp_pm_lock_create(pm_lock_type, 0, i3c_master_periph_signal[0].module_name, &i3c_master_handle->pm_lock);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "create pm lock failed");
    }

#endif

    if (i3c_master_handle->use_dma_transaction == false) {
        i3c_master_ll_set_rx_data_fifo_wm_threshold(i3c_master_handle->hal.dev, I3C_MASTER_LL_FIFO_WM_LENGTH_16);
        i3c_master_ll_set_tx_data_fifo_wm_threshold(i3c_master_handle->hal.dev, I3C_MASTER_LL_FIFO_WM_LENGTH_16);
    }

    i3c_master_handle->event_queue = xQueueCreateWithCaps(1, sizeof(i3c_master_event_t), I3C_MASTER_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i3c_master_handle->event_queue, ESP_ERR_NO_MEM, err, TAG, "No memory for event queue");

    SLIST_INIT(&i3c_master_handle->i2c_device_list);

    // interrupt configurations
    if (bus_config->intr_priority) {
        ESP_GOTO_ON_FALSE((1 << (bus_config->intr_priority)) & I3C_MASTER_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, err, TAG, "invalid interrupt priority:%d", bus_config->intr_priority);
    }

    int isr_flags = I3C_MASTER_INTR_ALLOC_FLAG;
    if (bus_config->intr_priority) {
        isr_flags |= 1 << (bus_config->intr_priority);
    }

    i3c_master_ll_enable_intr_mask(i3c_master_handle->hal.dev, I3C_LL_MASTER_COMMON_INTR);
    i3c_master_ll_clear_intr_mask(i3c_master_handle->hal.dev, I3C_LL_MASTER_COMMON_INTR);

    ret = esp_intr_alloc_intrstatus(i3c_master_periph_signal->irq, isr_flags, (uint32_t)i3c_master_ll_get_interrupt_status_reg(i3c_master_handle->hal.dev), I3C_LL_MASTER_EVENT_INTR, i3c_master_isr_handler_default, i3c_master_handle, &i3c_master_handle->intr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install i3c master interrupt failed");

    // Disable DMA by default, will be enabled when decorator is called
    i3c_master_ll_enable_tx_by_dma(i3c_master_handle->hal.dev, false);
    i3c_master_ll_enable_rx_by_dma(i3c_master_handle->hal.dev, false);

    // Initialize async transaction memory if async transaction is needed.
    // Note: DMA dependency will be checked at runtime when transactions are executed
    if (i3c_master_handle->async_transaction) {
        ESP_GOTO_ON_ERROR(i3c_master_async_transaction_preparation(i3c_master_handle), err, TAG, "async transaction resource failed");
    }

    i3c_master_init_address_slots(i3c_master_handle);

    *ret_bus_handle = i3c_master_handle;

    return ESP_OK;

err:
    if (i3c_master_handle) {
        i3c_master_bus_destroy(i3c_master_handle);
    }
    return ret;
}

esp_err_t i3c_master_bus_rm_i2c_device(i3c_master_i2c_device_handle_t handle)
{
    ESP_RETURN_ON_FALSE((handle != NULL), ESP_ERR_INVALID_ARG, TAG, "this device is not initialized");

    i3c_master_bus_handle_t i3c_master = handle->base->bus_handle;
    // Save device address before removing from list
    uint8_t device_address = handle->address;

    portENTER_CRITICAL_SAFE(&handle->base->bus_handle->spinlock);
    SLIST_REMOVE(&i3c_master->i2c_device_list, handle, i3c_master_i2c_dev_t, next);
    portEXIT_CRITICAL_SAFE(&handle->base->bus_handle->spinlock);

    // Restore address slot to FREE state
    i3c_master_set_address_slot(i3c_master, device_address, I3C_ADDR_SLOT_FREE);

    if (handle) {
        // Free base device handle first
        if (handle->base) {
            free(handle->base);
        }
        free(handle);
    }
    return ESP_OK;
}

esp_err_t i3c_master_bus_rm_i3c_device(i3c_master_i3c_device_handle_t handle)
{
    ESP_RETURN_ON_FALSE((handle != NULL), ESP_ERR_INVALID_ARG, TAG, "this device is not initialized");

    i3c_master_bus_handle_t i3c_master = handle->base->bus_handle;
    // Save device dynamic address before removing from list
    uint8_t device_dynamic_addr = handle->dynamic_addr;

    portENTER_CRITICAL_SAFE(&handle->base->bus_handle->spinlock);
    SLIST_REMOVE(&i3c_master->i3c_device_list, handle, i3c_master_i3c_dev_t, next);
    portEXIT_CRITICAL_SAFE(&handle->base->bus_handle->spinlock);

    // Restore address slot to FREE state
    i3c_master_set_address_slot(i3c_master, device_dynamic_addr, I3C_ADDR_SLOT_FREE);

    if (handle) {
        if (handle->base) {
            free(handle->base);
        }
        free(handle);
    }
    return ESP_OK;
}

esp_err_t i3c_master_bus_add_i2c_device(i3c_master_bus_handle_t bus_handle, const i3c_device_i2c_config_t *dev_config, i3c_master_i2c_device_handle_t *ret_handle)
{
    ESP_RETURN_ON_FALSE((bus_handle != NULL), ESP_ERR_INVALID_ARG, TAG, "this bus is not initialized, please call `i3c_new_master_bus`");
    ESP_RETURN_ON_FALSE(dev_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(dev_config->scl_freq_hz > 0, ESP_ERR_INVALID_ARG, TAG, "invalid scl frequency");

    i3c_master_i2c_dev_t *i3c_i2c_dev = (i3c_master_i2c_dev_t *)heap_caps_calloc(1, sizeof(i3c_master_i2c_dev_t), MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(i3c_i2c_dev, ESP_ERR_NO_MEM, TAG, "out of memory");

    // Allocate base device handle
    i3c_master_device_t *base_dev = (i3c_master_device_t *)heap_caps_calloc(1, sizeof(i3c_master_device_t), MALLOC_CAP_INTERNAL);
    if (!base_dev) {
        heap_caps_free(i3c_i2c_dev);
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NO_MEM, TAG, "no memory for base device handle");
    }

    // Set up base device
    base_dev->bus_handle = bus_handle;

    // Set up I2C device
    i3c_i2c_dev->base = base_dev;
    i3c_i2c_dev->address = dev_config->device_address;
    i3c_i2c_dev->scl_freq_hz = dev_config->scl_freq_hz;

    // Check if address slot is free before setting it
    uint8_t slot_status = i3c_master_get_address_slot_status(bus_handle, i3c_i2c_dev->address);
    ESP_RETURN_ON_FALSE(slot_status == (uint8_t)I3C_ADDR_SLOT_FREE, ESP_ERR_INVALID_STATE, TAG, "address slot is already in use");

    i3c_master_set_address_slot(bus_handle, i3c_i2c_dev->address, I3C_ADDR_SLOT_I2CDEV);

    *ret_handle = i3c_i2c_dev;

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    SLIST_INSERT_HEAD(&bus_handle->i2c_device_list, i3c_i2c_dev, next);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    return ESP_OK;
}

esp_err_t i3c_master_bus_add_i3c_static_device(i3c_master_bus_handle_t bus_handle, const i3c_device_i3c_config_t *dev_config, i3c_master_i3c_device_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((bus_handle != NULL), ESP_ERR_INVALID_ARG, TAG, "this bus is not initialized, please call `i3c_new_master_bus`");
    ESP_RETURN_ON_FALSE(dev_config  && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    // Declare early to avoid "may be used uninitialized" on error paths
    i3c_master_device_t *base_dev = NULL;

    i3c_master_i3c_dev_t *i3c_i3c_dev = (i3c_master_i3c_dev_t *)heap_caps_calloc(1, sizeof(i3c_master_i3c_dev_t), MALLOC_CAP_INTERNAL);
    ESP_GOTO_ON_FALSE(i3c_i3c_dev, ESP_ERR_NO_MEM, err, TAG, "no memory for i3c device handle");

    // Allocate base device handle
    base_dev = (i3c_master_device_t *)heap_caps_calloc(1, sizeof(i3c_master_device_t), MALLOC_CAP_INTERNAL);
    ESP_GOTO_ON_FALSE(base_dev, ESP_ERR_NO_MEM, err, TAG, "no memory for base device handle");

    // Set up base device
    base_dev->bus_handle = bus_handle;

    // Set up I3C device
    i3c_i3c_dev->base = base_dev;
    i3c_i3c_dev->static_addr = dev_config->static_addr;
    i3c_i3c_dev->dynamic_addr = dev_config->dynamic_addr;

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.i3c_static = {.static_addr = i3c_i3c_dev->static_addr, .dynamic_addr = i3c_i3c_dev->dynamic_addr, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I3C,}},
    };

    i3c_master_ll_command_descriptor_t command[1] = {
        {.cmd_l.addr = {.cmd_attr = 0x2, .tid = 0x0, .cmd = I3C_CCC_SETDASA, .dev_indx = 0x0, .dev_cnt = 0x1, .roc = 0x1, .toc = 0x1}, .cmd_h.val = 0x0}, // Set Dynamic Address from Static Address
    };

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    i3c_master_ll_reset_command_buf(bus_handle->hal.dev);
    i3c_master_ll_set_device_address_table(bus_handle->hal.dev, addr_table, 1);
    i3c_master_ll_set_command(bus_handle->hal.dev, command, 1);
    i3c_master_ll_start_transaction(bus_handle->hal.dev);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    i3c_master_event_t event;
    if (xQueueReceive(bus_handle->event_queue, &event, portMAX_DELAY) == pdFALSE) {
        ESP_GOTO_ON_FALSE(false, ESP_ERR_TIMEOUT, err, TAG, "event queue wait timeout.");
    }

    // Check if address slot is free before setting it
    uint8_t slot_status = i3c_master_get_address_slot_status(bus_handle, i3c_i3c_dev->dynamic_addr);
    ESP_GOTO_ON_FALSE(slot_status == (uint8_t)I3C_ADDR_SLOT_FREE, ESP_ERR_INVALID_STATE, err, TAG, "address slot is already in use");

    i3c_master_set_address_slot(bus_handle, i3c_i3c_dev->dynamic_addr, I3C_ADDR_SLOT_I3CDEV);

    *ret_handle = i3c_i3c_dev;

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    SLIST_INSERT_HEAD(&bus_handle->i3c_device_list, i3c_i3c_dev, next);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    return ESP_OK;

err:
    if (i3c_i3c_dev) {
        heap_caps_free(i3c_i3c_dev);
    }
    if (base_dev) {
        heap_caps_free(base_dev);
    }
    return ret;
}

/**
 * @brief Calculate parity bit for I3C dynamic address
 *
 * According to I3C specification, the parity bit is calculated by performing
 * XOR operation on all 7 bits of the dynamic address.
 *
 * @param dynamic_addr 7-bit dynamic address (valid range: 0x01-0x7F, excluding reserved addresses)
 * @return uint8_t Parity bit (0 or 1)
 */
static inline uint8_t i3c_calculate_dynamic_address_parity(uint8_t dynamic_addr)
{
    uint8_t temp_addr = dynamic_addr & 0x7F;
    return (~__builtin_parity(temp_addr)) & 0x01;  // Odd parity
}

esp_err_t i3c_master_scan_devices_by_entdaa(i3c_master_bus_handle_t bus_handle, i3c_master_i3c_device_table_handle_t *ret_table_handle)
{
    ESP_RETURN_ON_FALSE(bus_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid bus handle");
    ESP_RETURN_ON_FALSE(ret_table_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "ret_table_handle pointer is NULL");
    ESP_RETURN_ON_FALSE(bus_handle->entdaa_device_num > 0, ESP_ERR_INVALID_ARG, TAG, "entdaa_device_num is 0");

    esp_err_t ret = ESP_OK;
    i3c_master_i3c_device_handle_t *temp_devices = NULL;
    size_t temp_count = 0;
    size_t max_devices = bus_handle->entdaa_device_num;
    i3c_master_i3c_device_table_handle_t table_handle = NULL;

    temp_devices = heap_caps_calloc(max_devices, sizeof(i3c_master_i3c_device_handle_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(temp_devices, ESP_ERR_NO_MEM, TAG, "no memory for device handles array");

    // Pointers for the current iteration, used for cleanup on failure
    i3c_master_i3c_dev_t *new_device = NULL;
    i3c_master_device_t *loop_base_dev = NULL;
    uint8_t valid_addr = 0;  // Address slot used in current iteration, needed for error recovery

    for (size_t i = 0; i < max_devices; i++) {
        new_device = NULL;
        loop_base_dev = NULL;
        valid_addr = 0;
        ESP_GOTO_ON_ERROR(i3c_master_get_valid_address_slot(bus_handle, 0x1, &valid_addr), err, TAG, "free address slot is not found.");

        uint8_t parity = i3c_calculate_dynamic_address_parity(valid_addr);
        ESP_LOGD(TAG, "parity: %d", parity);

        i3c_master_ll_device_address_descriptor_t addr_table[1] = {
            {.dynamic = {.dynamic_addr = valid_addr, .par = parity, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I3C,}}, // address should use address slot
        };

        i3c_master_ll_command_descriptor_t command[1] = {
            {.cmd_l.addr = {.cmd_attr = 0x2, .tid = 0x0, .cmd = I3C_CCC_ENTDAA, .dev_indx = 0x0, .dev_cnt = 0x1, .roc = 0x0, .toc = 0x1}, .cmd_h.val = 0x0}, // Enter Dynamic Address Assignment
        };

        portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
        i3c_master_ll_reset_command_buf(bus_handle->hal.dev);
        i3c_master_ll_set_device_address_table(bus_handle->hal.dev, addr_table, 1);
        i3c_master_ll_set_command(bus_handle->hal.dev, command, 1);
        i3c_master_ll_start_transaction(bus_handle->hal.dev);
        portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

        i3c_master_event_t event;
        if (xQueueReceive(bus_handle->event_queue, &event, -1) == pdFALSE) {
            ESP_GOTO_ON_FALSE(NULL, ESP_ERR_TIMEOUT, err, TAG, "event queue wait timeout.");
        }

        i3c_master_ll_response_descriptor_t response_data;
        response_data = i3c_master_ll_get_response_data(bus_handle->hal.dev);

        // Check if NACK event occurred or error status is set
        if (event == I3C_MASTER_EVENT_NACK || response_data.err_sts != 0) {
            // No device responded or error occurred, restore address slot and break
            i3c_master_set_address_slot(bus_handle, valid_addr, I3C_ADDR_SLOT_FREE);
            break;
        }

        // Only set address slot when transaction is done successfully
        i3c_master_set_address_slot(bus_handle, valid_addr, I3C_ADDR_SLOT_I3CDEV);

        i3c_master_ll_device_char_descriptor_t dev_char_table;
        i3c_master_ll_get_dev_char_table(bus_handle->hal.dev, &dev_char_table, i);

        // 2.6 Allocate a new I3C device handle
        new_device = heap_caps_calloc(1, sizeof(i3c_master_i3c_dev_t), MALLOC_CAP_INTERNAL);
        ESP_GOTO_ON_FALSE(new_device, ESP_ERR_NO_MEM, err_free_current, TAG, "no memory for new device handle");

        // Allocate base device handle
        loop_base_dev = heap_caps_calloc(1, sizeof(i3c_master_device_t), MALLOC_CAP_INTERNAL);
        ESP_GOTO_ON_FALSE(loop_base_dev, ESP_ERR_NO_MEM, err_free_current, TAG, "no memory for base device handle");

        // 2.7 Parse device information
        loop_base_dev->bus_handle = bus_handle;
        new_device->base = loop_base_dev;
        new_device->pid = dev_char_table.pid;
        new_device->bcr = dev_char_table.bcr;
        new_device->dcr = dev_char_table.dcr;
        new_device->dynamic_addr = (dev_char_table.dynamic_addr >> 1);

        portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
        SLIST_INSERT_HEAD(&bus_handle->i3c_device_list, new_device, next);
        portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

        temp_devices[temp_count] = new_device;
        temp_count++;

        ESP_LOGI(TAG, "ENTDAA finds device #%zu: PID=0x%08llX, BCR=0x%02X, DCR=0x%02X, Dynamic Address=0x%02X",
                 temp_count, new_device->pid, new_device->bcr, new_device->dcr, new_device->dynamic_addr);
        // After finishing this iteration, prevent double free on error paths
        new_device = NULL;
        loop_base_dev = NULL;
    }

    if (temp_count > 0) {
        // Reallocate the array to match the actual number of discovered devices
        i3c_master_i3c_device_handle_t *final_devices = heap_caps_realloc(temp_devices, temp_count * sizeof(i3c_master_i3c_device_handle_t), MALLOC_CAP_DEFAULT);
        ESP_GOTO_ON_FALSE(final_devices != NULL, ESP_ERR_NO_MEM, err, TAG, "realloc failed");
        temp_devices = final_devices;  // Update temp_devices for error handling

        // Allocate the device table structure
        table_handle = heap_caps_calloc(1, sizeof(i3c_master_i3c_device_table_t), MALLOC_CAP_DEFAULT);
        ESP_GOTO_ON_FALSE(table_handle != NULL, ESP_ERR_NO_MEM, err, TAG, "no memory for device table handle");

        table_handle->devices = final_devices;
        table_handle->device_count = temp_count;
        table_handle->bus_handle = bus_handle;  // Save bus handle for later validation
        *ret_table_handle = table_handle;
        ESP_LOGI(TAG, "ENTDAA finishes, find %d I3C devices", temp_count);
        ret = ESP_OK;
    } else {
        // no device found, free the temp_devices
        heap_caps_free(temp_devices);

        ESP_LOGI(TAG, "ENTDAA finishes, no I3C devices is found");
    }

    return ret;

err_free_current:
    if (new_device) {
        heap_caps_free(new_device);
        new_device = NULL;
    }
    if (loop_base_dev) {
        heap_caps_free(loop_base_dev);
        loop_base_dev = NULL;
    }

err:
    if (table_handle) {
        heap_caps_free(table_handle);
    }
    if (temp_devices) {
        // Rollback devices that have been created and inserted before failure
        for (size_t j = 0; j < temp_count; j++) {
            if (temp_devices[j]) {
                i3c_master_bus_rm_i3c_device(temp_devices[j]);
            }
        }
        heap_caps_free(temp_devices);
    }
    return ret;

}

esp_err_t i3c_master_i3c_device_get_info(i3c_master_i3c_device_handle_t dev_handle, i3c_device_information_t *info)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid device handle");
    ESP_RETURN_ON_FALSE(info != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid info pointer");

    info->pid = dev_handle->pid;
    info->bcr = dev_handle->bcr;
    info->dcr = dev_handle->dcr;
    info->dynamic_addr = dev_handle->dynamic_addr;

    return ESP_OK;
}

esp_err_t i3c_master_free_device_handle_table(i3c_master_i3c_device_table_handle_t table_handle)
{
    ESP_RETURN_ON_FALSE(table_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid device table handle");

    heap_caps_free(table_handle);
    return ESP_OK;
}

esp_err_t i3c_master_get_device_count(i3c_master_i3c_device_table_handle_t table_handle, size_t *device_count)
{
    ESP_RETURN_ON_FALSE(table_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid device table handle");
    ESP_RETURN_ON_FALSE(device_count != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid device_count pointer");

    *device_count = table_handle->device_count;
    return ESP_OK;
}

esp_err_t i3c_master_get_device_handle(i3c_master_i3c_device_table_handle_t table_handle, size_t index, i3c_master_i3c_device_handle_t *ret_handle)
{
    ESP_RETURN_ON_FALSE(table_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid device table handle");
    ESP_RETURN_ON_FALSE(ret_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid ret_handle pointer");
    ESP_RETURN_ON_FALSE(table_handle->devices != NULL, ESP_ERR_INVALID_STATE, TAG, "device table is empty");
    ESP_RETURN_ON_FALSE(index < table_handle->device_count, ESP_ERR_INVALID_ARG, TAG, "index out of range");

    *ret_handle = table_handle->devices[index];
    return ESP_OK;
}

esp_err_t i3c_master_transfer_ccc(i3c_master_bus_handle_t bus_handle, const i3c_master_ccc_transfer_config_t *ccc_trans)
{
    ESP_RETURN_ON_FALSE(bus_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid bus handle");
    ESP_RETURN_ON_FALSE(ccc_trans != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid ccc structure");

    uint8_t parity = i3c_calculate_dynamic_address_parity(ccc_trans->device_address);

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.dynamic = {.dynamic_addr = ccc_trans->device_address, .par = parity, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I3C,}},
    };

    i3c_master_ll_command_descriptor_t command[1] = {
        {.cmd_l.regular = {.cmd_attr = 0x0, .tid = 0x0, .cmd = ccc_trans->ccc_command, .cp = 0x1, .dev_indx = 0x0, .mode = 0x0, .rnw = (ccc_trans->direction == I3C_MASTER_TRANSFER_DIRECTION_READ) ? I3C_MASTER_LL_TRANSFER_DIR_READ : I3C_MASTER_LL_TRANSFER_DIR_WRITE, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = ccc_trans->data_size}},
    };

    portENTER_CRITICAL_SAFE(&bus_handle->spinlock);
    i3c_master_ll_reset_command_buf(bus_handle->hal.dev);
    i3c_master_ll_set_device_address_table(bus_handle->hal.dev, addr_table, 1);
    i3c_master_ll_set_command(bus_handle->hal.dev, command, 1);
    portEXIT_CRITICAL_SAFE(&bus_handle->spinlock);

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 1,
        .write_buffer = (ccc_trans->direction == I3C_MASTER_TRANSFER_DIRECTION_WRITE) ? (uint8_t *)ccc_trans->data : NULL,
        .read_buffer = (ccc_trans->direction == I3C_MASTER_TRANSFER_DIRECTION_READ) ? ccc_trans->data : NULL,
        .scl_freq_hz = 0,
        .dev_handle = NULL,
    };

    ESP_RETURN_ON_ERROR(do_fifo_transaction_handler(bus_handle, &trans_desc), TAG, "FIFO transaction handler failed");

    i3c_master_event_t event;
    if (xQueueReceive(bus_handle->event_queue, &event, -1) == pdFALSE) {
        ESP_RETURN_ON_FALSE(NULL, ESP_ERR_TIMEOUT, TAG, "event queue wait timeout.");
    }

    // Check if NACK event occurred
    if (event == I3C_MASTER_EVENT_NACK) {
        ESP_LOGE(TAG, "NACK event occurred");
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}

static esp_err_t do_async_transaction(i3c_master_device_handle_t dev_handle, i3c_transaction_desc_t *trans_desc)
{
    i3c_master_bus_t *bus_handle = dev_handle->bus_handle;

    memcpy(bus_handle->i3c_async_addr_table[bus_handle->ops_prepare_idx], trans_desc->addr_table, trans_desc->addr_table_num * sizeof(i3c_master_ll_device_address_descriptor_t));
    memcpy(bus_handle->i3c_async_command_table[bus_handle->ops_prepare_idx], trans_desc->command_table, trans_desc->command_table_num * sizeof(i3c_master_ll_command_descriptor_t));

    i3c_transaction_desc_t *t = NULL;

    if (xQueueReceive(bus_handle->trans_queues[I3C_TRANS_QUEUE_READY], &t, 0) != pdTRUE) {
        if (xQueueReceive(bus_handle->trans_queues[I3C_TRANS_QUEUE_COMPLETE], &t, 0) == pdTRUE) {
            assert(t != NULL);
            bus_handle->num_trans_inflight--;
        }
    }

    memset(t, 0, sizeof(i3c_transaction_desc_t));
    t->addr_table = bus_handle->i3c_async_addr_table[bus_handle->ops_prepare_idx];
    t->addr_table_num = trans_desc->addr_table_num;
    t->command_table = bus_handle->i3c_async_command_table[bus_handle->ops_prepare_idx];
    t->command_table_num = trans_desc->command_table_num;
    t->write_buffer = trans_desc->write_buffer;
    t->read_buffer = trans_desc->read_buffer;
    t->scl_freq_hz = trans_desc->scl_freq_hz;
    t->dev_handle = dev_handle;

    ESP_RETURN_ON_FALSE(xQueueSend(bus_handle->trans_queues[I3C_TRANS_QUEUE_PROGRESS], &t, 0) == pdTRUE, ESP_ERR_NO_MEM, TAG, "i3c-i2c transaction queue full");
    bus_handle->ops_prepare_idx = (bus_handle->ops_prepare_idx + 1) % bus_handle->queue_depth;
    bus_handle->num_trans_inflight++;

    i3c_fsm_t expected_fsm = I3C_FSM_ENABLE;
    esp_err_t ret = ESP_OK;
    if (atomic_compare_exchange_strong(&bus_handle->fsm, &expected_fsm, I3C_FSM_WAIT)) {
        if (xQueueReceive(bus_handle->trans_queues[I3C_TRANS_QUEUE_PROGRESS], &t, 0)) {
            atomic_store(&bus_handle->fsm, I3C_FSM_RUN);
            ret = bus_handle->transaction_handler(bus_handle, t);
            (void)ret;
            assert(ret == ESP_OK);
        } else {
            atomic_store(&bus_handle->fsm, I3C_FSM_ENABLE);
        }
    }

    return ESP_OK;
}

/**
* This function handles the preparation and execution of an I3C or I2C transaction on the I3C master bus.
* Depending on the configuration, the transaction can be handled asynchronously, using DMA, or via FIFO.
* It also manages the bus locking mechanism and power management (PM) locks, ensuring thread safety
* and power optimization during the transaction.
*/
static esp_err_t i3c_master_prepare_transaction(i3c_master_device_handle_t base_handle, i3c_transaction_desc_t *trans_desc, int xfer_timeout_ms)
{
    esp_err_t ret = ESP_OK;
    TickType_t ticks_to_wait = (xfer_timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(xfer_timeout_ms);

    // Check if async transaction requires DMA
    if (base_handle->bus_handle->async_transaction && !base_handle->bus_handle->dma_initialized) {
        ESP_LOGE(TAG, "Async transaction requires DMA, please call `i3c_master_bus_decorate_dma()` first");
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(base_handle->bus_handle->bus_lock_mux, ticks_to_wait) != pdTRUE) {
        ESP_LOGE(TAG, "I3C software timeout");
        return ESP_ERR_TIMEOUT;
    }

#if CONFIG_PM_ENABLE
    if (base_handle->bus_handle->pm_lock) {
        ESP_GOTO_ON_ERROR(esp_pm_lock_acquire(base_handle->bus_handle->pm_lock), err, TAG, "acquire pm_lock failed");
    }
#endif

    if (base_handle->bus_handle->async_transaction) {
        ESP_GOTO_ON_ERROR(do_async_transaction(trans_desc->dev_handle, trans_desc), err, TAG, "Prepare async transaction failed");
    } else {
        ESP_GOTO_ON_ERROR(base_handle->bus_handle->transaction_handler(base_handle->bus_handle, trans_desc), err, TAG, "transaction handler failed");
    }

    if (base_handle->bus_handle->async_transaction == false) {
        i3c_master_event_t event;
        if (xQueueReceive(base_handle->bus_handle->event_queue, &event, ticks_to_wait) == pdFALSE) {
            ESP_GOTO_ON_FALSE(NULL, ESP_ERR_TIMEOUT, err, TAG, "event queue wait timeout. Please check whether stretch happened on i3c bus");
        } else {
            if (event ==  I3C_MASTER_EVENT_NACK) {
                ESP_LOGD(TAG, "%s(%d): i3c-i2c nack detected", __FUNCTION__, __LINE__);
                ret = ESP_ERR_INVALID_STATE;
                goto err;
            }
            if (event == I3C_MASTER_EVENT_TRANS_DONE) {
                if (base_handle->bus_handle->use_dma_transaction) {
                    if (trans_desc->read_buffer) {
                        size_t dma_rcv_size = gdma_link_count_buffer_size_till_eof(base_handle->bus_handle->rx_dma_link, 0);
                        size_t c2m_aligned_size = I3C_ALIGN_UP(dma_rcv_size, base_handle->bus_handle->cache_line_size);
                        ESP_GOTO_ON_ERROR(esp_cache_msync(trans_desc->read_buffer, c2m_aligned_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C), err, TAG, "memory sync failed");
                    }
                }
            }
        }

    }

#if CONFIG_PM_ENABLE
    if (base_handle->bus_handle->pm_lock) {
        ESP_GOTO_ON_ERROR(esp_pm_lock_release(base_handle->bus_handle->pm_lock), err, TAG, "release pm_lock failed");
    }
#endif
    xSemaphoreGive(base_handle->bus_handle->bus_lock_mux);
    return ret;

err:
    xSemaphoreGive(base_handle->bus_handle->bus_lock_mux);
    return ret;
}

esp_err_t i3c_master_i2c_device_transmit(i3c_master_i2c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c-i2c handle not initialized");
    ESP_RETURN_ON_FALSE((write_buffer != NULL) && (write_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c-i2c transmit buffer or size invalid");

    if (dev_handle->base->bus_handle->use_dma_transaction) {
        ESP_RETURN_ON_FALSE((((uintptr_t)write_buffer & (I3C_MASTER_DMA_INTERFACE_ALIGNMENT - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, write buffer address must be %d bytes aligned", I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
    }

    i3c_master_ll_i2c_speed_mode_t i2c_work_mode = (dev_handle->scl_freq_hz > 400 * 1000) ? I3C_MASTER_LL_I2C_FAST_MODE_PLUS : I3C_MASTER_LL_I2C_FAST_MODE;

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.i2c_static = {.static_addr = dev_handle->address, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I2C,}},
    };

    i3c_master_ll_command_descriptor_t command[1] = {
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x0, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = i2c_work_mode, .rnw = I3C_MASTER_LL_TRANSFER_DIR_WRITE, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = write_size}},
    };

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 1,
        .write_buffer = (uint8_t *)write_buffer,
        .read_buffer = NULL,
        .scl_freq_hz = dev_handle->scl_freq_hz,
        .dev_handle = dev_handle->base,
        .i2c_trans = true,
    };

    return i3c_master_prepare_transaction(dev_handle->base, &trans_desc, xfer_timeout_ms);
}

esp_err_t i3c_master_i3c_device_transmit(i3c_master_i3c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c handle not initialized");
    ESP_RETURN_ON_FALSE((write_buffer != NULL) && (write_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c transmit buffer or size invalid");

    if (dev_handle->base->bus_handle->use_dma_transaction) {
        ESP_RETURN_ON_FALSE((((uintptr_t)write_buffer & (I3C_MASTER_DMA_INTERFACE_ALIGNMENT - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, write buffer address must be %d bytes aligned", I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
    }
    uint8_t parity = i3c_calculate_dynamic_address_parity(dev_handle->dynamic_addr);

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.dynamic = {.dynamic_addr = dev_handle->dynamic_addr, .par = parity, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I3C,}},
    };

    i3c_master_ll_command_descriptor_t command[1] = {
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x0, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = 0, .rnw = I3C_MASTER_LL_TRANSFER_DIR_WRITE, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = write_size}},
    };

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 1,
        .write_buffer = (uint8_t *)write_buffer,
        .read_buffer = NULL,
        .scl_freq_hz = 0,
        .dev_handle = dev_handle->base,
        .i2c_trans = false,
    };

    portENTER_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);
    i3c_master_ll_inject_broadcast_address_head(dev_handle->base->bus_handle->hal.dev, true);
    portEXIT_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);

    return i3c_master_prepare_transaction(dev_handle->base, &trans_desc, xfer_timeout_ms);
}

esp_err_t i3c_master_i2c_device_receive(i3c_master_i2c_device_handle_t dev_handle, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c-i2c handle not initialized");
    ESP_RETURN_ON_FALSE((read_buffer != NULL) && (read_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c-i2c receive buffer or size invalid");

    if (dev_handle->base->bus_handle->use_dma_transaction) {
        ESP_RETURN_ON_FALSE((((uintptr_t)read_buffer & (dev_handle->base->bus_handle->cache_line_size - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, read buffer address must be %d bytes aligned", dev_handle->base->bus_handle->cache_line_size);
    }

    i3c_master_ll_i2c_speed_mode_t i2c_work_mode = (dev_handle->scl_freq_hz > 400 * 1000) ? I3C_MASTER_LL_I2C_FAST_MODE_PLUS : I3C_MASTER_LL_I2C_FAST_MODE;

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.i2c_static = {.static_addr = dev_handle->address, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I2C,}},
    };

    i3c_master_ll_command_descriptor_t command[1] = {
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x0, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = i2c_work_mode, .rnw = I3C_MASTER_LL_TRANSFER_DIR_READ, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = read_size}},
    };

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 1,
        .write_buffer = NULL,
        .read_buffer = read_buffer,
        .scl_freq_hz = dev_handle->scl_freq_hz,
        .dev_handle = dev_handle->base,
        .i2c_trans = true,
    };

    return i3c_master_prepare_transaction(dev_handle->base, &trans_desc, xfer_timeout_ms);
}

esp_err_t i3c_master_i3c_device_receive(i3c_master_i3c_device_handle_t dev_handle, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c handle not initialized");
    ESP_RETURN_ON_FALSE((read_buffer != NULL) && (read_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c receive buffer or size invalid");

    if (dev_handle->base->bus_handle->use_dma_transaction) {
        ESP_RETURN_ON_FALSE((((uintptr_t)read_buffer & (dev_handle->base->bus_handle->cache_line_size - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, read buffer address must be %d bytes aligned", dev_handle->base->bus_handle->cache_line_size);
    }

    uint8_t parity = i3c_calculate_dynamic_address_parity(dev_handle->dynamic_addr);

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.dynamic = {.dynamic_addr = dev_handle->dynamic_addr, .par = parity, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I3C,}},
    };

    i3c_master_ll_command_descriptor_t command[1] = {
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x0, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = 0, .rnw = I3C_MASTER_LL_TRANSFER_DIR_READ, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = read_size}},
    };

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 1,
        .write_buffer = NULL,
        .read_buffer = read_buffer,
        .scl_freq_hz = 0,
        .dev_handle = dev_handle->base,
        .i2c_trans = false,
    };

    portENTER_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);
    i3c_master_ll_inject_broadcast_address_head(dev_handle->base->bus_handle->hal.dev, true);
    portEXIT_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);

    return i3c_master_prepare_transaction(dev_handle->base, &trans_desc, xfer_timeout_ms);
}

esp_err_t i3c_master_i2c_device_transmit_receive(i3c_master_i2c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c-i2c handle not initialized");
    ESP_RETURN_ON_FALSE((write_buffer != NULL) && (write_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c-i2c transmit buffer or size invalid");
    ESP_RETURN_ON_FALSE((read_buffer != NULL) && (read_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c-i2c receive buffer or size invalid");

    if (dev_handle->base->bus_handle->use_dma_transaction) {
        ESP_RETURN_ON_FALSE((((uintptr_t)read_buffer & (dev_handle->base->bus_handle->cache_line_size - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, read buffer address must be %d bytes aligned", dev_handle->base->bus_handle->cache_line_size);
        ESP_RETURN_ON_FALSE((((uintptr_t)write_buffer & (I3C_MASTER_DMA_INTERFACE_ALIGNMENT - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, write buffer address must be %d bytes aligned", I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
    }

    i3c_master_ll_i2c_speed_mode_t i2c_work_mode = (dev_handle->scl_freq_hz > 400 * 1000) ? I3C_MASTER_LL_I2C_FAST_MODE_PLUS : I3C_MASTER_LL_I2C_FAST_MODE;

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.i2c_static = {.static_addr = dev_handle->address, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I2C,}},
    };

    i3c_master_ll_command_descriptor_t command[2] = {
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x0, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = i2c_work_mode, .rnw = I3C_MASTER_LL_TRANSFER_DIR_WRITE, .roc = 0x1, .toc = 0x0}, .cmd_h.regular = {.dl = write_size}},
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x1, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = i2c_work_mode, .rnw = I3C_MASTER_LL_TRANSFER_DIR_READ, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = read_size}},
    };

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 2,
        .write_buffer = (uint8_t *)write_buffer,
        .read_buffer = read_buffer,
        .scl_freq_hz = dev_handle->scl_freq_hz,
        .dev_handle = dev_handle->base,
        .i2c_trans = true,
    };

    return i3c_master_prepare_transaction(dev_handle->base, &trans_desc, xfer_timeout_ms);
}

esp_err_t i3c_master_i3c_device_transmit_receive(i3c_master_i3c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c handle not initialized");
    ESP_RETURN_ON_FALSE((write_buffer != NULL) && (write_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c transmit buffer or size invalid");
    ESP_RETURN_ON_FALSE((read_buffer != NULL) && (read_size > 0), ESP_ERR_INVALID_ARG, TAG, "i3c receive buffer or size invalid");

    if (dev_handle->base->bus_handle->use_dma_transaction) {
        ESP_RETURN_ON_FALSE((((uintptr_t)read_buffer & (dev_handle->base->bus_handle->cache_line_size - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, read buffer address must be %d bytes aligned", dev_handle->base->bus_handle->cache_line_size);
        ESP_RETURN_ON_FALSE((((uintptr_t)write_buffer & (I3C_MASTER_DMA_INTERFACE_ALIGNMENT - 1)) == 0), ESP_ERR_INVALID_ARG, TAG, "when dma transaction, write buffer address must be %d bytes aligned", I3C_MASTER_DMA_INTERFACE_ALIGNMENT);
    }

    uint8_t parity = i3c_calculate_dynamic_address_parity(dev_handle->dynamic_addr);

    i3c_master_ll_device_address_descriptor_t addr_table[1] = {
        {.dynamic = {.dynamic_addr = dev_handle->dynamic_addr, .par = parity, .dnrc = 0x0, .mode = I3C_MASTER_LL_MODE_I3C,}},
    };

    i3c_master_ll_command_descriptor_t command[2] = {
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x0, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = 0, .rnw = I3C_MASTER_LL_TRANSFER_DIR_WRITE, .roc = 0x1, .toc = 0x0}, .cmd_h.regular = {.dl = write_size}},
        {.cmd_l.regular = {.cmd_attr = I3C_MASTER_LL_COMMAND_REGULAR, .tid = 0x1, .cmd = 0x0, .cp = 0x0, .dev_indx = 0x0, .mode = 0, .rnw = I3C_MASTER_LL_TRANSFER_DIR_READ, .roc = 0x1, .toc = 0x1}, .cmd_h.regular = {.dl = read_size}},
    };

    i3c_transaction_desc_t trans_desc = {
        .addr_table = addr_table,
        .addr_table_num = 1,
        .command_table = command,
        .command_table_num = 2,
        .write_buffer = (uint8_t *)write_buffer,
        .read_buffer = read_buffer,
        .scl_freq_hz = 0,
        .dev_handle = dev_handle->base,
        .i2c_trans = false,
    };

    portENTER_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);
    i3c_master_ll_inject_broadcast_address_head(dev_handle->base->bus_handle->hal.dev, true);
    portEXIT_CRITICAL_SAFE(&dev_handle->base->bus_handle->spinlock);

    return i3c_master_prepare_transaction(dev_handle->base, &trans_desc, xfer_timeout_ms);
}

esp_err_t i3c_master_i2c_device_register_event_callbacks(i3c_master_i2c_device_handle_t dev_handle, const i3c_master_i2c_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c-i2c handle not initialized");

    if (dev_handle->base->bus_handle->async_transaction == false) {
        ESP_LOGE(TAG, "async transaction is not enabled, callback is not needed");
        return ESP_ERR_INVALID_STATE;
    }

#if CONFIG_I3C_MASTER_ISR_CACHE_SAFE
    if (cbs->on_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_done), ESP_ERR_INVALID_ARG, TAG, "i3c trans done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_I3C_MASTER_ISR_CACHE_SAFE

    dev_handle->on_trans_done = cbs->on_trans_done;
    dev_handle->user_ctx = user_data;

    return ESP_OK;
}

esp_err_t i3c_master_i3c_device_register_event_callbacks(i3c_master_i3c_device_handle_t dev_handle, const i3c_master_i3c_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i3c handle not initialized");

#if CONFIG_I3C_MASTER_ISR_CACHE_SAFE
    if (cbs->on_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_done), ESP_ERR_INVALID_ARG, TAG, "i3c trans done callback not in IRAM");
    }
    if (cbs->on_ibi) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_ibi), ESP_ERR_INVALID_ARG, TAG, "i3c ibi callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_I3C_MASTER_ISR_CACHE_SAFE

    dev_handle->on_trans_done = cbs->on_trans_done;
    dev_handle->on_ibi = cbs->on_ibi;
    dev_handle->user_ctx = user_data;

    return ESP_OK;
}

esp_err_t i3c_master_bus_wait_all_done(i3c_master_bus_handle_t bus_handle, int timeout_ms)
{
    ESP_RETURN_ON_FALSE(bus_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    TickType_t wait_ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    i3c_transaction_desc_t *t = NULL;

    size_t cnt = bus_handle->num_trans_inflight;
    for (size_t i = 0; i < cnt; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(bus_handle->trans_queues[I3C_TRANS_QUEUE_COMPLETE], &t, wait_ticks) == pdTRUE,
                            ESP_ERR_TIMEOUT, TAG, "flush timeout");
        ESP_RETURN_ON_FALSE(xQueueSend(bus_handle->trans_queues[I3C_TRANS_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        bus_handle->num_trans_inflight--;
    }
    return ESP_OK;
}

#if CONFIG_I3C_MASTER_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void i3c_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
