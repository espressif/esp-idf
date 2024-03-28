/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/cdefs.h>
#include "esp_compiler.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/gpio.h"
#include "driver/rmt_types_legacy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "soc/soc_memory_layout.h"
#include "soc/rmt_periph.h"
#include "soc/rmt_struct.h"
#include "esp_clk_tree.h"
#include "hal/rmt_hal.h"
#include "hal/rmt_ll.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"

#define RMT_CHANNEL_ERROR_STR "RMT CHANNEL ERR"
#define RMT_ADDR_ERROR_STR "RMT ADDRESS ERR"
#define RMT_MEM_CNT_ERROR_STR "RMT MEM BLOCK NUM ERR"
#define RMT_CARRIER_ERROR_STR "RMT CARRIER LEVEL ERR"
#define RMT_MEM_OWNER_ERROR_STR "RMT MEM OWNER_ERR"
#define RMT_BASECLK_ERROR_STR "RMT BASECLK ERR"
#define RMT_WR_MEM_OVF_ERROR_STR "RMT WR MEM OVERFLOW"
#define RMT_GPIO_ERROR_STR "RMT GPIO ERROR"
#define RMT_MODE_ERROR_STR "RMT MODE ERROR"
#define RMT_CLK_DIV_ERROR_STR "RMT CLK DIV ERR"
#define RMT_DRIVER_ERROR_STR "RMT DRIVER ERR"
#define RMT_DRIVER_LENGTH_ERROR_STR "RMT PARAM LEN ERROR"
#define RMT_PSRAM_BUFFER_WARN_STR "Using buffer allocated from psram"
#define RMT_TRANSLATOR_NULL_STR "RMT translator is null"
#define RMT_TRANSLATOR_UNINIT_STR "RMT translator not init"
#define RMT_PARAM_ERR_STR "RMT param error"

static const char *TAG = "rmt(legacy)";

// Spinlock for protecting concurrent register-level access only
#define RMT_ENTER_CRITICAL()  portENTER_CRITICAL_SAFE(&(rmt_contex.rmt_spinlock))
#define RMT_EXIT_CRITICAL()   portEXIT_CRITICAL_SAFE(&(rmt_contex.rmt_spinlock))

#define RMT_RX_CHANNEL_ENCODING_START (SOC_RMT_CHANNELS_PER_GROUP-SOC_RMT_TX_CANDIDATES_PER_GROUP)
#define RMT_TX_CHANNEL_ENCODING_END   (SOC_RMT_TX_CANDIDATES_PER_GROUP-1)

#define RMT_IS_RX_CHANNEL(channel) ((channel) >= RMT_RX_CHANNEL_ENCODING_START)
#define RMT_IS_TX_CHANNEL(channel) ((channel) <= RMT_TX_CHANNEL_ENCODING_END)
#define RMT_DECODE_RX_CHANNEL(encode_chan) ((encode_chan - RMT_RX_CHANNEL_ENCODING_START))
#define RMT_ENCODE_RX_CHANNEL(decode_chan) ((decode_chan + RMT_RX_CHANNEL_ENCODING_START))

#if SOC_PERIPH_CLK_CTRL_SHARED
#define RMT_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define RMT_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define RMT_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define RMT_RCC_ATOMIC()
#endif

typedef struct {
    rmt_hal_context_t hal;
    _lock_t rmt_driver_isr_lock;
    portMUX_TYPE rmt_spinlock; // Mutex lock for protecting concurrent register/unregister of RMT channels' ISR
    rmt_isr_handle_t rmt_driver_intr_handle;
    rmt_tx_end_callback_t rmt_tx_end_callback;// Event called when transmission is ended
    uint8_t rmt_driver_channels; // Bitmask of installed drivers' channels, used to protect concurrent register/unregister of RMT channels' ISR
    bool rmt_module_enabled;
    uint32_t synchro_channel_mask; // Bitmap of channels already added in the synchronous group
} rmt_contex_t;

typedef struct {
    size_t tx_offset;
    size_t tx_len_rem;
    size_t tx_sub_len;
    bool translator;
    bool wait_done; //Mark whether wait tx done.
    bool loop_autostop; // mark whether loop auto-stop is enabled
    rmt_channel_t channel;
    const rmt_item32_t *tx_data;
    SemaphoreHandle_t tx_sem;
#if CONFIG_SPIRAM_USE_MALLOC
    int intr_alloc_flags;
    StaticSemaphore_t tx_sem_buffer;
#endif
    rmt_item32_t *tx_buf;
    RingbufHandle_t rx_buf;
#if SOC_RMT_SUPPORT_RX_PINGPONG
    rmt_item32_t *rx_item_buf;
    uint32_t rx_item_buf_size;
    uint32_t rx_item_len;
    int rx_item_start_idx;
#endif
    sample_to_rmt_t sample_to_rmt;
    void *tx_context;
    size_t sample_size_remain;
    const uint8_t *sample_cur;
} rmt_obj_t;

static rmt_contex_t rmt_contex = {
    .hal.regs = &RMT,
    .rmt_spinlock = portMUX_INITIALIZER_UNLOCKED,
    .rmt_driver_intr_handle = NULL,
    .rmt_tx_end_callback = {
        .function = NULL,
    },
    .rmt_driver_channels = 0,
    .rmt_module_enabled = false,
    .synchro_channel_mask = 0
};

static rmt_obj_t *p_rmt_obj[RMT_CHANNEL_MAX] = {0};

#if SOC_RMT_CHANNEL_CLK_INDEPENDENT
static uint32_t s_rmt_source_clock_hz[RMT_CHANNEL_MAX];
#else
static uint32_t s_rmt_source_clock_hz;
#endif

// RMTMEM address is declared in <target>.peripherals.ld
extern rmt_mem_t RMTMEM;

//Enable RMT module
static void rmt_module_enable(void)
{
    RMT_ENTER_CRITICAL();
    if (rmt_contex.rmt_module_enabled == false) {
        RMT_RCC_ATOMIC() {
            rmt_ll_enable_bus_clock(0, true);
            rmt_ll_reset_register(0);
        }
        rmt_contex.rmt_module_enabled = true;
    }
    RMT_EXIT_CRITICAL();
}

//Disable RMT module
static void rmt_module_disable(void)
{
    RMT_ENTER_CRITICAL();
    if (rmt_contex.rmt_module_enabled == true) {
        RMT_RCC_ATOMIC() {
            rmt_ll_enable_bus_clock(0, false);
        }
        rmt_contex.rmt_module_enabled = false;
    }
    RMT_EXIT_CRITICAL();
}

