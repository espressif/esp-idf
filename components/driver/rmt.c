// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/cdefs.h>
#include "esp_compiler.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "driver/rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "soc/soc_memory_layout.h"
#include "soc/rmt_periph.h"
#include "soc/rtc.h"
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

static const char *RMT_TAG = "rmt";
#define RMT_CHECK(a, str, ret_val, ...)                                          \
    if (unlikely(!(a))) {                                                        \
        ESP_LOGE(RMT_TAG, "%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val);                                                        \
    }

// Spinlock for protecting concurrent register-level access only
#define RMT_ENTER_CRITICAL()  portENTER_CRITICAL_SAFE(&(rmt_contex.rmt_spinlock))
#define RMT_EXIT_CRITICAL()   portEXIT_CRITICAL_SAFE(&(rmt_contex.rmt_spinlock))

#define RMT_RX_CHANNEL_ENCODING_START (SOC_RMT_CHANNELS_PER_GROUP-SOC_RMT_TX_CANDIDATES_PER_GROUP)
#define RMT_TX_CHANNEL_ENCODING_END   (SOC_RMT_TX_CANDIDATES_PER_GROUP-1)

#define RMT_IS_RX_CHANNEL(channel) ((channel) >= RMT_RX_CHANNEL_ENCODING_START)
#define RMT_IS_TX_CHANNEL(channel) ((channel) <= RMT_TX_CHANNEL_ENCODING_END)
#define RMT_DECODE_RX_CHANNEL(encode_chan) ((encode_chan - RMT_RX_CHANNEL_ENCODING_START))
#define RMT_ENCODE_RX_CHANNEL(decode_chan) ((decode_chan + RMT_RX_CHANNEL_ENCODING_START))

typedef struct {
    rmt_hal_context_t hal;
    _lock_t rmt_driver_isr_lock;
    portMUX_TYPE rmt_spinlock; // Mutex lock for protecting concurrent register/unregister of RMT channels' ISR
    rmt_isr_handle_t rmt_driver_intr_handle;
    rmt_tx_end_callback_t rmt_tx_end_callback;// Event called when transmission is ended
    uint8_t rmt_driver_channels; // Bitmask of installed drivers' channels
    bool rmt_module_enabled;
    uint32_t synchro_channel_mask; // Bitmap of channels already added in the synchronous group
} rmt_contex_t;

typedef struct {
    size_t tx_offset;
    size_t tx_len_rem;
    size_t tx_sub_len;
    bool translator;
    bool wait_done; //Mark whether wait tx done.
    rmt_channel_t channel;
    const rmt_item32_t *tx_data;
    xSemaphoreHandle tx_sem;
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
    .hal.regs = RMT_LL_HW_BASE,
    .hal.mem = RMT_LL_MEM_BASE,
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

#if SOC_RMT_SOURCE_CLK_INDEPENDENT
static uint32_t s_rmt_source_clock_hz[RMT_CHANNEL_MAX];
#else
static uint32_t s_rmt_source_clock_hz;
#endif

//Enable RMT module
static void rmt_module_enable(void)
{
    RMT_ENTER_CRITICAL();
    if (rmt_contex.rmt_module_enabled == false) {
        periph_module_reset(rmt_periph_signals.module);
        periph_module_enable(rmt_periph_signals.module);
        rmt_contex.rmt_module_enabled = true;
    }
    RMT_EXIT_CRITICAL();
}

//Disable RMT module
static void rmt_module_disable(void)
{
    RMT_ENTER_CRITICAL();
    if (rmt_contex.rmt_module_enabled == true) {
        periph_module_disable(rmt_periph_signals.module);
        rmt_contex.rmt_module_enabled = false;
    }
    RMT_EXIT_CRITICAL();
}

esp_err_t rmt_set_clk_div(rmt_channel_t channel, uint8_t div_cnt)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
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
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(div_cnt != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
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
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_set_idle_thres(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), thresh);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_rx_idle_thresh(rmt_channel_t channel, uint16_t *thresh)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(thresh != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    *thresh = (uint16_t)rmt_ll_rx_get_idle_thres(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_mem_block_num(rmt_channel_t channel, uint8_t rmt_mem_num)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(rmt_mem_num <= RMT_CHANNEL_MAX - channel, RMT_MEM_CNT_ERROR_STR, ESP_ERR_INVALID_ARG);
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
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(rmt_mem_num != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
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
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(carrier_level < RMT_CARRIER_LEVEL_MAX, RMT_CARRIER_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_set_carrier_high_low_ticks(rmt_contex.hal.regs, channel, high_level, low_level);
    rmt_ll_tx_set_carrier_level(rmt_contex.hal.regs, channel, carrier_level);
    rmt_ll_tx_enable_carrier_modulation(rmt_contex.hal.regs, channel, carrier_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_mem_pd(rmt_channel_t channel, bool pd_en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_power_down_mem(rmt_contex.hal.regs, pd_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_mem_pd(rmt_channel_t channel, bool *pd_en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    *pd_en = rmt_ll_is_mem_power_down(rmt_contex.hal.regs);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_tx_start(rmt_channel_t channel, bool tx_idx_rst)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    if (tx_idx_rst) {
        rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    }
    rmt_ll_clear_tx_end_interrupt(rmt_contex.hal.regs, channel);
    // enable tx end interrupt in non-loop mode
    if (!rmt_ll_is_tx_loop_enabled(rmt_contex.hal.regs, channel)) {
        rmt_ll_enable_tx_end_interrupt(rmt_contex.hal.regs, channel, true);
    } else {
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
        rmt_ll_tx_reset_loop(rmt_contex.hal.regs, channel);
        rmt_ll_tx_enable_loop_count(rmt_contex.hal.regs, channel, true);
        rmt_ll_clear_tx_loop_interrupt(rmt_contex.hal.regs, channel);
        rmt_ll_enable_tx_loop_interrupt(rmt_contex.hal.regs, channel, true);
#endif
    }
    rmt_ll_tx_start(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_tx_stop(rmt_channel_t channel)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_stop(rmt_contex.hal.regs, channel);
    rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_rx_start(rmt_channel_t channel, bool rx_idx_rst)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_enable(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
    if (rx_idx_rst) {
        rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    }
    rmt_ll_clear_rx_end_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    rmt_ll_enable_rx_end_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), true);

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
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_enable_rx_end_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
    rmt_ll_rx_enable(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
    rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
#if SOC_RMT_SUPPORT_RX_PINGPONG
    rmt_ll_enable_rx_thres_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
#endif
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_tx_memory_reset(rmt_channel_t channel)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_rx_memory_reset(rmt_channel_t channel)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_memory_owner(rmt_channel_t channel, rmt_mem_owner_t owner)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(owner < RMT_MEM_OWNER_MAX, RMT_MEM_OWNER_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), owner);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_memory_owner(rmt_channel_t channel, rmt_mem_owner_t *owner)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(owner != NULL, RMT_MEM_OWNER_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    *owner = (rmt_mem_owner_t)rmt_ll_rx_get_mem_owner(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_loop_mode(rmt_channel_t channel, bool loop_en)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_enable_loop(rmt_contex.hal.regs, channel, loop_en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_tx_loop_mode(rmt_channel_t channel, bool *loop_en)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    *loop_en = rmt_ll_is_tx_loop_enabled(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_rx_filter(rmt_channel_t channel, bool rx_filter_en, uint8_t thresh)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_rx_enable_filter(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), rx_filter_en);
    rmt_ll_rx_set_filter_thres(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), thresh);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_source_clk(rmt_channel_t channel, rmt_source_clk_t base_clk)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(base_clk < RMT_BASECLK_MAX, RMT_BASECLK_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_set_group_clock_src(rmt_contex.hal.regs, channel, base_clk, 0, 0, 0);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_source_clk(rmt_channel_t channel, rmt_source_clk_t *src_clk)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    *src_clk = (rmt_source_clk_t)rmt_ll_get_group_clock_src(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_idle_level(rmt_channel_t channel, bool idle_out_en, rmt_idle_level_t level)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(level < RMT_IDLE_LEVEL_MAX, "RMT IDLE LEVEL ERR", ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_enable_idle(rmt_contex.hal.regs, channel, idle_out_en);
    rmt_ll_tx_set_idle_level(rmt_contex.hal.regs, channel, level);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_idle_level(rmt_channel_t channel, bool *idle_out_en, rmt_idle_level_t *level)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    *idle_out_en = rmt_ll_is_tx_idle_enabled(rmt_contex.hal.regs, channel);
    *level = rmt_ll_tx_get_idle_level(rmt_contex.hal.regs, channel);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_get_status(rmt_channel_t channel, uint32_t *status)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        *status = rmt_ll_rx_get_channel_status(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        *status = rmt_ll_tx_get_channel_status(rmt_contex.hal.regs, channel);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

void rmt_set_intr_enable_mask(uint32_t mask)
{
    RMT_ENTER_CRITICAL();
    rmt_ll_set_intr_enable_mask(mask);
    RMT_EXIT_CRITICAL();
}

void rmt_clr_intr_enable_mask(uint32_t mask)
{
    RMT_ENTER_CRITICAL();
    rmt_ll_clr_intr_enable_mask(mask);
    RMT_EXIT_CRITICAL();
}

esp_err_t rmt_set_rx_intr_en(rmt_channel_t channel, bool en)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_enable_rx_end_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

#if SOC_RMT_SUPPORT_RX_PINGPONG
esp_err_t rmt_set_rx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh)
{
    RMT_CHECK(RMT_IS_RX_CHANNEL(channel) && channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    if (en) {
        uint32_t item_block_len = rmt_ll_rx_get_mem_blocks(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel)) * RMT_MEM_ITEM_NUM;
        RMT_CHECK(evt_thresh <= item_block_len, "RMT EVT THRESH ERR", ESP_ERR_INVALID_ARG);
        RMT_ENTER_CRITICAL();
        rmt_ll_rx_set_limit(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), evt_thresh);
        rmt_ll_enable_rx_thres_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), true);
        RMT_EXIT_CRITICAL();
    } else {
        RMT_ENTER_CRITICAL();
        rmt_ll_enable_rx_thres_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), false);
        RMT_EXIT_CRITICAL();
    }
    return ESP_OK;
}
#endif

esp_err_t rmt_set_err_intr_en(rmt_channel_t channel, bool en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        rmt_ll_enable_rx_err_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), en);
    } else {
        rmt_ll_enable_tx_err_interrupt(rmt_contex.hal.regs, channel, en);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_intr_en(rmt_channel_t channel, bool en)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_enable_tx_end_interrupt(rmt_contex.hal.regs, channel, en);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_set_tx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    if (en) {
        uint32_t item_block_len = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel) * RMT_MEM_ITEM_NUM;
        RMT_CHECK(evt_thresh <= item_block_len, "RMT EVT THRESH ERR", ESP_ERR_INVALID_ARG);
        RMT_ENTER_CRITICAL();
        rmt_ll_tx_set_limit(rmt_contex.hal.regs, channel, evt_thresh);
        rmt_ll_enable_tx_thres_interrupt(rmt_contex.hal.regs, channel, true);
        RMT_EXIT_CRITICAL();
    } else {
        RMT_ENTER_CRITICAL();
        rmt_ll_enable_tx_thres_interrupt(rmt_contex.hal.regs, channel, false);
        RMT_EXIT_CRITICAL();
    }
    return ESP_OK;
}

esp_err_t rmt_set_gpio(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num, bool invert_signal)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(mode < RMT_MODE_MAX, RMT_MODE_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(((GPIO_IS_VALID_GPIO(gpio_num) && (mode == RMT_MODE_RX)) ||
               (GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) && (mode == RMT_MODE_TX))),
              RMT_GPIO_ERROR_STR, ESP_ERR_INVALID_ARG);

    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    if (mode == RMT_MODE_TX) {
        RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio_num, rmt_periph_signals.channels[channel].tx_sig, invert_signal, 0);
    } else {
        RMT_CHECK(RMT_IS_RX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        esp_rom_gpio_connect_in_signal(gpio_num, rmt_periph_signals.channels[channel].rx_sig, invert_signal);
    }
    return ESP_OK;
}

esp_err_t rmt_set_pin(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num)
{
    // only for backword compatibility
    return rmt_set_gpio(channel, mode, gpio_num, false);
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

    RMT_CHECK(rmt_is_channel_number_valid(channel, mode), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((mem_cnt + channel <= 8 && mem_cnt > 0), RMT_MEM_CNT_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((clk_div > 0), RMT_CLK_DIV_ERROR_STR, ESP_ERR_INVALID_ARG);

    if (mode == RMT_MODE_TX) {
        RMT_CHECK((!carrier_en || carrier_freq_hz > 0), "RMT carrier frequency can't be zero", ESP_ERR_INVALID_ARG);
    }

    RMT_ENTER_CRITICAL();
    rmt_ll_enable_mem_access(dev, true);

    if (rmt_param->flags & RMT_CHANNEL_FLAGS_AWARE_DFS) {
#if SOC_RMT_SUPPORT_XTAL
        // clock src: XTAL_CLK
        rmt_source_clk_hz = rtc_clk_xtal_freq_get() * 1000000;
        rmt_ll_set_group_clock_src(dev, channel, RMT_BASECLK_XTAL, 0, 0, 0);
#elif SOC_RMT_SUPPORT_REF_TICK
        // clock src: REF_CLK
        rmt_source_clk_hz = REF_CLK_FREQ;
        rmt_ll_set_group_clock_src(dev, channel, RMT_BASECLK_REF, 0, 0, 0);
#endif
    } else {
        // clock src: APB_CLK
        rmt_source_clk_hz = APB_CLK_FREQ;
        rmt_ll_set_group_clock_src(dev, channel, RMT_BASECLK_APB, 0, 0, 0);
    }
    RMT_EXIT_CRITICAL();

#if SOC_RMT_SOURCE_CLK_INDEPENDENT
    s_rmt_source_clock_hz[channel] = rmt_source_clk_hz;
#else
    if (s_rmt_source_clock_hz && rmt_source_clk_hz != s_rmt_source_clock_hz) {
        ESP_LOGW(RMT_TAG, "RMT clock source has been configured to %d by other channel, now reconfigure it to %d", s_rmt_source_clock_hz, rmt_source_clk_hz);
    }
    s_rmt_source_clock_hz = rmt_source_clk_hz;
#endif
    ESP_LOGD(RMT_TAG, "rmt_source_clk_hz: %d\n", rmt_source_clk_hz);

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
        rmt_ll_tx_enable_pingpong(dev, channel, true);
        /*Set idle level */
        rmt_ll_tx_enable_idle(dev, channel, rmt_param->tx_config.idle_output_en);
        rmt_ll_tx_set_idle_level(dev, channel, idle_level);
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
            rmt_ll_tx_set_carrier_high_low_ticks(dev, channel, 0, 0);
        }
        RMT_EXIT_CRITICAL();

        ESP_LOGD(RMT_TAG, "Rmt Tx Channel %u|Gpio %u|Sclk_Hz %u|Div %u|Carrier_Hz %u|Duty %u",
                 channel, gpio_num, rmt_source_clk_hz, clk_div, carrier_freq_hz, carrier_duty_percent);
    } else if (RMT_MODE_RX == mode) {
        uint8_t filter_cnt = rmt_param->rx_config.filter_ticks_thresh;
        uint16_t threshold = rmt_param->rx_config.idle_threshold;

        RMT_ENTER_CRITICAL();
        rmt_ll_rx_set_channel_clock_div(dev, RMT_DECODE_RX_CHANNEL(channel), clk_div);
        rmt_ll_rx_set_mem_blocks(dev, RMT_DECODE_RX_CHANNEL(channel), mem_cnt);
        rmt_ll_rx_reset_pointer(dev, RMT_DECODE_RX_CHANNEL(channel));
        rmt_ll_rx_set_mem_owner(dev, RMT_DECODE_RX_CHANNEL(channel), RMT_MEM_OWNER_HW);
        /*Set idle threshold*/
        rmt_ll_rx_set_idle_thres(dev, RMT_DECODE_RX_CHANNEL(channel), threshold);
        /* Set RX filter */
        rmt_ll_rx_set_filter_thres(dev, RMT_DECODE_RX_CHANNEL(channel), filter_cnt);
        rmt_ll_rx_enable_filter(dev, RMT_DECODE_RX_CHANNEL(channel), rmt_param->rx_config.filter_en);

#if SOC_RMT_SUPPORT_RX_PINGPONG
        /* always enable rx ping-pong */
        rmt_ll_rx_enable_pingpong(dev, RMT_DECODE_RX_CHANNEL(channel), true);
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

        ESP_LOGD(RMT_TAG, "Rmt Rx Channel %u|Gpio %u|Sclk_Hz %u|Div %u|Thresold %u|Filter %u",
                 channel, gpio_num, rmt_source_clk_hz, clk_div, threshold, filter_cnt);
    }

    return ESP_OK;
}

esp_err_t rmt_config(const rmt_config_t *rmt_param)
{
    rmt_module_enable();

    RMT_CHECK(rmt_set_gpio(rmt_param->channel, rmt_param->rmt_mode, rmt_param->gpio_num, rmt_param->flags & RMT_CHANNEL_FLAGS_INVERT_SIG) == ESP_OK,
              "set gpio for RMT driver failed", ESP_ERR_INVALID_ARG);

    RMT_CHECK(rmt_internal_config(&RMT, rmt_param) == ESP_OK,
              "initialize RMT driver failed", ESP_ERR_INVALID_ARG);

    return ESP_OK;
}

static void IRAM_ATTR rmt_fill_memory(rmt_channel_t channel, const rmt_item32_t *item,
                                      uint16_t item_num, uint16_t mem_offset)
{
    RMT_ENTER_CRITICAL();
    rmt_ll_write_memory(rmt_contex.hal.mem, channel, item, item_num, mem_offset);
    RMT_EXIT_CRITICAL();
}

esp_err_t rmt_fill_tx_items(rmt_channel_t channel, const rmt_item32_t *item, uint16_t item_num, uint16_t mem_offset)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, (0));
    RMT_CHECK((item != NULL), RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((item_num > 0), RMT_DRIVER_LENGTH_ERROR_STR, ESP_ERR_INVALID_ARG);

    /*Each block has 64 x 32 bits of data*/
    uint8_t mem_cnt = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    RMT_CHECK((mem_cnt * RMT_MEM_ITEM_NUM >= item_num), RMT_WR_MEM_OVF_ERROR_STR, ESP_ERR_INVALID_ARG);
    rmt_fill_memory(channel, item, item_num, mem_offset);
    return ESP_OK;
}

esp_err_t rmt_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, rmt_isr_handle_t *handle)
{
    RMT_CHECK((fn != NULL), RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(rmt_contex.rmt_driver_channels == 0, "RMT driver installed, can not install generic ISR handler", ESP_FAIL);

    return esp_intr_alloc(rmt_periph_signals.irq, intr_alloc_flags, fn, arg, handle);
}

esp_err_t rmt_isr_deregister(rmt_isr_handle_t handle)
{
    return esp_intr_free(handle);
}

static int IRAM_ATTR rmt_rx_get_mem_len_in_isr(rmt_channel_t channel)
{
    int block_num = rmt_ll_rx_get_mem_blocks(rmt_contex.hal.regs, channel);
    int item_block_len = block_num * RMT_MEM_ITEM_NUM;
    volatile rmt_item32_t *data = (rmt_item32_t *)RMTMEM.chan[RMT_ENCODE_RX_CHANNEL(channel)].data32;
    int idx;
    for (idx = 0; idx < item_block_len; idx++) {
        if (data[idx].duration0 == 0) {
            return idx;
        } else if (data[idx].duration1 == 0) {
            return idx + 1;
        }
    }
    return idx;
}

static void IRAM_ATTR rmt_driver_isr_default(void *arg)
{
    uint32_t status = 0;
    rmt_item32_t volatile *addr = NULL;
    uint8_t channel = 0;
    rmt_hal_context_t *hal = (rmt_hal_context_t *)arg;
    portBASE_TYPE HPTaskAwoken = pdFALSE;

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
            if (rmt_contex.rmt_tx_end_callback.function != NULL) {
                rmt_contex.rmt_tx_end_callback.function(channel, rmt_contex.rmt_tx_end_callback.arg);
            }
        }
        rmt_ll_clear_tx_end_interrupt(hal->regs, channel);
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
            if (len_rem >= p_rmt->tx_sub_len) {
                rmt_fill_memory(channel, pdata, p_rmt->tx_sub_len, p_rmt->tx_offset);
                p_rmt->tx_data += p_rmt->tx_sub_len;
                p_rmt->tx_len_rem -= p_rmt->tx_sub_len;
            } else if (len_rem == 0) {
                rmt_item32_t stop_data = {0};
                rmt_ll_write_memory(rmt_contex.hal.mem, channel, &stop_data, 1, p_rmt->tx_offset);
            } else {
                rmt_fill_memory(channel, pdata, len_rem, p_rmt->tx_offset);
                rmt_item32_t stop_data = {0};
                rmt_ll_write_memory(rmt_contex.hal.mem, channel, &stop_data, 1, p_rmt->tx_offset + len_rem);
                p_rmt->tx_data += len_rem;
                p_rmt->tx_len_rem -= len_rem;
            }
            if (p_rmt->tx_offset == 0) {
                p_rmt->tx_offset = p_rmt->tx_sub_len;
            } else {
                p_rmt->tx_offset = 0;
            }
        }
        rmt_ll_clear_tx_thres_interrupt(hal->regs, channel);
    }

    // Rx end interrupt
    status = rmt_ll_get_rx_end_interrupt_status(hal->regs);
    while (status) {
        channel = __builtin_ffs(status) - 1;
        status &= ~(1 << channel);
        rmt_obj_t *p_rmt = p_rmt_obj[RMT_ENCODE_RX_CHANNEL(channel)];
        if (p_rmt) {
            rmt_ll_rx_enable(rmt_contex.hal.regs, channel, false);
            int item_len = rmt_rx_get_mem_len_in_isr(channel);
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_MEM_OWNER_SW);
            if (p_rmt->rx_buf) {
                addr = RMTMEM.chan[RMT_ENCODE_RX_CHANNEL(channel)].data32;
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
                    ESP_EARLY_LOGE(RMT_TAG, "RMT RX BUFFER FULL");
                }
            } else {
                ESP_EARLY_LOGE(RMT_TAG, "RMT RX BUFFER ERROR");
            }

#if SOC_RMT_SUPPORT_RX_PINGPONG
            p_rmt->rx_item_start_idx = 0;
            p_rmt->rx_item_len = 0;
            memset((void *)p_rmt->rx_item_buf, 0, p_rmt->rx_item_buf_size);
#endif
            rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, channel);
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_MEM_OWNER_HW);
            rmt_ll_rx_enable(rmt_contex.hal.regs, channel, true);
        }
        rmt_ll_clear_rx_end_interrupt(hal->regs, channel);
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
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_MEM_OWNER_SW);
            memcpy((void *)(p_rmt->rx_item_buf + p_rmt->rx_item_len), (void *)(RMTMEM.chan[RMT_ENCODE_RX_CHANNEL(channel)].data32 + p_rmt->rx_item_start_idx), item_len * 4);
            rmt_ll_rx_set_mem_owner(rmt_contex.hal.regs, channel, RMT_MEM_OWNER_HW);
            p_rmt->rx_item_len += item_len;
            p_rmt->rx_item_start_idx += item_len;
            if (p_rmt->rx_item_start_idx >= mem_item_size) {
                p_rmt->rx_item_start_idx = 0;
            }
        } else {
            ESP_EARLY_LOGE(RMT_TAG, "---RX buffer too small: %d", sizeof(p_rmt->rx_item_buf));
        }
        rmt_ll_clear_rx_thres_interrupt(hal->regs, channel);
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
            xSemaphoreGiveFromISR(p_rmt->tx_sem, &HPTaskAwoken);
            if (rmt_contex.rmt_tx_end_callback.function != NULL) {
                rmt_contex.rmt_tx_end_callback.function(channel,  rmt_contex.rmt_tx_end_callback.arg);
            }
        }
        rmt_ll_clear_tx_loop_interrupt(hal->regs, channel);
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
            ESP_EARLY_LOGD(RMT_TAG, "RMT RX channel %d error", channel);
            ESP_EARLY_LOGD(RMT_TAG, "status: 0x%08x", rmt_ll_rx_get_channel_status(rmt_contex.hal.regs, channel));
        }
        rmt_ll_clear_rx_err_interrupt(hal->regs, channel);
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
            ESP_EARLY_LOGD(RMT_TAG, "RMT TX channel %d error", channel);
            ESP_EARLY_LOGD(RMT_TAG, "status: 0x%08x", rmt_ll_tx_get_channel_status(rmt_contex.hal.regs, channel));
        }
        rmt_ll_clear_tx_err_interrupt(hal->regs, channel);
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t rmt_driver_uninstall(rmt_channel_t channel)
{
    esp_err_t err = ESP_OK;
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((rmt_contex.rmt_driver_channels & BIT(channel)) != 0, "No RMT driver for this channel", ESP_ERR_INVALID_STATE);
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
        rmt_ll_enable_rx_end_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), 0);
        rmt_ll_enable_rx_err_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), 0);
