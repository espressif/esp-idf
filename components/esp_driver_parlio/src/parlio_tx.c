/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#if CONFIG_PARLIO_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_rom_gpio.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "soc/parlio_periph.h"
#include "hal/parlio_ll.h"
#include "driver/gpio.h"
#include "driver/parlio_tx.h"
#include "parlio_private.h"
#include "esp_memory_utils.h"
#include "esp_clk_tree.h"
#include "esp_private/gdma.h"

static const char *TAG = "parlio-tx";

typedef struct {
    uint32_t idle_value; // Parallel IO bus idle value
    const void *payload; // payload to be transmitted
    size_t payload_bits; // payload size in bits
} parlio_tx_trans_desc_t;

typedef struct parlio_tx_unit_t {
    struct parlio_unit_t base; // base unit
    size_t data_width;     // data width
    intr_handle_t intr;    // allocated interrupt handle
    esp_pm_lock_handle_t pm_lock;   // power management lock
    gdma_channel_handle_t dma_chan; // DMA channel
    parlio_dma_desc_t *dma_nodes;   // DMA descriptor nodes
    parlio_dma_desc_t *dma_nodes_nc;// non-cached DMA descriptor nodes
    size_t dma_nodes_num;           // number of DMA descriptor nodes
#if CONFIG_PM_ENABLE
    char pm_lock_name[PARLIO_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
    portMUX_TYPE spinlock;     // prevent resource accessing by user and interrupt concurrently
    uint32_t out_clk_freq_hz;  // output clock frequency
    size_t max_transfer_bits;  // maximum transfer size in bits
    size_t queue_depth;        // size of transaction queue
    size_t num_trans_inflight; // indicates the number of transactions that are undergoing but not recycled to ready_queue
    QueueHandle_t trans_queues[PARLIO_TX_QUEUE_MAX]; // transaction queues
    parlio_tx_trans_desc_t *cur_trans; // points to current transaction
    uint32_t idle_value_mask;          // mask of idle value
    _Atomic parlio_tx_fsm_t fsm;       // Driver FSM state
    parlio_tx_done_callback_t on_trans_done; // callback function when the transmission is done
    void *user_data;                   // user data passed to the callback function
    parlio_tx_trans_desc_t trans_desc_pool[];   // transaction descriptor pool
} parlio_tx_unit_t;

static void parlio_tx_default_isr(void *args);

static esp_err_t parlio_tx_create_trans_queue(parlio_tx_unit_t *tx_unit, const parlio_tx_unit_config_t *config)
{
    esp_err_t ret;

    tx_unit->queue_depth = config->trans_queue_depth;
    // Allocate transaction queues. Each queue only holds pointers to the transaction descriptors
    for (int i = 0; i < PARLIO_TX_QUEUE_MAX; i++) {
        tx_unit->trans_queues[i] = xQueueCreateWithCaps(config->trans_queue_depth, sizeof(parlio_tx_trans_desc_t *), PARLIO_MEM_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(tx_unit->trans_queues[i], ESP_ERR_NO_MEM, exit, TAG, "no mem for queue");
    }

    // Initialize the ready queue
    parlio_tx_trans_desc_t *p_trans_desc = NULL;
    for (int i = 0; i < config->trans_queue_depth; i++) {
        p_trans_desc = &tx_unit->trans_desc_pool[i];
        ESP_GOTO_ON_FALSE(xQueueSend(tx_unit->trans_queues[PARLIO_TX_QUEUE_READY], &p_trans_desc, 0) == pdTRUE,
                          ESP_ERR_INVALID_STATE, exit, TAG, "ready queue full");
    }

    return ESP_OK;

exit:
    for (int i = 0; i < PARLIO_TX_QUEUE_MAX; i++) {
        if (tx_unit->trans_queues[i]) {
            vQueueDeleteWithCaps(tx_unit->trans_queues[i]);
            tx_unit->trans_queues[i] = NULL;
        }
    }
    return ret;
}

static esp_err_t parlio_destroy_tx_unit(parlio_tx_unit_t *tx_unit)
{
    if (tx_unit->intr) {
        ESP_RETURN_ON_ERROR(esp_intr_free(tx_unit->intr), TAG, "delete interrupt service failed");
    }
    if (tx_unit->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(tx_unit->pm_lock), TAG, "delete pm lock failed");
    }
    if (tx_unit->dma_chan) {
        ESP_RETURN_ON_ERROR(gdma_disconnect(tx_unit->dma_chan), TAG, "disconnect dma channel failed");
        ESP_RETURN_ON_ERROR(gdma_del_channel(tx_unit->dma_chan), TAG, "delete dma channel failed");
    }
    for (int i = 0; i < PARLIO_TX_QUEUE_MAX; i++) {
        if (tx_unit->trans_queues[i]) {
            vQueueDeleteWithCaps(tx_unit->trans_queues[i]);
        }
    }
    if (tx_unit->base.group) {
        // de-register from group
        parlio_unregister_unit_from_group(&tx_unit->base);
    }
    if (tx_unit->dma_nodes) {
        free(tx_unit->dma_nodes);
    }
    free(tx_unit);
    return ESP_OK;
}

static esp_err_t parlio_tx_unit_configure_gpio(parlio_tx_unit_t *tx_unit, const parlio_tx_unit_config_t *config)
{
    int group_id = tx_unit->base.group->group_id;
    int unit_id = tx_unit->base.unit_id;
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = config->flags.io_loop_back ? GPIO_MODE_INPUT_OUTPUT : GPIO_MODE_OUTPUT,
        .pull_down_en = false,
        .pull_up_en = true,
    };

    // connect peripheral signals via GPIO matrix
    for (size_t i = 0; i < config->data_width; i++) {
        if (config->data_gpio_nums[i] >= 0) {
            gpio_conf.pin_bit_mask = BIT64(config->data_gpio_nums[i]);
            ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "config data GPIO failed");
            esp_rom_gpio_connect_out_signal(config->data_gpio_nums[i],
                                            parlio_periph_signals.groups[group_id].tx_units[unit_id].data_sigs[i], false, false);
            gpio_func_sel(config->data_gpio_nums[i], PIN_FUNC_GPIO);
        }
    }
    // Note: the valid signal will override TXD[PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG]
    if (config->valid_gpio_num >= 0) {
        gpio_conf.pin_bit_mask = BIT64(config->valid_gpio_num);
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "config valid GPIO failed");
        esp_rom_gpio_connect_out_signal(config->valid_gpio_num,
                                        parlio_periph_signals.groups[group_id].tx_units[unit_id].data_sigs[PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG],
                                        false, false);
        gpio_func_sel(config->valid_gpio_num, PIN_FUNC_GPIO);
    }
    if (config->clk_out_gpio_num >= 0) {
        gpio_conf.pin_bit_mask = BIT64(config->clk_out_gpio_num);
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "config clk out GPIO failed");
        esp_rom_gpio_connect_out_signal(config->clk_out_gpio_num,
                                        parlio_periph_signals.groups[group_id].tx_units[unit_id].clk_out_sig, false, false);
        gpio_func_sel(config->clk_out_gpio_num, PIN_FUNC_GPIO);
    }
    if (config->clk_in_gpio_num >= 0) {
        gpio_conf.mode = config->flags.io_loop_back ? GPIO_MODE_INPUT_OUTPUT : GPIO_MODE_INPUT;
        gpio_conf.pin_bit_mask = BIT64(config->clk_in_gpio_num);
        ESP_RETURN_ON_ERROR(gpio_config(&gpio_conf), TAG, "config clk in GPIO failed");
        esp_rom_gpio_connect_in_signal(config->clk_in_gpio_num,
                                       parlio_periph_signals.groups[group_id].tx_units[unit_id].clk_in_sig, false);
        gpio_func_sel(config->clk_in_gpio_num, PIN_FUNC_GPIO);
    }
    return ESP_OK;
}