esp_err_t rmt_set_clk_div(rmt_channel_t channel, uint8_t div_cnt)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        rmt_ll_rx_set_channel_clock_div(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), div_cnt);
    } else {
        rmt_ll_tx_set_channel_clock_div(rmt_contex.hal.regs, channel, div_cnt);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_clk_div(rmt_channel_t channel, uint8_t *div_cnt)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(div_cnt, ESP_ERR_INVALID_ARG, TAG, RMT_ADDR_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        *div_cnt = (uint8_t)rmt_ll_rx_get_channel_clock_div(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        *div_cnt = (uint8_t)rmt_ll_tx_get_channel_clock_div(rmt_contex.hal.regs, channel);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_rx_idle_thresh(rmt_channel_t channel, uint16_t thresh)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_set_idle_thres(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), thresh);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_rx_idle_thresh(rmt_channel_t channel, uint16_t *thresh)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(thresh, ESP_ERR_INVALID_ARG, TAG, RMT_ADDR_ERROR_STR);
    RMT_ENTER_CRITICAL();
    *thresh = (uint16_t)rmt_ll_rx_get_idle_thres(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_mem_block_num(rmt_channel_t channel, uint8_t rmt_mem_num)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(rmt_mem_num <= RMT_CHANNEL_MAX - channel, ESP_ERR_INVALID_ARG, TAG, RMT_MEM_CNT_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        rmt_ll_rx_set_mem_blocks(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), rmt_mem_num);
    } else {
        rmt_ll_tx_set_mem_blocks(rmt_contex.hal.regs, channel, rmt_mem_num);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_mem_block_num(rmt_channel_t channel, uint8_t *rmt_mem_num)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(rmt_mem_num, ESP_ERR_INVALID_ARG, TAG, RMT_ADDR_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        *rmt_mem_num = (uint8_t)rmt_ll_rx_get_mem_blocks(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        *rmt_mem_num = (uint8_t)rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_carrier(rmt_channel_t channel, bool carrier_en, uint16_t high_level, uint16_t low_level,
                             rmt_carrier_level_t carrier_level)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(carrier_level < RMT_CARRIER_LEVEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CARRIER_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_set_carrier_high_low_ticks(rmt_contex.hal.regs, channel, high_level, low_level);
    rmt_ll_tx_set_carrier_level(rmt_contex.hal.regs, channel, carrier_level);
    rmt_ll_tx_enable_carrier_modulation(rmt_contex.hal.regs, channel, carrier_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_mem_pd(rmt_channel_t channel, bool pd_en)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_power_down_mem(rmt_contex.hal.regs, pd_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_mem_pd(rmt_channel_t channel, bool *pd_en)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    *pd_en = rmt_ll_is_mem_powered_down(rmt_contex.hal.regs);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_tx_start(rmt_channel_t channel, bool tx_idx_rst)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (tx_idx_rst) {
        rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    }
    rmt_ll_clear_interrupt_status(rmt_contex.hal.regs, RMT_LL_EVENT_TX_DONE(channel));
    // enable tx end interrupt in non-loop mode
    if (!rmt_ll_tx_is_loop_enabled(rmt_contex.hal.regs, channel)) {
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_DONE(channel), true);
    } else {
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
        rmt_ll_tx_reset_loop_count(rmt_contex.hal.regs, channel);
        rmt_ll_tx_enable_loop_count(rmt_contex.hal.regs, channel, true);
        rmt_ll_clear_interrupt_status(rmt_contex.hal.regs, RMT_LL_EVENT_TX_LOOP_END(channel));
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_LOOP_END(channel), true);
#endif
    }
    rmt_ll_tx_start(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_tx_stop(rmt_channel_t channel)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
#if SOC_RMT_SUPPORT_TX_ASYNC_STOP
    rmt_ll_tx_stop(rmt_contex.hal.regs, channel);
#else
    // write ending marker to stop the TX channel
    RMTMEM.chan[channel].data32[0].val = 0;
#endif
    rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

#if SOC_RMT_SUPPORT_RX_PINGPONG
esp_err_t rmt_set_rx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    if (en) {
        uint32_t item_block_len = rmt_ll_rx_get_mem_blocks(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel)) * RMT_MEM_ITEM_NUM;
        ESP_RETURN_ON_FALSE(evt_thresh <= item_block_len, ESP_ERR_INVALID_ARG, TAG, "RMT EVT THRESH ERR");
        RMT_ENTER_CRITICAL();
        rmt_ll_rx_set_limit(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), evt_thresh);
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_THRES(RMT_DECODE_RX_CHANNEL(channel)), true);
        RMT_EXIT_CRITICAL();
    } else {
        RMT_ENTER_CRITICAL();
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_THRES(RMT_DECODE_RX_CHANNEL(channel)), false);
        RMT_EXIT_CRITICAL();
    }
    return ESP_OK;
}
#endif

esp_err_t rmt_rx_start(rmt_channel_t channel, bool rx_idx_rst)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_enable(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
    if (rx_idx_rst) {
        rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    }
    rmt_ll_clear_interrupt_status(rmt_contex.hal.regs, RMT_LL_EVENT_RX_DONE(RMT_DECODE_RX_CHANNEL(channel)));
    rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_DONE(RMT_DECODE_RX_CHANNEL(channel)), true);

#if SOC_RMT_SUPPORT_RX_PINGPONG
    const uint32_t item_block_len = rmt_ll_rx_get_mem_blocks(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel)) * RMT_MEM_ITEM_NUM;
    p_rmt_obj[channel]->rx_item_start_idx = 0;
    p_rmt_obj[channel]->rx_item_len = 0;
    rmt_set_rx_thr_intr_en(channel, true, item_block_len / 2);
#endif

    rmt_ll_rx_enable(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), true);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_rx_stop(rmt_channel_t channel)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_DONE(RMT_DECODE_RX_CHANNEL(channel)), false);
    rmt_ll_rx_enable(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
    rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
#if SOC_RMT_SUPPORT_RX_PINGPONG
    rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_THRES(RMT_DECODE_RX_CHANNEL(channel)), false);