#if SOC_RMT_SUPPORT_RX_PINGPONG
        rmt_ll_enable_rx_thres_interrupt(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel), 0);
#endif
    } else {
        rmt_ll_enable_tx_end_interrupt(rmt_contex.hal.regs, channel, 0);
        rmt_ll_enable_tx_err_interrupt(rmt_contex.hal.regs, channel, 0);
        rmt_ll_enable_tx_thres_interrupt(rmt_contex.hal.regs, channel, false);
    }
    RMT_EXIT_CRITICAL();

    _lock_acquire_recursive(&(rmt_contex.rmt_driver_isr_lock));
    rmt_contex.rmt_driver_channels &= ~BIT(channel);
    if (rmt_contex.rmt_driver_channels == 0) {
        rmt_module_disable();
        // all channels have driver disabled
        err = rmt_isr_deregister(rmt_contex.rmt_driver_intr_handle);
        rmt_contex.rmt_driver_intr_handle = NULL;
    }
    _lock_release_recursive(&(rmt_contex.rmt_driver_isr_lock));

    if (err != ESP_OK) {
        return err;
    }

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
    return ESP_OK;
}

esp_err_t rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((rmt_contex.rmt_driver_channels & BIT(channel)) == 0,
              "RMT driver already installed for channel", ESP_ERR_INVALID_STATE);

    esp_err_t err = ESP_OK;

    if (p_rmt_obj[channel] != NULL) {
        ESP_LOGD(RMT_TAG, "RMT driver already installed");
        return ESP_ERR_INVALID_STATE;
    }

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
        ESP_LOGE(RMT_TAG, "RMT driver malloc error");
        return ESP_ERR_NO_MEM;
    }

    p_rmt_obj[channel]->tx_len_rem = 0;
    p_rmt_obj[channel]->tx_data = NULL;
    p_rmt_obj[channel]->channel = channel;
    p_rmt_obj[channel]->tx_offset = 0;
    p_rmt_obj[channel]->tx_sub_len = 0;
    p_rmt_obj[channel]->wait_done = false;
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
            ESP_LOGE(RMT_TAG, "RMT malloc fail");
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
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    RMT_CHECK(rmt_item != NULL, RMT_ADDR_ERROR_STR, ESP_FAIL);
    RMT_CHECK(item_num > 0, RMT_DRIVER_LENGTH_ERROR_STR, ESP_ERR_INVALID_ARG);