static esp_err_t parlio_tx_unit_init_dma(parlio_tx_unit_t *tx_unit)
{
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
    ESP_RETURN_ON_ERROR(PARLIO_GDMA_NEW_CHANNEL(&dma_chan_config, &tx_unit->dma_chan), TAG, "allocate TX DMA channel failed");
    gdma_connect(tx_unit->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_PARLIO, 0));
    gdma_strategy_config_t gdma_strategy_conf = {
        .auto_update_desc = true,
        .owner_check = true,
    };
    gdma_apply_strategy(tx_unit->dma_chan, &gdma_strategy_conf);

    // Link the descriptors
    size_t dma_nodes_num = tx_unit->dma_nodes_num;
    for (int i = 0; i < dma_nodes_num; i++) {
        tx_unit->dma_nodes_nc[i].next = (i == dma_nodes_num - 1) ? NULL : &(tx_unit->dma_nodes[i + 1]);
    }
    return ESP_OK;
}

static esp_err_t parlio_select_periph_clock(parlio_tx_unit_t *tx_unit, const parlio_tx_unit_config_t *config)
{
    parlio_hal_context_t *hal = &tx_unit->base.group->hal;
    parlio_clock_source_t clk_src = config->clk_in_gpio_num >= 0 ? PARLIO_CLK_SRC_EXTERNAL : config->clk_src;
    uint32_t periph_src_clk_hz = 0;
    // if the source clock is input from the GPIO, then we're in the slave mode
    if (clk_src == PARLIO_CLK_SRC_EXTERNAL) {
        periph_src_clk_hz = config->input_clk_src_freq_hz;
    } else {
        // get the internal clock source frequency
        esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &periph_src_clk_hz);
    }
    ESP_RETURN_ON_FALSE(periph_src_clk_hz, ESP_ERR_INVALID_ARG, TAG, "invalid clock source frequency");