#endif
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_tx_memory_reset(rmt_channel_t channel)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_rx_memory_reset(rmt_channel_t channel)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_memory_owner(rmt_channel_t channel, rmt_mem_owner_t owner)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(owner < RMT_MEM_OWNER_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_MEM_OWNER_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), owner);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_memory_owner(rmt_channel_t channel, rmt_mem_owner_t *owner)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(owner, ESP_ERR_INVALID_ARG, TAG, RMT_MEM_OWNER_ERROR_STR);
    RMT_ENTER_CRITICAL();
    *owner = (rmt_mem_owner_t)rmt_ll_rx_get_mem_owner(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_loop_mode(rmt_channel_t channel, bool loop_en)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_enable_loop(rmt_contex.hal.regs, channel, loop_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_tx_loop_mode(rmt_channel_t channel, bool *loop_en)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    *loop_en = rmt_ll_tx_is_loop_enabled(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_rx_filter(rmt_channel_t channel, bool rx_filter_en, uint8_t thresh)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_enable_filter(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), rx_filter_en);
    rmt_ll_rx_set_filter_thres(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), thresh);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_source_clk(rmt_channel_t channel, rmt_source_clk_t base_clk)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    // `rmt_clock_source_t` and `rmt_source_clk_t` are binary compatible, as the underlying enum entries come from the same `soc_module_clk_t`
    RMT_CLOCK_SRC_ATOMIC() {
        rmt_ll_set_group_clock_src(rmt_contex.hal.regs, channel, (rmt_clock_source_t)base_clk, 1, 0, 0);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_source_clk(rmt_channel_t channel, rmt_source_clk_t *src_clk)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    // `rmt_clock_source_t` and `rmt_source_clk_t` are binary compatible, as the underlying enum entries come from the same `soc_module_clk_t`
    *src_clk = (rmt_source_clk_t)rmt_ll_get_group_clock_src(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_idle_level(rmt_channel_t channel, bool idle_out_en, rmt_idle_level_t level)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(level < RMT_IDLE_LEVEL_MAX, ESP_ERR_INVALID_ARG, TAG, "RMT IDLE LEVEL ERR");
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_fix_idle_level(rmt_contex.hal.regs, channel, level, idle_out_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_idle_level(rmt_channel_t channel, bool *idle_out_en, rmt_idle_level_t *level)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    *idle_out_en = rmt_ll_tx_is_idle_enabled(rmt_contex.hal.regs, channel);
    *level = rmt_ll_tx_get_idle_level(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_status(rmt_channel_t channel, uint32_t *status)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        *status = rmt_ll_rx_get_status_word(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        *status = rmt_ll_tx_get_status_word(rmt_contex.hal.regs, channel);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_rx_intr_en(rmt_channel_t channel, bool en)
{
    ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_DONE(RMT_DECODE_RX_CHANNEL(channel)), en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_err_intr_en(rmt_channel_t channel, bool en)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_ERROR(RMT_DECODE_RX_CHANNEL(channel)), en);
    } else {
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_ERROR(channel), en);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_intr_en(rmt_channel_t channel, bool en)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_DONE(channel), en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    if (en) {
        uint32_t item_block_len = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel) * RMT_MEM_ITEM_NUM;
        ESP_RETURN_ON_FALSE(evt_thresh <= item_block_len, ESP_ERR_INVALID_ARG, TAG, "RMT EVT THRESH ERR");
        RMT_ENTER_CRITICAL();
        rmt_ll_tx_set_limit(rmt_contex.hal.regs, channel, evt_thresh);
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_THRES(channel), true);
        RMT_EXIT_CRITICAL();
    } else {
        RMT_ENTER_CRITICAL();
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_THRES(channel), false);
        RMT_EXIT_CRITICAL();
    }
    return ESP_OK;
}

esp_err_t rmt_set_gpio(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num, bool invert_signal)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(mode < RMT_MODE_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_MODE_ERROR_STR);
    ESP_RETURN_ON_FALSE(((GPIO_IS_VALID_GPIO(gpio_num) && (mode == RMT_MODE_RX)) ||
                         (GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) && (mode == RMT_MODE_TX))), ESP_ERR_INVALID_ARG, TAG, RMT_GPIO_ERROR_STR);

    gpio_func_sel(gpio_num, PIN_FUNC_GPIO);
    if (mode == RMT_MODE_TX) {
        ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio_num, rmt_periph_signals.groups[0].channels[channel].tx_sig, invert_signal, 0);
    } else {
        ESP_RETURN_ON_FALSE(RMT_IS_RX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(gpio_num, rmt_periph_signals.groups[0].channels[channel].rx_sig, invert_signal);
    }
    return ESP_OK;
}

static bool rmt_is_channel_number_valid(rmt_channel_t channel, uint8_t mode)
{
    // RX mode
    if (mode == RMT_MODE_RX) {
        return RMT_IS_RX_CHANNEL(channel) && (channel < RMT_CHANNEL_MAX);
    }
    // TX mode
    return (channel >= 0) && RMT_IS_TX_CHANNEL(channel);
}