#if CONFIG_SPIRAM_USE_MALLOC
    if (p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
        if (!esp_ptr_internal(rmt_item)) {
            ESP_LOGE(RMT_TAG, RMT_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    rmt_obj_t *p_rmt = p_rmt_obj[channel];
    int block_num = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel);
    int item_block_len = block_num * RMT_MEM_ITEM_NUM;
    int item_sub_len = block_num * RMT_MEM_ITEM_NUM / 2;
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
        rmt_item32_t stop_data = {0};
        rmt_ll_write_memory(rmt_contex.hal.mem, channel, &stop_data, 1, len_rem);
        p_rmt->tx_len_rem = 0;
    }
    rmt_tx_start(channel, true);
    p_rmt->wait_done = wait_tx_done;
    if (wait_tx_done) {
        // wait loop done
        if (rmt_ll_is_tx_loop_enabled(rmt_contex.hal.regs, channel)) {
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
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    if (xSemaphoreTake(p_rmt_obj[channel]->tx_sem, wait_time) == pdTRUE) {
        p_rmt_obj[channel]->wait_done = false;
        xSemaphoreGive(p_rmt_obj[channel]->tx_sem);
        return ESP_OK;
    } else {
        if (wait_time != 0) {
            // Don't emit error message if just polling.
            ESP_LOGE(RMT_TAG, "Timeout on wait_tx_done");
        }
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t rmt_get_ringbuf_handle(rmt_channel_t channel, RingbufHandle_t *buf_handle)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    RMT_CHECK(buf_handle != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
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
    RMT_CHECK(fn != NULL, RMT_TRANSLATOR_NULL_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    const uint32_t block_size = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel) *
                                RMT_MEM_ITEM_NUM * sizeof(rmt_item32_t);
    if (p_rmt_obj[channel]->tx_buf == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)malloc(block_size);
#else
        if (p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
            p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)malloc(block_size);
        } else {
            p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)heap_caps_calloc(1, block_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        }
#endif
        if (p_rmt_obj[channel]->tx_buf == NULL) {
            ESP_LOGE(RMT_TAG, "RMT translator buffer create fail");
            return ESP_FAIL;
        }
    }
    p_rmt_obj[channel]->sample_to_rmt = fn;
    p_rmt_obj[channel]->tx_context = NULL;
    p_rmt_obj[channel]->sample_size_remain = 0;
    p_rmt_obj[channel]->sample_cur = NULL;
    ESP_LOGD(RMT_TAG, "RMT translator init done");
    return ESP_OK;
}

esp_err_t rmt_translator_set_context(rmt_channel_t channel, void *context)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);

    p_rmt_obj[channel]->tx_context = context;
    return ESP_OK;
}