#if CONFIG_PM_ENABLE
    if (clk_src != PARLIO_CLK_SRC_EXTERNAL) {
        // XTAL and PLL clock source will be turned off in light sleep, so we need to create a NO_LIGHT_SLEEP lock
        sprintf(tx_unit->pm_lock_name, "parlio_tx_%d_%d", tx_unit->base.group->group_id, tx_unit->base.unit_id); // e.g. parlio_tx_0_0
        esp_err_t ret  = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, tx_unit->pm_lock_name, &tx_unit->pm_lock);
        ESP_RETURN_ON_ERROR(ret, TAG, "create NO_LIGHT_SLEEP lock failed");
    }
#endif
    hal_utils_clk_div_t clk_div = {};
    hal_utils_clk_info_t clk_info = {
        .src_freq_hz = periph_src_clk_hz,
        .exp_freq_hz = config->output_clk_freq_hz,
        .max_integ = PARLIO_LL_TX_MAX_CLK_INT_DIV,
        .min_integ = 1,
        .round_opt = HAL_DIV_ROUND,
    };
#if PARLIO_LL_TX_MAX_CLK_FRACT_DIV
    clk_info.max_fract = PARLIO_LL_TX_MAX_CLK_FRACT_DIV;
    tx_unit->out_clk_freq_hz = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
#else
    tx_unit->out_clk_freq_hz = hal_utils_calc_clk_div_integer(&clk_info, &clk_div.integer);
#endif
    PARLIO_CLOCK_SRC_ATOMIC() {
        parlio_ll_tx_set_clock_source(hal->regs, clk_src);
        // set clock division
        parlio_ll_tx_set_clock_div(hal->regs, &clk_div);
    }
    // precision lost due to division
    if (tx_unit->out_clk_freq_hz != config->output_clk_freq_hz) {
        ESP_LOGW(TAG, "precision loss, real output frequency: %"PRIu32, tx_unit->out_clk_freq_hz);
    }

    return ESP_OK;
}