static esp_err_t rmt_internal_config(rmt_dev_t *dev, const rmt_config_t *rmt_param)
{
    uint8_t mode = rmt_param->rmt_mode;
    uint8_t channel = rmt_param->channel;
    uint8_t gpio_num = rmt_param->gpio_num;
    uint8_t mem_cnt = rmt_param->mem_block_num;
    uint8_t clk_div = rmt_param->clk_div;
    uint32_t carrier_freq_hz = rmt_param->tx_config.carrier_freq_hz;
    bool carrier_en = rmt_param->tx_config.carrier_en;
    uint32_t rmt_source_clk_hz;
    rmt_clock_source_t clk_src = RMT_BASECLK_DEFAULT;

    ESP_RETURN_ON_FALSE(rmt_is_channel_number_valid(channel, mode), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(mem_cnt + channel <= SOC_RMT_CHANNELS_PER_GROUP && mem_cnt > 0, ESP_ERR_INVALID_ARG, TAG, RMT_MEM_CNT_ERROR_STR);
    ESP_RETURN_ON_FALSE(clk_div > 0, ESP_ERR_INVALID_ARG, TAG, RMT_CLK_DIV_ERROR_STR);

    if (mode == RMT_MODE_TX) {
        ESP_RETURN_ON_FALSE(!carrier_en || carrier_freq_hz > 0, ESP_ERR_INVALID_ARG, TAG, "RMT carrier frequency can't be zero");
    }

    RMT_ENTER_CRITICAL();
    rmt_ll_enable_mem_access_nonfifo(dev, true);

    if (rmt_param->flags & RMT_CHANNEL_FLAGS_AWARE_DFS) {
#if SOC_RMT_SUPPORT_XTAL
        // clock src: XTAL_CLK
        clk_src = RMT_BASECLK_XTAL;
#elif SOC_RMT_SUPPORT_REF_TICK
        // clock src: REF_CLK
        clk_src = RMT_BASECLK_REF;
#else
#error "No clock source is aware of DFS"
#endif
    }
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &rmt_source_clk_hz);
    RMT_CLOCK_SRC_ATOMIC() {
        rmt_ll_set_group_clock_src(dev, channel, clk_src, 1, 0, 0);
        rmt_ll_enable_group_clock(dev, true);
    }
    RMT_EXIT_CRITICAL();

#if SOC_RMT_CHANNEL_CLK_INDEPENDENT
    s_rmt_source_clock_hz[channel] = rmt_source_clk_hz;
#else
    if (s_rmt_source_clock_hz && rmt_source_clk_hz != s_rmt_source_clock_hz) {
        ESP_LOGW(TAG, "RMT clock source has been configured to %"PRIu32" by other channel, now reconfigure it to %"PRIu32, s_rmt_source_clock_hz, rmt_source_clk_hz);
    }
    s_rmt_source_clock_hz = rmt_source_clk_hz;
#endif
    ESP_LOGD(TAG, "rmt_source_clk_hz: %"PRIu32, rmt_source_clk_hz);

    if (mode == RMT_MODE_TX) {
        uint16_t carrier_duty_percent = rmt_param->tx_config.carrier_duty_percent;
        uint8_t carrier_level = rmt_param->tx_config.carrier_level;
        uint8_t idle_level = rmt_param->tx_config.idle_level;

        RMT_ENTER_CRITICAL();
        rmt_ll_tx_set_channel_clock_div(dev, channel, clk_div);
        rmt_ll_tx_set_mem_blocks(dev, channel, mem_cnt);
        rmt_ll_tx_reset_pointer(dev, channel);
        rmt_ll_tx_enable_loop(dev, channel, rmt_param->tx_config.loop_en);
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
        if (rmt_param->tx_config.loop_en) {
            rmt_ll_tx_set_loop_count(dev, channel, rmt_param->tx_config.loop_count);
        }
#endif
        /* always enable tx ping-pong */
        rmt_ll_tx_enable_wrap(dev, channel, true);
        /*Set idle level */
        rmt_ll_tx_fix_idle_level(dev, channel, idle_level, rmt_param->tx_config.idle_output_en);
        /*Set carrier*/
        rmt_ll_tx_enable_carrier_modulation(dev, channel, carrier_en);
        if (carrier_en) {
            uint32_t duty_div, duty_h, duty_l;
            duty_div = rmt_source_clk_hz / carrier_freq_hz;
            duty_h = duty_div * carrier_duty_percent / 100;
            duty_l = duty_div - duty_h;
            rmt_ll_tx_set_carrier_level(dev, channel, carrier_level);
            rmt_ll_tx_set_carrier_high_low_ticks(dev, channel, duty_h, duty_l);
        } else {
            rmt_ll_tx_set_carrier_level(dev, channel, 0);
        }
        RMT_EXIT_CRITICAL();

        ESP_LOGD(TAG, "Rmt Tx Channel %u|Gpio %u|Sclk_Hz %"PRIu32"|Div %u|Carrier_Hz %"PRIu32"|Duty %u",
                 channel, gpio_num, rmt_source_clk_hz, clk_div, carrier_freq_hz, carrier_duty_percent);
    } else if (RMT_MODE_RX == mode) {
        uint8_t filter_cnt = rmt_param->rx_config.filter_ticks_thresh;
        uint16_t threshold = rmt_param->rx_config.idle_threshold;

        RMT_ENTER_CRITICAL();
        rmt_ll_rx_set_channel_clock_div(dev, RMT_DECODE_RX_CHANNEL(channel), clk_div);
        rmt_ll_rx_set_mem_blocks(dev, RMT_DECODE_RX_CHANNEL(channel), mem_cnt);
        rmt_ll_rx_reset_pointer(dev, RMT_DECODE_RX_CHANNEL(channel));
        rmt_ll_rx_set_mem_owner(dev, RMT_DECODE_RX_CHANNEL(channel), RMT_LL_MEM_OWNER_HW);
        /*Set idle threshold*/
        rmt_ll_rx_set_idle_thres(dev, RMT_DECODE_RX_CHANNEL(channel), threshold);
        /* Set RX filter */
        rmt_ll_rx_set_filter_thres(dev, RMT_DECODE_RX_CHANNEL(channel), filter_cnt);
        rmt_ll_rx_enable_filter(dev, RMT_DECODE_RX_CHANNEL(channel), rmt_param->rx_config.filter_en);

#if SOC_RMT_SUPPORT_RX_PINGPONG
        /* always enable rx ping-pong */
        rmt_ll_rx_enable_wrap(dev, RMT_DECODE_RX_CHANNEL(channel), true);
#endif

#if SOC_RMT_SUPPORT_RX_DEMODULATION
        rmt_ll_rx_enable_carrier_demodulation(dev, RMT_DECODE_RX_CHANNEL(channel), rmt_param->rx_config.rm_carrier);
        if (rmt_param->rx_config.rm_carrier) {
            uint32_t duty_total = rmt_source_clk_hz / rmt_ll_rx_get_channel_clock_div(dev, RMT_DECODE_RX_CHANNEL(channel)) / rmt_param->rx_config.carrier_freq_hz;
            uint32_t duty_high = duty_total * rmt_param->rx_config.carrier_duty_percent / 100;
            // there could be residual in timing the carrier pulse, so double enlarge the theoretical value
            rmt_ll_rx_set_carrier_high_low_ticks(dev, RMT_DECODE_RX_CHANNEL(channel), duty_high * 2, (duty_total - duty_high) * 2);
            rmt_ll_rx_set_carrier_level(dev, RMT_DECODE_RX_CHANNEL(channel), rmt_param->rx_config.carrier_level);
        }
#endif
        RMT_EXIT_CRITICAL();

        ESP_LOGD(TAG, "Rmt Rx Channel %u|Gpio %u|Sclk_Hz %"PRIu32"|Div %u|Threshold %u|Filter %u",
                 channel, gpio_num, rmt_source_clk_hz, clk_div, threshold, filter_cnt);
    }

    return ESP_OK;
}

esp_err_t rmt_config(const rmt_config_t *rmt_param)
{
    rmt_module_enable();

    ESP_RETURN_ON_ERROR(rmt_set_gpio(rmt_param->channel, rmt_param->rmt_mode, rmt_param->gpio_num, rmt_param->flags & RMT_CHANNEL_FLAGS_INVERT_SIG), TAG, "set gpio for RMT driver failed");
    ESP_RETURN_ON_ERROR(rmt_internal_config(&RMT, rmt_param), TAG, "initialize RMT driver failed");

    return ESP_OK;
}

static void IRAM_ATTR rmt_fill_memory(rmt_channel_t channel, const rmt_item32_t *item,
                                      uint16_t item_num, uint16_t mem_offset)
{
    uint32_t *from = (uint32_t *)item;
    volatile uint32_t *to = (volatile uint32_t *)&RMTMEM.chan[channel].data32[0].val;
    to += mem_offset;
    while (item_num--) {
        *to++ = *from++;
    }
}

esp_err_t rmt_fill_tx_items(rmt_channel_t channel, const rmt_item32_t *item, uint16_t item_num, uint16_t mem_offset)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), (0), TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(item, ESP_ERR_INVALID_ARG, TAG, RMT_ADDR_ERROR_STR);
    ESP_RETURN_ON_FALSE(item_num > 0, ESP_ERR_INVALID_ARG, TAG, RMT_DRIVER_LENGTH_ERROR_STR);

    uint8_t mem_cnt = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    ESP_RETURN_ON_FALSE(mem_cnt * RMT_MEM_ITEM_NUM >= item_num, ESP_ERR_INVALID_ARG, TAG, RMT_WR_MEM_OVF_ERROR_STR);
    rmt_fill_memory(channel, item, item_num, mem_offset);
    return ESP_OK;
}

esp_err_t rmt_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, rmt_isr_handle_t *handle)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, RMT_ADDR_ERROR_STR);
    ESP_RETURN_ON_FALSE(rmt_contex.rmt_driver_channels == 0, ESP_FAIL, TAG, "RMT driver installed, can not install generic ISR handler");

    return esp_intr_alloc(rmt_periph_signals.groups[0].irq, intr_alloc_flags, fn, arg, handle);
}

esp_err_t rmt_isr_deregister(rmt_isr_handle_t handle)
{
    return esp_intr_free(handle);
}

