/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "esp_intr_alloc.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_private/uhci.h"
#include "esp_private/uhci_types.h"
#include "soc/uhci_periph.h"
#include "soc/soc_caps.h"
#include "hal/uhci_hal.h"
#include "hal/uhci_ll.h"
#include "hal/dma_types.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/gdma.h"
#include "esp_private/esp_cache_private.h"
#include "esp_memory_utils.h"
#include "esp_cache.h"
#include "uhci_private.h"
#if CONFIG_UHCI_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"

static const char* TAG = "uhci";

#define UHCI_DMA_DESC_ALIGN      4

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

    if (uhci_ctrl->pm_lock) {
        esp_pm_lock_release(uhci_ctrl->pm_lock);
    }

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


static esp_err_t uhci_gdma_initialize(uhci_controller_handle_t uhci_ctrl, const uhci_controller_config_t *config)
{
    // Initialize DMA TX channel
    gdma_channel_alloc_config_t tx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
#if CONFIG_UHCI_ISR_CACHE_SAFE
        .flags.isr_cache_safe = true,
#endif
    };
    ESP_RETURN_ON_ERROR(gdma_new_ahb_channel(&tx_alloc_config, &uhci_ctrl->tx_dir.dma_chan), TAG, "DMA tx channel alloc failed");
    gdma_connect(uhci_ctrl->tx_dir.dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));

    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    gdma_apply_strategy(uhci_ctrl->tx_dir.dma_chan, &strategy_config);

    uhci_ctrl->tx_dir.ext_mem_align = 4;
    uhci_ctrl->tx_dir.int_mem_align = 1;
    // create DMA link list
    uhci_ctrl->tx_dir.dma_nodes = heap_caps_aligned_calloc(UHCI_DMA_DESC_ALIGN, 3, sizeof(uhci_dma_descriptor_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    assert(uhci_ctrl->tx_dir.dma_nodes);

    // Register callbacks
    gdma_tx_event_callbacks_t tx_cbk = {
        .on_trans_eof = uhci_gdma_tx_callback_eof,
    };
    ESP_RETURN_ON_ERROR(gdma_register_tx_event_callbacks(uhci_ctrl->tx_dir.dma_chan, &tx_cbk, uhci_ctrl), TAG, "register DMA callbacks failed");

    return ESP_OK;

}

static esp_err_t uhci_gdma_deinitialize(uhci_controller_handle_t uhci_ctrl)
{
    if (uhci_ctrl->tx_dir.dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_disconnect(uhci_ctrl->tx_dir.dma_chan), TAG, "disconnect tx dma channel failed");
        ESP_RETURN_ON_ERROR(gdma_del_channel(uhci_ctrl->tx_dir.dma_chan), TAG, "delete tx dma channel failed");
    }

    if (uhci_ctrl->tx_dir.dma_nodes != NULL) {
        heap_caps_free(uhci_ctrl->tx_dir.dma_nodes);
    }

    return ESP_OK;
}

static void uhci_do_transmit(uhci_controller_handle_t uhci_ctrl, uhci_transaction_desc_t *trans)
{
    uhci_ctrl->tx_dir.cur_trans = trans;

    if (trans->buffer_size > 3 * DMA_DESCRIPTOR_BUFFER_MAX_SIZE) {
        ESP_LOGE(TAG, "buffer size is greater than 3 * 4095 bytes");
        abort();
    }

    int node_count = (trans->buffer_size + DMA_DESCRIPTOR_BUFFER_MAX_SIZE - 1) / DMA_DESCRIPTOR_BUFFER_MAX_SIZE;

    size_t remaining_size = trans->buffer_size;
    uint8_t *buffer_ptr = trans->buffer;

    for (int i = 0; i < node_count; i++) {
        size_t current_size = (remaining_size > DMA_DESCRIPTOR_BUFFER_MAX_SIZE) ? DMA_DESCRIPTOR_BUFFER_MAX_SIZE : remaining_size;

        uhci_ctrl->tx_dir.dma_nodes[i].buffer = buffer_ptr;
        uhci_ctrl->tx_dir.dma_nodes[i].dw0.size = current_size;
        uhci_ctrl->tx_dir.dma_nodes[i].dw0.length = current_size;
        uhci_ctrl->tx_dir.dma_nodes[i].dw0.owner = 1;

        if (i == node_count - 1) {
            // last node
            uhci_ctrl->tx_dir.dma_nodes[i].dw0.suc_eof = 1;
            uhci_ctrl->tx_dir.dma_nodes[i].next = NULL;
        } else {
            uhci_ctrl->tx_dir.dma_nodes[i].dw0.suc_eof = 0;
            uhci_ctrl->tx_dir.dma_nodes[i].next = &uhci_ctrl->tx_dir.dma_nodes[i + 1];
        }

        buffer_ptr += current_size;
        remaining_size -= current_size;
    }

    // acquire power manager lock
    if (uhci_ctrl->pm_lock) {
        esp_pm_lock_acquire(uhci_ctrl->pm_lock);
    }
    gdma_start(uhci_ctrl->tx_dir.dma_chan, (intptr_t)uhci_ctrl->tx_dir.dma_nodes);
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

    if (uhci_ctrl->tx_dir.tx_fsm != UHCI_TX_FSM_ENABLE) {
        ESP_LOGE(TAG, "TX transaction is not finished, delete controller failed");
        return ESP_ERR_INVALID_STATE;
    }

    uhci_ll_enable_bus_clock(uhci_ctrl->uhci_num, false);

    for (int i = 0; i < UHCI_TRANS_QUEUE_MAX; i++) {
        if (uhci_ctrl->tx_dir.trans_queues[i]) {
            vQueueDeleteWithCaps(uhci_ctrl->tx_dir.trans_queues[i]);
        }
    }

    if (uhci_ctrl->tx_dir.trans_desc_pool) {
        heap_caps_free(uhci_ctrl->tx_dir.trans_desc_pool);
    }

    if (uhci_ctrl->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(uhci_ctrl->pm_lock), TAG, "delete rx pm_lock failed");
    }

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

    uhci_ll_enable_bus_clock(uhci_ctrl->uhci_num, true);
    uhci_ll_reset_register(uhci_ctrl->uhci_num);

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

    // esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &uhci_ctrl->ext_mem_cache_line_size);
    // esp_cache_get_alignment(MALLOC_CAP_INTERNAL, &uhci_ctrl->int_mem_cache_line_size);

    uhci_ctrl->ext_mem_cache_line_size = 1;
    uhci_ctrl->int_mem_cache_line_size = 1;

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

#if CONFIG_UHCI_ISR_CACHE_SAFE
    if (cbs->on_tx_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_tx_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_tx_trans_done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

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