esp_err_t parlio_new_tx_unit(const parlio_tx_unit_config_t *config, parlio_tx_unit_handle_t *ret_unit)
{
#if CONFIG_PARLIO_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    parlio_tx_unit_t *unit = NULL;
    ESP_RETURN_ON_FALSE(config && ret_unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    size_t data_width = config->data_width;
    // data_width must be power of 2 and less than or equal to SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH
    ESP_RETURN_ON_FALSE(data_width && (data_width <= SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH) && ((data_width & (data_width - 1)) == 0),
                        ESP_ERR_INVALID_ARG, TAG, "invalid data width");
    // data_width must not conflict with the valid signal
    ESP_RETURN_ON_FALSE(!(config->valid_gpio_num >= 0 && data_width > PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG),
                        ESP_ERR_INVALID_ARG, TAG, "valid signal conflicts with data signal");
    ESP_RETURN_ON_FALSE(config->max_transfer_size && config->max_transfer_size <= PARLIO_LL_TX_MAX_BITS_PER_FRAME / 8,
                        ESP_ERR_INVALID_ARG, TAG, "invalid max transfer size");
#if SOC_PARLIO_TX_CLK_SUPPORT_GATING
    // clock gating is controlled by either the MSB bit of data bus or the valid signal
    ESP_RETURN_ON_FALSE(!(config->flags.clk_gate_en && config->valid_gpio_num < 0 && config->data_width <= PARLIO_LL_TX_DATA_LINE_AS_CLK_GATE),
                        ESP_ERR_INVALID_ARG, TAG, "no gpio can control the clock gating");
#else
    ESP_RETURN_ON_FALSE(config->flags.clk_gate_en == 0, ESP_ERR_NOT_SUPPORTED, TAG, "clock gating is not supported");
#endif // SOC_PARLIO_TX_CLK_SUPPORT_GATING

    // malloc unit memory
    uint32_t mem_caps = PARLIO_MEM_ALLOC_CAPS;
    unit = heap_caps_calloc(1, sizeof(parlio_tx_unit_t) + sizeof(parlio_tx_trans_desc_t) * config->trans_queue_depth, mem_caps);
    ESP_GOTO_ON_FALSE(unit, ESP_ERR_NO_MEM, err, TAG, "no memory for tx unit");

    // create DMA descriptors
    // DMA descriptors must be placed in internal SRAM
    mem_caps |= MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
    size_t dma_nodes_num = config->max_transfer_size / DMA_DESCRIPTOR_BUFFER_MAX_SIZE + 1;
    uint32_t data_cache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    // the alignment should meet both the DMA and cache requirement
    size_t alignment = MAX(data_cache_line_size, PARLIO_DMA_DESC_ALIGNMENT);
    size_t dma_nodes_mem_size = ALIGN_UP(dma_nodes_num * sizeof(parlio_dma_desc_t), alignment);
    parlio_dma_desc_t *dma_nodes = heap_caps_aligned_calloc(alignment, 1, dma_nodes_mem_size, mem_caps);
    ESP_GOTO_ON_FALSE(dma_nodes, ESP_ERR_NO_MEM, err, TAG, "no memory for DMA nodes");
    unit->dma_nodes = dma_nodes;
    unit->dma_nodes_num = dma_nodes_num;

    // write back and then invalidate the cached dma_nodes, we will skip the cache (by non-cacheable address) when access the dma_nodes
    if (data_cache_line_size) {
        ESP_GOTO_ON_ERROR(esp_cache_msync(dma_nodes, dma_nodes_mem_size,
                                          ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE),
                          err, TAG, "cache sync failed");
    }
    // we will use the non-cached address to manipulate the DMA descriptor, for simplicity
    unit->dma_nodes_nc = PARLIO_GET_NON_CACHED_DESC_ADDR(dma_nodes);

    unit->max_transfer_bits = config->max_transfer_size * 8;
    unit->base.dir = PARLIO_DIR_TX;
    unit->data_width = data_width;
    //create transaction queue
    ESP_GOTO_ON_ERROR(parlio_tx_create_trans_queue(unit, config), err, TAG, "create transaction queue failed");

    // register the unit to a group
    ESP_GOTO_ON_ERROR(parlio_register_unit_to_group(&unit->base), err, TAG, "register unit to group failed");
    parlio_group_t *group = unit->base.group;
    parlio_hal_context_t *hal = &group->hal;
    // select the clock source
    ESP_GOTO_ON_ERROR(parlio_select_periph_clock(unit, config), err, TAG, "set clock source failed");

    // install interrupt service
    int isr_flags = PARLIO_INTR_ALLOC_FLAG;
    ret = esp_intr_alloc_intrstatus(parlio_periph_signals.groups[group->group_id].tx_irq_id, isr_flags,
                                    (uint32_t)parlio_ll_get_interrupt_status_reg(hal->regs),
                                    PARLIO_LL_EVENT_TX_EOF, parlio_tx_default_isr, unit, &unit->intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install interrupt failed");

    // install DMA service
    ESP_GOTO_ON_ERROR(parlio_tx_unit_init_dma(unit), err, TAG, "install tx DMA failed");

    // reset fifo and core clock domain
    PARLIO_RCC_ATOMIC() {
        parlio_ll_tx_reset_clock(hal->regs);
    }
    parlio_ll_tx_reset_fifo(hal->regs);
    PARLIO_CLOCK_SRC_ATOMIC() {
        // stop output clock
        parlio_ll_tx_enable_clock(hal->regs, false);
    }
    // clock gating
    parlio_ll_tx_enable_clock_gating(hal->regs, config->flags.clk_gate_en);
    // set data width
    parlio_ll_tx_set_bus_width(hal->regs, data_width);
    unit->idle_value_mask = (1 << data_width) - 1;
    // whether to use the valid signal
    if (config->valid_gpio_num >= 0) {
        parlio_ll_tx_treat_msb_as_valid(hal->regs, true);
        unit->idle_value_mask &= ~(1 << PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG);
    } else {
        parlio_ll_tx_treat_msb_as_valid(hal->regs, false);
    }
    // set data byte packing order
    if (data_width < 8) {
        parlio_ll_tx_set_bit_pack_order(hal->regs, config->bit_pack_order);
    }
    // set sample clock edge
    parlio_ll_tx_set_sample_clock_edge(hal->regs, config->sample_edge);

#if SOC_PARLIO_TX_SIZE_BY_DMA
    // Always use DMA EOF as the Parlio TX EOF
    parlio_ll_tx_set_eof_condition(hal->regs, PARLIO_LL_TX_EOF_COND_DMA_EOF);
#endif  // SOC_PARLIO_TX_SIZE_BY_DMA

    // clear any pending interrupt
    parlio_ll_clear_interrupt_status(hal->regs, PARLIO_LL_EVENT_TX_MASK);

    // GPIO Matrix/MUX configuration
    ESP_GOTO_ON_ERROR(parlio_tx_unit_configure_gpio(unit, config), err, TAG, "configure gpio failed");

    portMUX_INITIALIZE(&unit->spinlock);
    atomic_init(&unit->fsm, PARLIO_TX_FSM_INIT);
    // return TX unit handle
    *ret_unit = unit;
    ESP_LOGD(TAG, "new tx unit(%d,%d) at %p, out clk=%"PRIu32"Hz, queue_depth=%zu, idle_mask=%"PRIx32,
             group->group_id, unit->base.unit_id, unit, unit->out_clk_freq_hz, unit->queue_depth, unit->idle_value_mask);
    return ESP_OK;

err:
    if (unit) {
        parlio_destroy_tx_unit(unit);
    }
    return ret;
}

esp_err_t parlio_del_tx_unit(parlio_tx_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(atomic_load(&unit->fsm) == PARLIO_TX_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "unit not in init state");
    ESP_LOGD(TAG, "del tx unit(%d,%d)", unit->base.group->group_id, unit->base.unit_id);
    return parlio_destroy_tx_unit(unit);
}

static void IRAM_ATTR parlio_tx_mount_dma_data(parlio_tx_unit_t *tx_unit, const void *buffer, size_t len)
{
    size_t prepared_length = 0;
    uint8_t *data = (uint8_t *)buffer;
    parlio_dma_desc_t *desc_nc = tx_unit->dma_nodes_nc;

    while (len) {
        uint32_t mount_bytes = len > DMA_DESCRIPTOR_BUFFER_MAX_SIZE ? DMA_DESCRIPTOR_BUFFER_MAX_SIZE : len;
        len -= mount_bytes;
        desc_nc->dw0.suc_eof = (len == 0);    // whether the last frame
        desc_nc->dw0.size = mount_bytes;
        desc_nc->dw0.length = mount_bytes;
        desc_nc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc_nc->buffer = &data[prepared_length];
        desc_nc = PARLIO_GET_NON_CACHED_DESC_ADDR(desc_nc->next);
        prepared_length += mount_bytes;
    }

#if CONFIG_IDF_TARGET_ESP32P4
    // Write back to cache to synchronize the cache before DMA start
    esp_cache_msync(buffer, len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
#endif  // CONFIG_IDF_TARGET_ESP32P4
}

esp_err_t parlio_tx_unit_wait_all_done(parlio_tx_unit_handle_t tx_unit, int timeout_ms)
{
    ESP_RETURN_ON_FALSE(tx_unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    TickType_t wait_ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    // recycle all pending transactions
    parlio_tx_trans_desc_t *t = NULL;
    size_t num_trans_inflight = tx_unit->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(tx_unit->trans_queues[PARLIO_TX_QUEUE_COMPLETE], &t, wait_ticks) == pdTRUE,
                            ESP_ERR_TIMEOUT, TAG, "flush timeout");
        ESP_RETURN_ON_FALSE(xQueueSend(tx_unit->trans_queues[PARLIO_TX_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        tx_unit->num_trans_inflight--;
    }
    return ESP_OK;
}

esp_err_t parlio_tx_unit_register_event_callbacks(parlio_tx_unit_handle_t tx_unit, const parlio_tx_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(tx_unit && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

#if CONFIG_PARLIO_ISR_IRAM_SAFE
    if (cbs->on_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_done), ESP_ERR_INVALID_ARG, TAG, "on_trans_done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif

    tx_unit->on_trans_done = cbs->on_trans_done;
    tx_unit->user_data = user_data;
    return ESP_OK;
}

static void IRAM_ATTR parlio_tx_do_transaction(parlio_tx_unit_t *tx_unit, parlio_tx_trans_desc_t *t)
{
    parlio_hal_context_t *hal = &tx_unit->base.group->hal;

    tx_unit->cur_trans = t;

    // DMA transfer data based on bytes not bits, so convert the bit length to bytes, round up
    parlio_tx_mount_dma_data(tx_unit, t->payload, (t->payload_bits + 7) / 8);

    parlio_ll_tx_reset_fifo(hal->regs);
    PARLIO_RCC_ATOMIC() {
        parlio_ll_tx_reset_clock(hal->regs);
    }
    parlio_ll_tx_set_idle_data_value(hal->regs, t->idle_value);
    parlio_ll_tx_set_trans_bit_len(hal->regs, t->payload_bits);

    gdma_start(tx_unit->dma_chan, (intptr_t)tx_unit->dma_nodes);
    // wait until the data goes from the DMA to TX unit's FIFO
    while (parlio_ll_tx_is_ready(hal->regs) == false);
    // turn on the core clock after we start the TX unit
    parlio_ll_tx_start(hal->regs, true);
    PARLIO_CLOCK_SRC_ATOMIC() {
        parlio_ll_tx_enable_clock(hal->regs, true);
    }
}

esp_err_t parlio_tx_unit_enable(parlio_tx_unit_handle_t tx_unit)
{
    ESP_RETURN_ON_FALSE(tx_unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    parlio_tx_fsm_t expected_fsm = PARLIO_TX_FSM_INIT;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_ENABLE_WAIT)) {
        // acquire power management lock
        if (tx_unit->pm_lock) {
            esp_pm_lock_acquire(tx_unit->pm_lock);
        }
        parlio_hal_context_t *hal = &tx_unit->base.group->hal;
        parlio_ll_enable_interrupt(hal->regs, PARLIO_LL_EVENT_TX_EOF, true);
        atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_ENABLE);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "unit not in init state");
    }

    // check if we need to start one pending transaction
    parlio_tx_trans_desc_t *t = NULL;
    expected_fsm = PARLIO_TX_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_RUN_WAIT)) {
        // check if we need to start one transaction
        if (xQueueReceive(tx_unit->trans_queues[PARLIO_TX_QUEUE_PROGRESS], &t, 0) == pdTRUE) {
            // sanity check
            assert(t);
            atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_RUN);
            parlio_tx_do_transaction(tx_unit, t);
        } else {
            atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_ENABLE);
        }
    }

    return ESP_OK;
}