static void IRAM_ATTR rmt_driver_isr_default(void *arg)
{
    uint32_t status = 0;
    rmt_item32_t *addr = NULL;
    uint8_t channel = 0;
    rmt_hal_context_t *hal = (rmt_hal_context_t *)arg;
    BaseType_t HPTaskAwoken = pdFALSE;

    // Tx end interrupt
    status = rmt_ll_get_tx_end_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[channel];
        if (p_rmt) {
            xSemaphoreGiveFromISR(p_rmt->tx_sem, &HPTaskAwoken);
            rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
            p_rmt->tx_data = NULL;
            p_rmt->tx_len_rem = 0;
            p_rmt->tx_offset = 0;
            p_rmt->tx_sub_len = 0;
            p_rmt->sample_cur = NULL;
            p_rmt->translator = false;
            if (rmt_contex.rmt_tx_end_callback.function) {
                rmt_contex.rmt_tx_end_callback.function(channel, rmt_contex.rmt_tx_end_callback.arg);
            }
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_DONE(channel));
    }

    // Tx thres interrupt
    status = rmt_ll_get_tx_thres_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[channel];
        if (p_rmt) {
            if (p_rmt->translator) {
                if (p_rmt->sample_size_remain > 0) {
                    size_t translated_size = 0;
                    p_rmt->sample_to_rmt((void *)p_rmt->sample_cur,
                                         p_rmt->tx_buf,
                                         p_rmt->sample_size_remain,
                                         p_rmt->tx_sub_len,
                                         &translated_size,
                                         &p_rmt->tx_len_rem);
                    p_rmt->sample_size_remain -= translated_size;
                    p_rmt->sample_cur += translated_size;
                    p_rmt->tx_data = p_rmt->tx_buf;
                } else {
                    p_rmt->sample_cur = NULL;
                    p_rmt->translator = false;
                }
            }
            const rmt_item32_t *pdata = p_rmt->tx_data;
            size_t len_rem = p_rmt->tx_len_rem;
            rmt_idle_level_t idle_level = rmt_ll_tx_get_idle_level(hal->regs, channel);
            rmt_item32_t stop_data = (rmt_item32_t) {
                .level0 = idle_level,
                .duration0 = 0,
            };
            if (len_rem >= p_rmt->tx_sub_len) {
                rmt_fill_memory(channel, pdata, p_rmt->tx_sub_len, p_rmt->tx_offset);
                p_rmt->tx_data += p_rmt->tx_sub_len;
                p_rmt->tx_len_rem -= p_rmt->tx_sub_len;
            } else if (len_rem == 0) {
                rmt_fill_memory(channel, &stop_data, 1, p_rmt->tx_offset);
            } else {
                rmt_fill_memory(channel, pdata, len_rem, p_rmt->tx_offset);
                rmt_fill_memory(channel, &stop_data, 1, p_rmt->tx_offset + len_rem);
                p_rmt->tx_data += len_rem;
                p_rmt->tx_len_rem -= len_rem;
            }
            if (p_rmt->tx_offset == 0) {
                p_rmt->tx_offset = p_rmt->tx_sub_len;
            } else {
                p_rmt->tx_offset = 0;
            }
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_THRES(channel));
    }

    // Rx end interrupt
    status = rmt_ll_get_rx_end_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[RMT_ENCODE_RX_CHANNEL(channel)];
        if (p_rmt) {
            rmt_ll_rx_enable(rmt_contex.hal.regs, channel, false);
            int item_len = rmt_ll_rx_get_memory_writer_offset(rmt_contex.hal.regs, channel);
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_LL_MEM_OWNER_SW);
            if (p_rmt->rx_buf) {
                addr = (rmt_item32_t *)RMTMEM.chan[RMT_ENCODE_RX_CHANNEL(channel)].data32;
#if SOC_RMT_SUPPORT_RX_PINGPONG
                if (item_len > p_rmt->rx_item_start_idx) {
                    item_len = item_len - p_rmt->rx_item_start_idx;
                }
                memcpy((void *)(p_rmt->rx_item_buf + p_rmt->rx_item_len), (void *)(addr + p_rmt->rx_item_start_idx), item_len * 4);
                p_rmt->rx_item_len += item_len;
                BaseType_t res = xRingbufferSendFromISR(p_rmt->rx_buf, (void *)(p_rmt->rx_item_buf), p_rmt->rx_item_len * 4, &HPTaskAwoken);
#else
                BaseType_t res = xRingbufferSendFromISR(p_rmt->rx_buf, (void *)addr, item_len * 4, &HPTaskAwoken);
#endif
                if (res == pdFALSE) {
                    ESP_DRAM_LOGE(TAG, "RMT RX BUFFER FULL");
                }
            } else {
                ESP_DRAM_LOGE(TAG, "RMT RX BUFFER ERROR");
            }

#if SOC_RMT_SUPPORT_RX_PINGPONG
            p_rmt->rx_item_start_idx = 0;
            p_rmt->rx_item_len = 0;
            memset((void *)p_rmt->rx_item_buf, 0, p_rmt->rx_item_buf_size);
#endif
            rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, channel);
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_LL_MEM_OWNER_HW);
            rmt_ll_rx_enable(rmt_contex.hal.regs, channel, true);
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_DONE(channel));
    }

#if SOC_RMT_SUPPORT_RX_PINGPONG
    // Rx thres interrupt
    status = rmt_ll_get_rx_thres_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[RMT_ENCODE_RX_CHANNEL(channel)];
        int mem_item_size = rmt_ll_rx_get_mem_blocks(rmt_contex.hal.regs, channel) * RMT_MEM_ITEM_NUM;
        int rx_thres_lim = rmt_ll_rx_get_limit(rmt_contex.hal.regs, channel);
        int item_len = (p_rmt->rx_item_start_idx == 0) ? rx_thres_lim : (mem_item_size - rx_thres_lim);
        if ((p_rmt->rx_item_len + item_len) < (p_rmt->rx_item_buf_size / 4)) {
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_LL_MEM_OWNER_SW);
            memcpy((void *)(p_rmt->rx_item_buf + p_rmt->rx_item_len), (void *)(RMTMEM.chan[RMT_ENCODE_RX_CHANNEL(channel)].data32 + p_rmt->rx_item_start_idx), item_len * 4);
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_LL_MEM_OWNER_HW);
            p_rmt->rx_item_len += item_len;
            p_rmt->rx_item_start_idx += item_len;
            if (p_rmt->rx_item_start_idx >= mem_item_size) {
                p_rmt->rx_item_start_idx = 0;
            }
        } else {
            ESP_DRAM_LOGE(TAG, "---RX buffer too small: %d", sizeof(p_rmt->rx_item_buf));
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_THRES(channel));
    }
#endif

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    // loop count interrupt
    status = rmt_ll_get_tx_loop_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[channel];
        if (p_rmt) {
            if (p_rmt->loop_autostop) {
#ifndef SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP
                // hardware doesn't support automatically stop output so driver should stop output here (possibility already overshotted several us)
                rmt_ll_tx_stop(rmt_contex.hal.regs, channel);
                rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
#endif
            }
            xSemaphoreGiveFromISR(p_rmt->tx_sem, &HPTaskAwoken);
            if (rmt_contex.rmt_tx_end_callback.function) {
                rmt_contex.rmt_tx_end_callback.function(channel,  rmt_contex.rmt_tx_end_callback.arg);
            }
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_LOOP_END(channel));
    }