esp_err_t rmt_translator_get_context(const size_t *item_num, void **context)
{
    RMT_CHECK(item_num && context, "invalid arguments", ESP_ERR_INVALID_ARG);

    // the address of tx_len_rem is directlly passed to the callback,
    // so it's possible to get the object address from that
    rmt_obj_t *obj = __containerof(item_num, rmt_obj_t, tx_len_rem);
    *context = obj->tx_context;

    return ESP_OK;
}

esp_err_t rmt_write_sample(rmt_channel_t channel, const uint8_t *src, size_t src_size, bool wait_tx_done)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    RMT_CHECK(p_rmt_obj[channel]->sample_to_rmt != NULL, RMT_TRANSLATOR_UNINIT_STR, ESP_FAIL);
#if CONFIG_SPIRAM_USE_MALLOC
    if (p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM) {
        if (!esp_ptr_internal(src)) {
            ESP_LOGE(RMT_TAG, RMT_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    size_t translated_size = 0;
    rmt_obj_t *p_rmt = p_rmt_obj[channel];
    const uint32_t item_block_len = rmt_ll_tx_get_mem_blocks(rmt_contex.hal.regs, channel) * RMT_MEM_ITEM_NUM;
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
        rmt_item32_t stop_data = {0};
        rmt_ll_write_memory(rmt_contex.hal.mem, channel, &stop_data, 1, p_rmt->tx_len_rem);
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
    RMT_CHECK(channel_status != NULL, RMT_PARAM_ERR_STR, ESP_ERR_INVALID_ARG);
    for (int i = 0; i < RMT_CHANNEL_MAX; i++) {
        channel_status->status[i] = RMT_CHANNEL_UNINIT;
        if (p_rmt_obj[i] != NULL) {
            if (p_rmt_obj[i]->tx_sem != NULL) {
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
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(clock_hz, "parameter clock_hz can't be null", ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    uint32_t rmt_source_clk_hz = 0;
#if SOC_RMT_SOURCE_CLK_INDEPENDENT
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
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_enable_sync(rmt_contex.hal.regs, true);
    rmt_contex.synchro_channel_mask |= (1 << channel);
    rmt_ll_tx_add_to_sync_group(rmt_contex.hal.regs, channel);
    rmt_ll_tx_reset_channels_clock_div(rmt_contex.hal.regs, rmt_contex.synchro_channel_mask);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_remove_channel_from_group(rmt_channel_t channel)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_contex.synchro_channel_mask &= ~(1 << channel);
    rmt_ll_tx_remove_from_sync_group(rmt_contex.hal.regs, channel);
    if (rmt_contex.synchro_channel_mask == 0) {
        rmt_ll_tx_enable_sync(rmt_contex.hal.regs, false);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rmt_memory_rw_rst(rmt_channel_t channel)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    if (RMT_IS_RX_CHANNEL(channel)) {
        rmt_ll_rx_reset_pointer(rmt_contex.hal.regs, RMT_DECODE_RX_CHANNEL(channel));
    } else {
        rmt_ll_tx_reset_pointer(rmt_contex.hal.regs, channel);
    }
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}
#endif

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
esp_err_t rmt_set_tx_loop_count(rmt_channel_t channel, uint32_t count)
{
    RMT_CHECK(RMT_IS_TX_CHANNEL(channel), RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_ENTER_CRITICAL();
    rmt_ll_tx_set_loop_count(rmt_contex.hal.regs, channel, count);
    RMT_EXIT_CRITICAL();
    return ESP_OK;
}
#endif