esp_err_t parlio_tx_unit_disable(parlio_tx_unit_handle_t tx_unit)
{
    ESP_RETURN_ON_FALSE(tx_unit, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    bool valid_state = false;
    // check the supported states, and switch to intermediate state: INIT_WAIT
    parlio_tx_fsm_t expected_fsm = PARLIO_TX_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_INIT_WAIT)) {
        valid_state = true;
    }
    expected_fsm = PARLIO_TX_FSM_RUN;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_INIT_WAIT)) {
        valid_state = true;
        assert(tx_unit->cur_trans);
        // recycle the interrupted transaction
        if (xQueueSend(tx_unit->trans_queues[PARLIO_TX_QUEUE_COMPLETE], &tx_unit->cur_trans, 0) == pdFALSE) {
            // this should never happen
            valid_state = false;
        }
        tx_unit->cur_trans = NULL;
    }
    ESP_RETURN_ON_FALSE(valid_state, ESP_ERR_INVALID_STATE, TAG, "unit can't be disabled in state %d", expected_fsm);

    // stop the TX engine
    parlio_hal_context_t *hal = &tx_unit->base.group->hal;
    gdma_stop(tx_unit->dma_chan);
    parlio_ll_tx_start(hal->regs, false);
    parlio_ll_enable_interrupt(hal->regs, PARLIO_LL_EVENT_TX_EOF, false);

    // release power management lock
    if (tx_unit->pm_lock) {
        esp_pm_lock_release(tx_unit->pm_lock);
    }

    // finally we switch to the INIT state
    atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_INIT);

    return ESP_OK;
}