#endif

    // RX Err interrupt
    status = rmt_ll_get_rx_err_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[RMT_ENCODE_RX_CHANNEL(channel)];
        if (p_rmt) {
            // Reset the receiver's write/read addresses to prevent endless err interrupts.
            rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, channel);
            ESP_DRAM_LOGD(TAG, "RMT RX channel %d error", channel);
            ESP_DRAM_LOGD(TAG, "status: 0x%08x", rmt_ll_rx_get_status_word(rmt_contex.hal.regs, channel));
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_RX_ERROR(channel));
    }

    // TX Err interrupt
    status = rmt_ll_get_tx_err_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[channel];
        if (p_rmt) {
            // Reset the transmitter's write/read addresses to prevent endless err interrupts.
            rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
            ESP_DRAM_LOGD(TAG, "RMT TX channel %d error", channel);
            ESP_DRAM_LOGD(TAG, "status: 0x%08x", rmt_ll_tx_get_status_word(rmt_contex.hal.regs, channel));
        }
        rmt_ll_clear_interrupt_status(hal->regs, RMT_LL_EVENT_TX_ERROR(channel));
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t rmt_driver_uninstall(rmt_channel_t channel)
{
    esp_err_t err = ESP_OK;
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    // we allow to call this uninstall function on the same channel for multiple times
    if (p_rmt_obj[channel] == NULL) {
        return ESP_OK;
    }
    //Avoid blocking here(when the interrupt is disabled and do not wait tx done).
    if (p_rmt_obj[channel]->wait_done) {
        xSemaphoreTake(p_rmt_obj[channel]->tx_sem, portMAX_DELAY);
    }

    RMT_ENTER_CRITICAL();
    // check channel's working mode
    if (p_rmt_obj[channel]->rx_buf) {
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_RX_MASK(RMT_DECODE_RX_CHANNEL(channel)) | RMT_LL_EVENT_RX_ERROR(RMT_DECODE_RX_CHANNEL(channel)), false);
    } else {
        rmt_ll_enable_interrupt(rmt_contex.hal.regs, RMT_LL_EVENT_TX_MASK(channel) | RMT_LL_EVENT_TX_ERROR(channel), false);
    }
    RMT_EXIT_CRITICAL();

    _lock_acquire_recursive(&(rmt_contex.rmt_driver_isr_lock));
    rmt_contex.rmt_driver_channels &= ~BIT(channel);
    if (rmt_contex.rmt_driver_channels == 0 && rmt_contex.rmt_driver_intr_handle) {
        rmt_module_disable();
        // all channels have driver disabled
        err = rmt_isr_deregister(rmt_contex.rmt_driver_intr_handle);
        rmt_contex.rmt_driver_intr_handle = NULL;
    }
    _lock_release_recursive(&(rmt_contex.rmt_driver_isr_lock));

    if (p_rmt_obj[channel]->tx_sem) {
        vSemaphoreDelete(p_rmt_obj[channel]->tx_sem);
        p_rmt_obj[channel]->tx_sem = NULL;
    }
    if (p_rmt_obj[channel]->rx_buf) {
        vRingbufferDelete(p_rmt_obj[channel]->rx_buf);
        p_rmt_obj[channel]->rx_buf = NULL;
    }
    if (p_rmt_obj[channel]->tx_buf) {
        free(p_rmt_obj[channel]->tx_buf);
        p_rmt_obj[channel]->tx_buf = NULL;
    }
    if (p_rmt_obj[channel]->sample_to_rmt) {
        p_rmt_obj[channel]->sample_to_rmt = NULL;
    }
#if SOC_RMT_SUPPORT_RX_PINGPONG
    if (p_rmt_obj[channel]->rx_item_buf) {
        free(p_rmt_obj[channel]->rx_item_buf);
        p_rmt_obj[channel]->rx_item_buf = NULL;
        p_rmt_obj[channel]->rx_item_buf_size = 0;
    }
#endif

    free(p_rmt_obj[channel]);
    p_rmt_obj[channel] = NULL;
    return err;
}

esp_err_t rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);

    esp_err_t err = ESP_OK;

    if (p_rmt_obj[channel]) {
        ESP_LOGD(TAG, "RMT driver already installed");
        return ESP_ERR_INVALID_STATE;
    }

#if CONFIG_RINGBUF_PLACE_ISR_FUNCTIONS_INTO_FLASH
    if (intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
        ESP_LOGE(TAG, "ringbuf ISR functions in flash, but used in IRAM interrupt");
        return ESP_ERR_INVALID_ARG;
    }
#endif

#if !CONFIG_SPIRAM_USE_MALLOC
    p_rmt_obj[channel] = calloc(1, sizeof(rmt_obj_t));