esp_err_t parlio_tx_unit_transmit(parlio_tx_unit_handle_t tx_unit, const void *payload, size_t payload_bits, const parlio_transmit_config_t *config)
{
    ESP_RETURN_ON_FALSE(tx_unit && payload && payload_bits, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((payload_bits % tx_unit->data_width) == 0, ESP_ERR_INVALID_ARG, TAG, "payload bit length must align to bus width");
    ESP_RETURN_ON_FALSE(payload_bits <= tx_unit->max_transfer_bits, ESP_ERR_INVALID_ARG, TAG, "payload bit length too large");
#if !SOC_PARLIO_TRANS_BIT_ALIGN
    ESP_RETURN_ON_FALSE((payload_bits % 8) == 0, ESP_ERR_INVALID_ARG, TAG, "payload bit length must be multiple of 8");
#endif // !SOC_PARLIO_TRANS_BIT_ALIGN

    TickType_t queue_wait_ticks = portMAX_DELAY;
    if (config->flags.queue_nonblocking) {
        queue_wait_ticks = 0;
    }
    parlio_tx_trans_desc_t *t = NULL;
    // acquire one transaction description from ready queue or complete queue
    if (xQueueReceive(tx_unit->trans_queues[PARLIO_TX_QUEUE_READY], &t, 0) != pdTRUE) {
        if (xQueueReceive(tx_unit->trans_queues[PARLIO_TX_QUEUE_COMPLETE], &t, queue_wait_ticks) == pdTRUE) {
            tx_unit->num_trans_inflight--;
        }
    }
    ESP_RETURN_ON_FALSE(t, ESP_ERR_INVALID_STATE, TAG, "no free transaction descriptor, please consider increasing trans_queue_depth");

    // fill in the transaction descriptor
    memset(t, 0, sizeof(parlio_tx_trans_desc_t));
    t->payload = payload;
    t->payload_bits = payload_bits;
    t->idle_value = config->idle_value & tx_unit->idle_value_mask;

    // send the transaction descriptor to progress queue
    ESP_RETURN_ON_FALSE(xQueueSend(tx_unit->trans_queues[PARLIO_TX_QUEUE_PROGRESS], &t, 0) == pdTRUE,
                        ESP_ERR_INVALID_STATE, TAG, "failed to send transaction descriptor to progress queue");
    tx_unit->num_trans_inflight++;

    // check if we need to start one pending transaction
    parlio_tx_fsm_t expected_fsm = PARLIO_TX_FSM_ENABLE;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_RUN_WAIT)) {
        // check if we need to start one transaction
        if (xQueueReceive(tx_unit->trans_queues[PARLIO_TX_QUEUE_PROGRESS], &t, 0) == pdTRUE) {
            atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_RUN);
            parlio_tx_do_transaction(tx_unit, t);
        } else {
            atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_ENABLE);
        }
    }

    return ESP_OK;
}

static void IRAM_ATTR parlio_tx_default_isr(void *args)
{
    parlio_tx_unit_t *tx_unit = (parlio_tx_unit_t *)args;
    parlio_group_t *group = tx_unit->base.group;
    parlio_hal_context_t *hal = &group->hal;
    BaseType_t high_task_woken = pdFALSE;
    bool need_yield = false;

    uint32_t status = parlio_ll_tx_get_interrupt_status(hal->regs);

    if (status & PARLIO_LL_EVENT_TX_EOF) {
        parlio_ll_clear_interrupt_status(hal->regs, PARLIO_LL_EVENT_TX_EOF);
        PARLIO_CLOCK_SRC_ATOMIC() {
            parlio_ll_tx_enable_clock(hal->regs, false);
        }
        parlio_ll_tx_start(hal->regs, false);

        parlio_tx_trans_desc_t *trans_desc = NULL;

        parlio_tx_fsm_t expected_fsm = PARLIO_TX_FSM_RUN;
        if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_ENABLE_WAIT)) {
            trans_desc = tx_unit->cur_trans;
            // move current finished transaction to the complete queue
            xQueueSendFromISR(tx_unit->trans_queues[PARLIO_TX_QUEUE_COMPLETE], &trans_desc, &high_task_woken);
            if (high_task_woken == pdTRUE) {
                need_yield = true;
            }
            tx_unit->cur_trans = NULL;
            atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_ENABLE);
        }

        // invoke callback
        parlio_tx_done_callback_t done_cb = tx_unit->on_trans_done;
        if (done_cb) {
            if (done_cb(tx_unit, NULL, tx_unit->user_data)) {
                need_yield = true;
            }
        }

        // if the tx unit is till in enable state (i.e. not disabled by user), let's try start the next pending transaction
        expected_fsm = PARLIO_TX_FSM_ENABLE;
        if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_RUN_WAIT)) {
            if (xQueueReceiveFromISR(tx_unit->trans_queues[PARLIO_TX_QUEUE_PROGRESS], &trans_desc, &high_task_woken) == pdTRUE) {
                // sanity check
                assert(trans_desc);
                atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_RUN);
                parlio_tx_do_transaction(tx_unit, trans_desc);
                if (high_task_woken == pdTRUE) {
                    need_yield = true;
                }
            } else {
                atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_ENABLE);
            }
        }

        if (need_yield) {
            portYIELD_FROM_ISR();
        }
    }
}