#else
    if (!(intr_alloc_flags & ESP_INTR_FLAG_IRAM)) {
        p_rmt_obj[channel] = calloc(1, sizeof(rmt_obj_t));
    } else {
        p_rmt_obj[channel] = heap_caps_calloc(1, sizeof(rmt_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }
#endif

    if (p_rmt_obj[channel] == NULL) {
        ESP_LOGE(TAG, "RMT driver malloc error");
        return ESP_ERR_NO_MEM;
    }

    p_rmt_obj[channel]->tx_len_rem = 0;
    p_rmt_obj[channel]->tx_data = NULL;
    p_rmt_obj[channel]->channel = channel;
    p_rmt_obj[channel]->tx_offset = 0;
    p_rmt_obj[channel]->tx_sub_len = 0;
    p_rmt_obj[channel]->wait_done = false;
    p_rmt_obj[channel]->loop_autostop = false;
    p_rmt_obj[channel]->translator = false;
    p_rmt_obj[channel]->sample_to_rmt = NULL;
    if (p_rmt_obj[channel]->tx_sem == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        p_rmt_obj[channel]->tx_sem = xSemaphoreCreateBinary();
#else
        p_rmt_obj[channel]->intr_alloc_flags = intr_alloc_flags;
        if (!(intr_alloc_flags & ESP_INTR_FLAG_IRAM)) {
            p_rmt_obj[channel]->tx_sem = xSemaphoreCreateBinary();
        } else {
            p_rmt_obj[channel]->tx_sem = xSemaphoreCreateBinaryStatic(&p_rmt_obj[channel]->tx_sem_buffer);
        }
#endif
        xSemaphoreGive(p_rmt_obj[channel]->tx_sem);
    }
    if (p_rmt_obj[channel]->rx_buf == NULL && rx_buf_size > 0) {
        p_rmt_obj[channel]->rx_buf = xRingbufferCreate(rx_buf_size, RINGBUF_TYPE_NOSPLIT);
    }

#if SOC_RMT_SUPPORT_RX_PINGPONG
    if (p_rmt_obj[channel]->rx_item_buf == NULL && rx_buf_size > 0) {
#if !CONFIG_SPIRAM_USE_MALLOC
        p_rmt_obj[channel]->rx_item_buf = calloc(1, rx_buf_size);
#else
        if (!(p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM)) {
            p_rmt_obj[channel]->rx_item_buf = calloc(1, rx_buf_size);
        } else {
            p_rmt_obj[channel]->rx_item_buf = heap_caps_calloc(1, rx_buf_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        }
#endif
        if (p_rmt_obj[channel]->rx_item_buf == NULL) {
            ESP_LOGE(TAG, "RMT malloc fail");
            return ESP_FAIL;
        }
        p_rmt_obj[channel]->rx_item_buf_size = rx_buf_size;
    }
#endif

    _lock_acquire_recursive(&(rmt_contex.rmt_driver_isr_lock));

    if (rmt_contex.rmt_driver_channels == 0) {
        // first RMT channel using driver
        err = rmt_isr_register(rmt_driver_isr_default, &rmt_contex.hal, intr_alloc_flags, &(rmt_contex.rmt_driver_intr_handle));
    }
    if (err == ESP_OK) {
        rmt_contex.rmt_driver_channels |= BIT(channel);
    }
    _lock_release_recursive(&(rmt_contex.rmt_driver_isr_lock));

    rmt_module_enable();

    if (RMT_IS_RX_CHANNEL(channel)) {
        rmt_hal_rx_channel_reset(&rmt_contex.hal, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        rmt_hal_tx_channel_reset(&rmt_contex.hal, channel);
    }

    return err;
}

esp_err_t rmt_write_items(rmt_channel_t channel, const rmt_item32_t *rmt_item, int item_num, bool wait_tx_done)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel], ESP_FAIL, TAG, RMT_DRIVER_ERROR_STR);
    ESP_RETURN_ON_FALSE(rmt_item, ESP_FAIL, TAG, RMT_ADDR_ERROR_STR);
    ESP_RETURN_ON_FALSE(item_num > 0, ESP_ERR_INVALID_ARG, TAG, RMT_DRIVER_LENGTH_ERROR_STR);
    uint32_t mem_blocks = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    ESP_RETURN_ON_FALSE(mem_blocks + channel <= SOC_RMT_CHANNELS_PER_GROUP, ESP_ERR_INVALID_STATE, TAG, RMT_MEM_CNT_ERROR_STR);
#if CONFIG_SPIRAM_USE_MALLOC
    if (p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
        if (!esp_ptr_internal(rmt_item)) {
            ESP_LOGE(TAG, RMT_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    rmt_obj_t *p_rmt = p_rmt_obj[channel];
    int item_block_len = mem_blocks * RMT_MEM_ITEM_NUM;
    int item_sub_len = mem_blocks * RMT_MEM_ITEM_NUM / 2;
    int len_rem = item_num;
    xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
    // fill the memory block first
    if (item_num >= item_block_len) {
        rmt_fill_memory(channel, rmt_item, item_block_len, 0);
        len_rem -= item_block_len;
        rmt_set_tx_loop_mode(channel, false);
        rmt_set_tx_thr_intr_en(channel, 1, item_sub_len);
        p_rmt->tx_data = rmt_item + item_block_len;
        p_rmt->tx_len_rem = len_rem;
        p_rmt->tx_offset = 0;
        p_rmt->tx_sub_len = item_sub_len;
    } else {
        rmt_fill_memory(channel, rmt_item, len_rem, 0);
        rmt_idle_level_t idle_level = rmt_ll_tx_get_idle_level(rmt_contex.hal.regs, channel);
        rmt_item32_t stop_data = (rmt_item32_t) {
            .level0 = idle_level,
            .duration0 = 0,
        };
        rmt_fill_memory(channel, &stop_data, 1, len_rem);
        p_rmt->tx_len_rem = 0;
    }
    rmt_tx_start(channel, true);
    p_rmt->wait_done = wait_tx_done;
    if (wait_tx_done) {
        // wait loop done
        if (rmt_ll_tx_is_loop_enabled(rmt_contex.hal.regs, channel)) {
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
            xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
            xSemaphoreGive(p_rmt->tx_sem);
#endif
        } else {
            // wait tx end
            xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
            xSemaphoreGive(p_rmt->tx_sem);
        }
    }
    return ESP_OK;
}

esp_err_t rmt_wait_tx_done(rmt_channel_t channel, TickType_t wait_time)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel], ESP_FAIL, TAG, RMT_DRIVER_ERROR_STR);
    if (xSemaphoreTake(p_rmt_obj[channel]->tx_sem, wait_time) == pdTRUE) {
        p_rmt_obj[channel]->wait_done = false;
        xSemaphoreGive(p_rmt_obj[channel]->tx_sem);
        return ESP_OK;
    } else {
        if (wait_time != 0) {
            // Don't emit error message if just polling.
            ESP_LOGE(TAG, "Timeout on wait_tx_done");
        }
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t rmt_get_ringbuf_handle(rmt_channel_t channel, RingbufHandle_t *buf_handle)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel], ESP_FAIL, TAG, RMT_DRIVER_ERROR_STR);
    ESP_RETURN_ON_FALSE(buf_handle, ESP_ERR_INVALID_ARG, TAG, RMT_ADDR_ERROR_STR);
    *buf_handle = p_rmt_obj[channel]->rx_buf;
    return ESP_OK;
}

rmt_tx_end_callback_t rmt_register_tx_end_callback(rmt_tx_end_fn_t function, void *arg)
{
    rmt_tx_end_callback_t previous = rmt_contex.rmt_tx_end_callback;
    rmt_contex.rmt_tx_end_callback.function = function;
    rmt_contex.rmt_tx_end_callback.arg = arg;
    return previous;
}

esp_err_t rmt_translator_init(rmt_channel_t channel, sample_to_rmt_t fn)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, RMT_TRANSLATOR_NULL_STR);
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel], ESP_FAIL, TAG, RMT_DRIVER_ERROR_STR);
    uint32_t mem_blocks = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    ESP_RETURN_ON_FALSE(mem_blocks + channel <= SOC_RMT_CHANNELS_PER_GROUP, ESP_ERR_INVALID_STATE, TAG, RMT_MEM_CNT_ERROR_STR);
    const uint32_t block_size = mem_blocks * RMT_MEM_ITEM_NUM * sizeof(rmt_item32_t);
    if (p_rmt_obj[channel]->tx_buf == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)calloc(1, block_size);
#else
        if (p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
            p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)heap_caps_calloc(1, block_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        } else {
            p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)calloc(1, block_size);
        }
#endif
        if (p_rmt_obj[channel]->tx_buf == NULL) {
            ESP_LOGE(TAG, "RMT translator buffer create fail");
            return ESP_FAIL;
        }
    }
    p_rmt_obj[channel]->sample_to_rmt = fn;
    p_rmt_obj[channel]->tx_context = NULL;
    p_rmt_obj[channel]->sample_size_remain = 0;
    p_rmt_obj[channel]->sample_cur = NULL;
    ESP_LOGD(TAG, "RMT translator init done");
    return ESP_OK;
}

esp_err_t rmt_translator_set_context(rmt_channel_t channel, void *context)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel], ESP_FAIL, TAG, RMT_DRIVER_ERROR_STR);

    p_rmt_obj[channel]->tx_context = context;
    return ESP_OK;
}

esp_err_t rmt_translator_get_context(const size_t *item_num, void **context)
{
    ESP_RETURN_ON_FALSE(item_num && context, ESP_ERR_INVALID_ARG, TAG, "invalid arguments");

    // the address of tx_len_rem is directlly passed to the callback,
    // so it's possible to get the object address from that
    rmt_obj_t *obj = __containerof(item_num, rmt_obj_t, tx_len_rem);
    *context = obj->tx_context;

    return ESP_OK;
}

esp_err_t rmt_write_sample(rmt_channel_t channel, const uint8_t *src, size_t src_size, bool wait_tx_done)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel], ESP_FAIL, TAG, RMT_DRIVER_ERROR_STR);
    ESP_RETURN_ON_FALSE(p_rmt_obj[channel]->sample_to_rmt, ESP_FAIL, TAG, RMT_TRANSLATOR_UNINIT_STR);
    uint32_t mem_blocks = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    ESP_RETURN_ON_FALSE(mem_blocks + channel <= SOC_RMT_CHANNELS_PER_GROUP, ESP_ERR_INVALID_STATE, TAG, RMT_MEM_CNT_ERROR_STR);
#if CONFIG_SPIRAM_USE_MALLOC
    if (p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
        if (!esp_ptr_internal(src)) {
            ESP_LOGE(TAG, RMT_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    size_t translated_size = 0;
    rmt_obj_t *p_rmt = p_rmt_obj[channel];
    const uint32_t item_block_len = mem_blocks * RMT_MEM_ITEM_NUM;
    const uint32_t item_sub_len = item_block_len / 2;
    xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
    p_rmt->sample_to_rmt((void *)src, p_rmt->tx_buf, src_size, item_block_len, &translated_size, &p_rmt->tx_len_rem);
    p_rmt->sample_size_remain = src_size - translated_size;
    p_rmt->sample_cur = src + translated_size;
    rmt_fill_memory(channel, p_rmt->tx_buf, p_rmt->tx_len_rem, 0);
    if (p_rmt->tx_len_rem == item_block_len) {
        rmt_set_tx_thr_intr_en(channel, 1, item_sub_len);
        p_rmt->tx_data = p_rmt->tx_buf;
        p_rmt->tx_offset = 0;
        p_rmt->tx_sub_len = item_sub_len;
        p_rmt->translator = true;
    } else {
        rmt_idle_level_t idle_level = rmt_ll_tx_get_idle_level(rmt_contex.hal.regs, channel);
        rmt_item32_t stop_data = (rmt_item32_t) {
            .level0 = idle_level,
            .duration0 = 0,
        };
        rmt_fill_memory(channel, &stop_data, 1, p_rmt->tx_len_rem);
        p_rmt->tx_len_rem = 0;
        p_rmt->sample_cur = NULL;
        p_rmt->translator = false;
    }
    rmt_tx_start(channel, true);
    p_rmt->wait_done = wait_tx_done;
    if (wait_tx_done) {
        xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
        xSemaphoreGive(p_rmt->tx_sem);
    }
    return ESP_OK;
}

esp_err_t rmt_get_channel_status(rmt_channel_status_result_t *channel_status)
{
    ESP_RETURN_ON_FALSE(channel_status, ESP_ERR_INVALID_ARG, TAG, RMT_PARAM_ERR_STR);
    for (int i = 0; i < RMT_CHANNEL_MAX; i++) {
        channel_status->status[i] = RMT_CHANNEL_UNINIT;
        if (p_rmt_obj[i]) {
            if (p_rmt_obj[i]->tx_sem) {
                if (xSemaphoreTake(p_rmt_obj[i]->tx_sem, (TickType_t)0) == pdTRUE) {
                    channel_status->status[i] = RMT_CHANNEL_IDLE;
                    xSemaphoreGive(p_rmt_obj[i]->tx_sem);
                } else {
                    channel_status->status[i] = RMT_CHANNEL_BUSY;
                }
            }
        }
    }
    return ESP_OK;
}

esp_err_t rmt_get_counter_clock(rmt_channel_t channel, uint32_t *clock_hz)
{
    ESP_RETURN_ON_FALSE(channel < RMT_CHANNEL_MAX, ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(clock_hz, ESP_ERR_INVALID_ARG, TAG, "parameter clock_hz can't be null");
    RMT_ENTER_CRITICAL();
    uint32_t rmt_source_clk_hz = 0;
#if SOC_RMT_CHANNEL_CLK_INDEPENDENT
    rmt_source_clk_hz = s_rmt_source_clock_hz[channel];
#else
    rmt_source_clk_hz = s_rmt_source_clock_hz;
#endif
    if (RMT_IS_RX_CHANNEL(channel)) {
        *clock_hz = rmt_source_clk_hz / rmt_ll_rx_get_channel_clock_div(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        *clock_hz = rmt_source_clk_hz / rmt_ll_tx_get_channel_clock_div(rmt_contex.hal.regs, channel);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

#if SOC_RMT_SUPPORT_TX_SYNCHRO
esp_err_t rmt_add_channel_to_group(rmt_channel_t channel)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_enable_sync(rmt_contex.hal.regs, true);
    rmt_contex.synchro_channel_mask |= (1 << channel);
    rmt_ll_tx_sync_group_add_channels(rmt_contex.hal.regs, 1 << channel);
    rmt_ll_tx_reset_channels_clock_div(rmt_contex.hal.regs, rmt_contex.synchro_channel_mask);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_remove_channel_from_group(rmt_channel_t channel)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    RMT_ENTER_CRITICAL();
    rmt_contex.synchro_channel_mask &= ~(1 << channel);
    rmt_ll_tx_sync_group_remove_channels(rmt_contex.hal.regs, 1 << channel);
    if (rmt_contex.synchro_channel_mask == 0) {
        rmt_ll_tx_enable_sync(rmt_contex.hal.regs, false);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}
#endif

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
esp_err_t rmt_set_tx_loop_count(rmt_channel_t channel, uint32_t count)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    ESP_RETURN_ON_FALSE(count <= RMT_LL_MAX_LOOP_COUNT_PER_BATCH, ESP_ERR_INVALID_ARG, TAG, "Invalid count value");
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_set_loop_count(rmt_contex.hal.regs, channel, count);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_enable_tx_loop_autostop(rmt_channel_t channel, bool en)
{
    ESP_RETURN_ON_FALSE(RMT_IS_TX_CHANNEL(channel), ESP_ERR_INVALID_ARG, TAG, RMT_CHANNEL_ERROR_STR);
    p_rmt_obj[channel]->loop_autostop = en;
#if SOC_RMT_SUPPORT_TX_LOOP_AUTO_STOP
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_enable_loop_autostop(rmt_contex.hal.regs, channel, en);
    RMT_EXIT_CRITICAL();
#endif
    return ESP_OK;
}
#endif

/**
 * @brief This function will be called during start up, to check that this legacy RMT driver is not running along with the new driver
 */
__attribute__((constructor))
static void check_rmt_legacy_driver_conflict(void)
{
    // This function was declared as weak here. The new RMT driver has one implementation.
    // So if the new RMT driver is not linked in, then `rmt_acquire_group_handle()` should be NULL at runtime.
    extern __attribute__((weak)) void *rmt_acquire_group_handle(int group_id);
    if ((void *)rmt_acquire_group_handle != NULL) {
        ESP_EARLY_LOGE(TAG, "CONFLICT! driver_ng is not allowed to be used with the legacy driver");
        abort();
    }
    ESP_EARLY_LOGW(TAG, "legacy driver is deprecated, please migrate to `driver/rmt_tx.h` and/or `driver/rmt_rx.h`");
}
