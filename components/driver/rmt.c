// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <esp_types.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/ringbuf.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/gpio_periph.h"
#include "driver/periph_ctrl.h"
#include "driver/rmt.h"
#include "soc/soc_memory_layout.h"

#include <sys/lock.h>

#define RMT_SOUCCE_CLK_APB (APB_CLK_FREQ) /*!< RMT source clock is APB_CLK */
#define RMT_SOURCE_CLK_REF (1 * 1000000)  /*!< not used yet */
#define RMT_SOURCE_CLK(select) ((select == RMT_BASECLK_REF) ? (RMT_SOURCE_CLK_REF) : (RMT_SOUCCE_CLK_APB)) /*! RMT source clock frequency */

#define RMT_CHANNEL_ERROR_STR  "RMT CHANNEL ERR"
#define RMT_ADDR_ERROR_STR     "RMT ADDRESS ERR"
#define RMT_MEM_CNT_ERROR_STR  "RMT MEM BLOCK NUM ERR"
#define RMT_CARRIER_ERROR_STR   "RMT CARRIER LEVEL ERR"
#define RMT_MEM_OWNER_ERROR_STR  "RMT MEM OWNER_ERR"
#define RMT_BASECLK_ERROR_STR    "RMT BASECLK ERR"
#define RMT_WR_MEM_OVF_ERROR_STR  "RMT WR MEM OVERFLOW"
#define RMT_GPIO_ERROR_STR        "RMT GPIO ERROR"
#define RMT_MODE_ERROR_STR        "RMT MODE ERROR"
#define RMT_CLK_DIV_ERROR_STR     "RMT CLK DIV ERR"
#define RMT_DRIVER_ERROR_STR      "RMT DRIVER ERR"
#define RMT_DRIVER_LENGTH_ERROR_STR  "RMT PARAM LEN ERROR"
#define RMT_PSRAM_BUFFER_WARN_STR    "Using buffer allocated from psram"
#define RMT_TRANSLATOR_NULL_STR    "RMT translator is null"
#define RMT_TRANSLATOR_UNINIT_STR  "RMT translator not init"
#define RMT_PARAM_ERR_STR          "RMT param error"

static const char* RMT_TAG = "rmt";
static uint8_t s_rmt_driver_channels; // Bitmask (bits 0-7) of installed drivers' channels
static rmt_isr_handle_t s_rmt_driver_intr_handle;

#define RMT_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(RMT_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

// Spinlock for protecting concurrent register-level access only
static portMUX_TYPE rmt_spinlock = portMUX_INITIALIZER_UNLOCKED;

// Mutex lock for protecting concurrent register/unregister of RMT channels' ISR
static _lock_t rmt_driver_isr_lock;

typedef struct {
    size_t tx_offset;
    size_t tx_len_rem;
    size_t tx_sub_len;
    bool translator;
    bool wait_done; //Mark whether wait tx done.
    rmt_channel_t channel;
    const rmt_item32_t* tx_data;
    xSemaphoreHandle tx_sem;
#if CONFIG_SPIRAM_USE_MALLOC
    int intr_alloc_flags;
    StaticSemaphore_t tx_sem_buffer;
#endif
    rmt_item32_t* tx_buf;
    RingbufHandle_t rx_buf;
    sample_to_rmt_t sample_to_rmt;
    size_t sample_size_remain;
    const uint8_t *sample_cur;
} rmt_obj_t;

rmt_obj_t* p_rmt_obj[RMT_CHANNEL_MAX] = {0};

// Event called when transmission is ended
static rmt_tx_end_callback_t rmt_tx_end_callback;

static void rmt_set_tx_wrap_en(bool en)
{
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.apb_conf.mem_tx_wrap_en = en;
    portEXIT_CRITICAL(&rmt_spinlock);
}

static void rmt_set_data_mode(rmt_data_mode_t data_mode)
{
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.apb_conf.fifo_mask = data_mode;
    portEXIT_CRITICAL(&rmt_spinlock);
}

esp_err_t rmt_set_clk_div(rmt_channel_t channel, uint8_t div_cnt)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT.conf_ch[channel].conf0.div_cnt = div_cnt;
    return ESP_OK;
}

esp_err_t rmt_get_clk_div(rmt_channel_t channel, uint8_t* div_cnt)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(div_cnt != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    *div_cnt = RMT.conf_ch[channel].conf0.div_cnt;
    return ESP_OK;
}

esp_err_t rmt_set_rx_idle_thresh(rmt_channel_t channel, uint16_t thresh)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT.conf_ch[channel].conf0.idle_thres = thresh;
    return ESP_OK;
}

esp_err_t rmt_get_rx_idle_thresh(rmt_channel_t channel, uint16_t *thresh)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(thresh != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    *thresh = RMT.conf_ch[channel].conf0.idle_thres;
    return ESP_OK;
}

esp_err_t rmt_set_mem_block_num(rmt_channel_t channel, uint8_t rmt_mem_num)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(rmt_mem_num <= RMT_CHANNEL_MAX - channel, RMT_MEM_CNT_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT.conf_ch[channel].conf0.mem_size = rmt_mem_num;
    return ESP_OK;
}

esp_err_t rmt_get_mem_block_num(rmt_channel_t channel, uint8_t* rmt_mem_num)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(rmt_mem_num != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    *rmt_mem_num = RMT.conf_ch[channel].conf0.mem_size;
    return ESP_OK;
}

esp_err_t rmt_set_tx_carrier(rmt_channel_t channel, bool carrier_en, uint16_t high_level, uint16_t low_level,
    rmt_carrier_level_t carrier_level)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(carrier_level < RMT_CARRIER_LEVEL_MAX, RMT_CARRIER_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT.carrier_duty_ch[channel].high = high_level;
    RMT.carrier_duty_ch[channel].low = low_level;
    RMT.conf_ch[channel].conf0.carrier_out_lv = carrier_level;
    RMT.conf_ch[channel].conf0.carrier_en = carrier_en;
    return ESP_OK;
}

esp_err_t rmt_set_mem_pd(rmt_channel_t channel, bool pd_en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT.conf_ch[channel].conf0.mem_pd = pd_en;
    return ESP_OK;
}

esp_err_t rmt_get_mem_pd(rmt_channel_t channel, bool* pd_en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    *pd_en = (bool) RMT.conf_ch[channel].conf0.mem_pd;
    return ESP_OK;
}

esp_err_t rmt_tx_start(rmt_channel_t channel, bool tx_idx_rst)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    if(tx_idx_rst) {
        RMT.conf_ch[channel].conf1.mem_rd_rst = 1;
    }
    RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_TX;
    RMT.conf_ch[channel].conf1.tx_start = 1;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_tx_stop(rmt_channel_t channel)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMTMEM.chan[channel].data32[0].val = 0;
    RMT.conf_ch[channel].conf1.tx_start = 0;
    RMT.conf_ch[channel].conf1.mem_rd_rst = 1;
    RMT.conf_ch[channel].conf1.mem_rd_rst = 0;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_rx_start(rmt_channel_t channel, bool rx_idx_rst)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    if(rx_idx_rst) {
        RMT.conf_ch[channel].conf1.mem_wr_rst = 1;
    }
    RMT.conf_ch[channel].conf1.rx_en = 0;
    RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_RX;
    RMT.conf_ch[channel].conf1.rx_en = 1;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_rx_stop(rmt_channel_t channel)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.rx_en = 0;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_memory_rw_rst(rmt_channel_t channel)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.mem_rd_rst = 1;
    RMT.conf_ch[channel].conf1.mem_wr_rst = 1;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_set_memory_owner(rmt_channel_t channel, rmt_mem_owner_t owner)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(owner < RMT_MEM_OWNER_MAX, RMT_MEM_OWNER_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.mem_owner = owner;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_get_memory_owner(rmt_channel_t channel, rmt_mem_owner_t* owner)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(owner != NULL, RMT_MEM_OWNER_ERROR_STR, ESP_ERR_INVALID_ARG);
    *owner = (rmt_mem_owner_t) RMT.conf_ch[channel].conf1.mem_owner;
    return ESP_OK;
}

esp_err_t rmt_set_tx_loop_mode(rmt_channel_t channel, bool loop_en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.tx_conti_mode = loop_en;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_get_tx_loop_mode(rmt_channel_t channel, bool* loop_en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    *loop_en = (bool) RMT.conf_ch[channel].conf1.tx_conti_mode;
    return ESP_OK;
}

esp_err_t rmt_set_rx_filter(rmt_channel_t channel, bool rx_filter_en, uint8_t thresh)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.rx_filter_en = rx_filter_en;
    RMT.conf_ch[channel].conf1.rx_filter_thres = thresh;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_set_source_clk(rmt_channel_t channel, rmt_source_clk_t base_clk)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(base_clk < RMT_BASECLK_MAX, RMT_BASECLK_ERROR_STR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.ref_always_on = base_clk;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_get_source_clk(rmt_channel_t channel, rmt_source_clk_t* src_clk)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    *src_clk = (rmt_source_clk_t) (RMT.conf_ch[channel].conf1.ref_always_on);
    return ESP_OK;
}

esp_err_t rmt_set_idle_level(rmt_channel_t channel, bool idle_out_en, rmt_idle_level_t level)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(level < RMT_IDLE_LEVEL_MAX, "RMT IDLE LEVEL ERR", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.idle_out_en = idle_out_en;
    RMT.conf_ch[channel].conf1.idle_out_lv = level;
    portEXIT_CRITICAL(&rmt_spinlock);
    return ESP_OK;
}

esp_err_t rmt_get_idle_level(rmt_channel_t channel, bool* idle_out_en, rmt_idle_level_t* level)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    *idle_out_en = (bool) (RMT.conf_ch[channel].conf1.idle_out_en);
    *level = (rmt_idle_level_t) (RMT.conf_ch[channel].conf1.idle_out_lv);
    return ESP_OK;
}

esp_err_t rmt_get_status(rmt_channel_t channel, uint32_t* status)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    *status = RMT.status_ch[channel];
    return ESP_OK;
}

rmt_data_mode_t rmt_get_data_mode(void)
{
    return (rmt_data_mode_t) (RMT.apb_conf.fifo_mask);
}

void rmt_set_intr_enable_mask(uint32_t mask)
{
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.int_ena.val |= mask;
    portEXIT_CRITICAL(&rmt_spinlock);
}

void rmt_clr_intr_enable_mask(uint32_t mask)
{
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.int_ena.val &= (~mask);
    portEXIT_CRITICAL(&rmt_spinlock);
}

esp_err_t rmt_set_rx_intr_en(rmt_channel_t channel, bool en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    if(en) {
        rmt_set_intr_enable_mask(BIT(channel * 3 + 1));
    } else {
        rmt_clr_intr_enable_mask(BIT(channel * 3 + 1));
    }
    return ESP_OK;
}

esp_err_t rmt_set_err_intr_en(rmt_channel_t channel, bool en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    if(en) {
        rmt_set_intr_enable_mask(BIT(channel * 3 + 2));
    } else {
        rmt_clr_intr_enable_mask(BIT(channel * 3 + 2));
    }
    return ESP_OK;
}

esp_err_t rmt_set_tx_intr_en(rmt_channel_t channel, bool en)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    if(en) {
        rmt_set_intr_enable_mask(BIT(channel * 3));
    } else {
        rmt_clr_intr_enable_mask(BIT(channel * 3));
    }
    return ESP_OK;
}

esp_err_t rmt_set_tx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    if(en) {
        RMT_CHECK(evt_thresh <= 256, "RMT EVT THRESH ERR", ESP_ERR_INVALID_ARG);
        portENTER_CRITICAL(&rmt_spinlock);
        RMT.tx_lim_ch[channel].limit = evt_thresh;
        portEXIT_CRITICAL(&rmt_spinlock);
        rmt_set_tx_wrap_en(true);
        rmt_set_intr_enable_mask(BIT(channel + 24));
    } else {
        rmt_clr_intr_enable_mask(BIT(channel + 24));
    }
    return ESP_OK;
}

esp_err_t rmt_set_pin(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(mode < RMT_MODE_MAX, RMT_MODE_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(((GPIO_IS_VALID_GPIO(gpio_num) && (mode == RMT_MODE_RX)) || (GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) && (mode == RMT_MODE_TX))),
        RMT_GPIO_ERROR_STR, ESP_ERR_INVALID_ARG);

    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], 2);
    if(mode == RMT_MODE_TX) {
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        gpio_matrix_out(gpio_num, RMT_SIG_OUT0_IDX + channel, 0, 0);
    } else {
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        gpio_matrix_in(gpio_num, RMT_SIG_IN0_IDX + channel, 0);
    }
    return ESP_OK;
}

esp_err_t rmt_config(const rmt_config_t* rmt_param)
{
    uint8_t mode = rmt_param->rmt_mode;
    uint8_t channel = rmt_param->channel;
    uint8_t gpio_num = rmt_param->gpio_num;
    uint8_t mem_cnt = rmt_param->mem_block_num;
    int clk_div = rmt_param->clk_div;
    uint32_t carrier_freq_hz = rmt_param->tx_config.carrier_freq_hz;
    bool carrier_en = rmt_param->tx_config.carrier_en;
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(GPIO_IS_VALID_GPIO(gpio_num), RMT_GPIO_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((mem_cnt + channel <= 8 && mem_cnt > 0), RMT_MEM_CNT_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((clk_div > 0), RMT_CLK_DIV_ERROR_STR, ESP_ERR_INVALID_ARG);
    if (mode == RMT_MODE_TX) {
        RMT_CHECK((!carrier_en || carrier_freq_hz > 0), "RMT carrier frequency can't be zero", ESP_ERR_INVALID_ARG);
    }

    static bool rmt_enable = false;
    if (rmt_enable == false) {
        periph_module_reset(PERIPH_RMT_MODULE);
        rmt_enable = true;
    }
    periph_module_enable(PERIPH_RMT_MODULE);

    RMT.conf_ch[channel].conf0.div_cnt = clk_div;
    /*Visit data use memory not FIFO*/
    rmt_set_data_mode(RMT_DATA_MODE_MEM);
    /*Reset tx/rx memory index */
    portENTER_CRITICAL(&rmt_spinlock);
    RMT.conf_ch[channel].conf1.mem_rd_rst = 1;
    RMT.conf_ch[channel].conf1.mem_wr_rst = 1;
    portEXIT_CRITICAL(&rmt_spinlock);

    if(mode == RMT_MODE_TX) {
        uint32_t rmt_source_clk_hz = 0;
        uint16_t carrier_duty_percent = rmt_param->tx_config.carrier_duty_percent;
        uint8_t carrier_level = rmt_param->tx_config.carrier_level;
        uint8_t idle_level = rmt_param->tx_config.idle_level;

        portENTER_CRITICAL(&rmt_spinlock);
        RMT.conf_ch[channel].conf1.tx_conti_mode = rmt_param->tx_config.loop_en;
        /*Memory set block number*/
        RMT.conf_ch[channel].conf0.mem_size = mem_cnt;
        RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_TX;
        /*We use APB clock in this version, which is 80Mhz, later we will release system reference clock*/
        RMT.conf_ch[channel].conf1.ref_always_on = RMT_BASECLK_APB;
        rmt_source_clk_hz = RMT_SOURCE_CLK(RMT_BASECLK_APB);
        /*Set idle level */
        RMT.conf_ch[channel].conf1.idle_out_en = rmt_param->tx_config.idle_output_en;
        RMT.conf_ch[channel].conf1.idle_out_lv = idle_level;
        /*Set carrier*/
        RMT.conf_ch[channel].conf0.carrier_en = carrier_en;
        if (carrier_en) {
            uint32_t duty_div, duty_h, duty_l;
            duty_div = rmt_source_clk_hz / carrier_freq_hz;
            duty_h = duty_div * carrier_duty_percent / 100;
            duty_l = duty_div - duty_h;
            RMT.conf_ch[channel].conf0.carrier_out_lv = carrier_level;
            RMT.carrier_duty_ch[channel].high = duty_h;
            RMT.carrier_duty_ch[channel].low = duty_l;
        } else {
            RMT.conf_ch[channel].conf0.carrier_out_lv = 0;
            RMT.carrier_duty_ch[channel].high = 0;
            RMT.carrier_duty_ch[channel].low = 0;
        }
        portEXIT_CRITICAL(&rmt_spinlock);

        ESP_LOGD(RMT_TAG, "Rmt Tx Channel %u|Gpio %u|Sclk_Hz %u|Div %u|Carrier_Hz %u|Duty %u",
                 channel, gpio_num, rmt_source_clk_hz, clk_div, carrier_freq_hz, carrier_duty_percent);

    }
    else if(RMT_MODE_RX == mode) {
        uint8_t filter_cnt = rmt_param->rx_config.filter_ticks_thresh;
        uint16_t threshold = rmt_param->rx_config.idle_threshold;

        portENTER_CRITICAL(&rmt_spinlock);
        /*clock init*/
        RMT.conf_ch[channel].conf1.ref_always_on = RMT_BASECLK_APB;
        uint32_t rmt_source_clk_hz = RMT_SOURCE_CLK(RMT_BASECLK_APB);
        /*memory set block number and owner*/
        RMT.conf_ch[channel].conf0.mem_size = mem_cnt;
        RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_RX;
        /*Set idle threshold*/
        RMT.conf_ch[channel].conf0.idle_thres = threshold;
        /* Set RX filter */
        RMT.conf_ch[channel].conf1.rx_filter_thres = filter_cnt;
        RMT.conf_ch[channel].conf1.rx_filter_en = rmt_param->rx_config.filter_en;
        portEXIT_CRITICAL(&rmt_spinlock);

        ESP_LOGD(RMT_TAG, "Rmt Rx Channel %u|Gpio %u|Sclk_Hz %u|Div %u|Thresold %u|Filter %u",
            channel, gpio_num, rmt_source_clk_hz, clk_div, threshold, filter_cnt);
    }
    rmt_set_pin(channel, mode, gpio_num);
    return ESP_OK;
}

static void IRAM_ATTR rmt_fill_memory(rmt_channel_t channel, const rmt_item32_t* item, uint16_t item_num, uint16_t mem_offset)
{
    portENTER_CRITICAL_SAFE(&rmt_spinlock);
    RMT.apb_conf.fifo_mask = RMT_DATA_MODE_MEM;
    portEXIT_CRITICAL_SAFE(&rmt_spinlock);
    int i;
    for(i = 0; i < item_num; i++) {
        RMTMEM.chan[channel].data32[i + mem_offset].val = item[i].val;
    }
}

esp_err_t rmt_fill_tx_items(rmt_channel_t channel, const rmt_item32_t* item, uint16_t item_num, uint16_t mem_offset)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, (0));
    RMT_CHECK((item != NULL), RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((item_num > 0), RMT_DRIVER_LENGTH_ERROR_STR, ESP_ERR_INVALID_ARG);

    /*Each block has 64 x 32 bits of data*/
    uint8_t mem_cnt = RMT.conf_ch[channel].conf0.mem_size;
    RMT_CHECK((mem_cnt * RMT_MEM_ITEM_NUM >= item_num), RMT_WR_MEM_OVF_ERROR_STR, ESP_ERR_INVALID_ARG);
    rmt_fill_memory(channel, item, item_num, mem_offset);
    return ESP_OK;
}

esp_err_t rmt_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, rmt_isr_handle_t *handle)
{
    RMT_CHECK((fn != NULL), RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(s_rmt_driver_channels == 0, "RMT driver installed, can not install generic ISR handler", ESP_FAIL);

    return esp_intr_alloc(ETS_RMT_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
}


esp_err_t rmt_isr_deregister(rmt_isr_handle_t handle)
{
    return esp_intr_free(handle);
}

static int IRAM_ATTR rmt_get_mem_len(rmt_channel_t channel)
{
    int block_num = RMT.conf_ch[channel].conf0.mem_size;
    int item_block_len = block_num * RMT_MEM_ITEM_NUM;
    volatile rmt_item32_t* data = RMTMEM.chan[channel].data32;
    int idx;
    for(idx = 0; idx < item_block_len; idx++) {
        if(data[idx].duration0 == 0) {
            return idx;
        } else if(data[idx].duration1 == 0) {
            return idx + 1;
        }
    }
    return idx;
}

static void IRAM_ATTR rmt_driver_isr_default(void* arg)
{
    const uint32_t intr_st = RMT.int_st.val;
    uint32_t status = intr_st;
    uint8_t channel;
    portBASE_TYPE HPTaskAwoken = 0;
    while (status) {
        int i = __builtin_ffs(status) - 1;
        status &= ~(1 << i);
        if(i < 24) {
            channel = i / 3;
            rmt_obj_t* p_rmt = p_rmt_obj[channel];
            if(NULL == p_rmt) {
                continue;
            }
            switch(i % 3) {
                //TX END
                case 0:
                    xSemaphoreGiveFromISR(p_rmt->tx_sem, &HPTaskAwoken);
                    RMT.conf_ch[channel].conf1.mem_rd_rst = 1;
                    RMT.conf_ch[channel].conf1.mem_rd_rst = 0;
                    p_rmt->tx_data = NULL;
                    p_rmt->tx_len_rem = 0;
                    p_rmt->tx_offset = 0;
                    p_rmt->tx_sub_len = 0;
                    p_rmt->sample_cur = NULL;
                    p_rmt->translator = false;
                    if(rmt_tx_end_callback.function != NULL) {
                        rmt_tx_end_callback.function(channel, rmt_tx_end_callback.arg);
                    }
                    break;
                    //RX_END
                case 1:
                    RMT.conf_ch[channel].conf1.rx_en = 0;
                    int item_len = rmt_get_mem_len(channel);
                    //change memory owner to protect data.
                    RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_TX;
                    if(p_rmt->rx_buf) {
                        BaseType_t res = xRingbufferSendFromISR(p_rmt->rx_buf, (void*) RMTMEM.chan[channel].data32, item_len * 4, &HPTaskAwoken);
                        if(res == pdFALSE) {
                            ESP_EARLY_LOGE(RMT_TAG, "RMT RX BUFFER FULL");
                        } else {

                        }
                    } else {
                        ESP_EARLY_LOGE(RMT_TAG, "RMT RX BUFFER ERROR\n");
                    }
                    RMT.conf_ch[channel].conf1.mem_wr_rst = 1;
                    RMT.conf_ch[channel].conf1.mem_owner = RMT_MEM_OWNER_RX;
                    RMT.conf_ch[channel].conf1.rx_en = 1;
                    break;
                    //ERR
                case 2:
                    ESP_EARLY_LOGE(RMT_TAG, "RMT[%d] ERR", channel);
                    ESP_EARLY_LOGE(RMT_TAG, "status: 0x%08x", RMT.status_ch[channel]);
                    RMT.int_ena.val &= (~(BIT(i)));
                    break;
                default:
                    break;
            }
        } else {
            channel = i - 24;
            rmt_obj_t* p_rmt = p_rmt_obj[channel];

            if(p_rmt->tx_data == NULL) {
                //skip
            } else {
                if(p_rmt->translator) {
                    if(p_rmt->sample_size_remain > 0) {
                        size_t translated_size = 0;
                        p_rmt->sample_to_rmt((void *) p_rmt->sample_cur,
                                             p_rmt->tx_buf,
                                             p_rmt->sample_size_remain,
                                             p_rmt->tx_sub_len,
                                             &translated_size,
                                             &p_rmt->tx_len_rem
                                            );
                        p_rmt->sample_size_remain -= translated_size;
                        p_rmt->sample_cur += translated_size;
                        p_rmt->tx_data = p_rmt->tx_buf;
                    } else {
                        p_rmt->sample_cur = NULL;
                        p_rmt->translator = false;
                    }
                }
                const rmt_item32_t* pdata = p_rmt->tx_data;
                int len_rem = p_rmt->tx_len_rem;
                if(len_rem >= p_rmt->tx_sub_len) {
                    rmt_fill_memory(channel, pdata, p_rmt->tx_sub_len, p_rmt->tx_offset);
                    p_rmt->tx_data += p_rmt->tx_sub_len;
                    p_rmt->tx_len_rem -= p_rmt->tx_sub_len;
                } else if(len_rem == 0) {
                    RMTMEM.chan[channel].data32[p_rmt->tx_offset].val = 0;
                } else {
                    rmt_fill_memory(channel, pdata, len_rem, p_rmt->tx_offset);
                    RMTMEM.chan[channel].data32[p_rmt->tx_offset + len_rem].val = 0;
                    p_rmt->tx_data += len_rem;
                    p_rmt->tx_len_rem -= len_rem;
                }
                if(p_rmt->tx_offset == 0) {
                    p_rmt->tx_offset = p_rmt->tx_sub_len;
                } else {
                    p_rmt->tx_offset = 0;
                }
            }
        }
    }
    RMT.int_clr.val = intr_st;
    if(HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t rmt_driver_uninstall(rmt_channel_t channel)
{
    esp_err_t err = ESP_OK;
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((s_rmt_driver_channels & BIT(channel)) != 0, "No RMT driver for this channel", ESP_ERR_INVALID_STATE);
    if(p_rmt_obj[channel] == NULL) {
        return ESP_OK;
    }
    //Avoid blocking here(when the interrupt is disabled and do not wait tx done).
    if(p_rmt_obj[channel]->wait_done) {
        xSemaphoreTake(p_rmt_obj[channel]->tx_sem, portMAX_DELAY);
    }
    rmt_set_rx_intr_en(channel, 0);
    rmt_set_err_intr_en(channel, 0);
    rmt_set_tx_intr_en(channel, 0);
    rmt_set_tx_thr_intr_en(channel, 0, 0xffff);

    _lock_acquire_recursive(&rmt_driver_isr_lock);

    s_rmt_driver_channels &= ~BIT(channel);
    if (s_rmt_driver_channels == 0) { // all channels have driver disabled
        err = rmt_isr_deregister(s_rmt_driver_intr_handle);
        s_rmt_driver_intr_handle = NULL;
    }

    _lock_release_recursive(&rmt_driver_isr_lock);

    if (err != ESP_OK) {
        return err;
    }

    if(p_rmt_obj[channel]->tx_sem) {
        vSemaphoreDelete(p_rmt_obj[channel]->tx_sem);
        p_rmt_obj[channel]->tx_sem = NULL;
    }
    if(p_rmt_obj[channel]->rx_buf) {
        vRingbufferDelete(p_rmt_obj[channel]->rx_buf);
        p_rmt_obj[channel]->rx_buf = NULL;
    }
    if(p_rmt_obj[channel]->tx_buf) {
        free(p_rmt_obj[channel]->tx_buf);
        p_rmt_obj[channel]->tx_buf = NULL;
    }
    if(p_rmt_obj[channel]->sample_to_rmt) {
        p_rmt_obj[channel]->sample_to_rmt = NULL;
    }

    free(p_rmt_obj[channel]);
    p_rmt_obj[channel] = NULL;
    return ESP_OK;
}

esp_err_t rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK((s_rmt_driver_channels & BIT(channel)) == 0, "RMT driver already installed for channel", ESP_ERR_INVALID_STATE);

    esp_err_t err = ESP_OK;

    if(p_rmt_obj[channel] != NULL) {
        ESP_LOGD(RMT_TAG, "RMT driver already installed");
        return ESP_ERR_INVALID_STATE;
    }

#if !CONFIG_SPIRAM_USE_MALLOC
    p_rmt_obj[channel] = (rmt_obj_t*) malloc(sizeof(rmt_obj_t));
#else
    if( !(intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
        p_rmt_obj[channel] = (rmt_obj_t*) malloc(sizeof(rmt_obj_t));
    } else {
        p_rmt_obj[channel] = (rmt_obj_t*) heap_caps_calloc(1, sizeof(rmt_obj_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    }
#endif

    if(p_rmt_obj[channel] == NULL) {
        ESP_LOGE(RMT_TAG, "RMT driver malloc error");
        return ESP_ERR_NO_MEM;
    }
    memset(p_rmt_obj[channel], 0, sizeof(rmt_obj_t));

    p_rmt_obj[channel]->tx_len_rem = 0;
    p_rmt_obj[channel]->tx_data = NULL;
    p_rmt_obj[channel]->channel = channel;
    p_rmt_obj[channel]->tx_offset = 0;
    p_rmt_obj[channel]->tx_sub_len = 0;
    p_rmt_obj[channel]->wait_done = false;
    p_rmt_obj[channel]->translator = false;
    p_rmt_obj[channel]->sample_to_rmt = NULL;
    if(p_rmt_obj[channel]->tx_sem == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        p_rmt_obj[channel]->tx_sem = xSemaphoreCreateBinary();
#else
        p_rmt_obj[channel]->intr_alloc_flags = intr_alloc_flags;
        if( !(intr_alloc_flags & ESP_INTR_FLAG_IRAM) ) {
            p_rmt_obj[channel]->tx_sem = xSemaphoreCreateBinary();
        } else {
            p_rmt_obj[channel]->tx_sem = xSemaphoreCreateBinaryStatic(&p_rmt_obj[channel]->tx_sem_buffer);
        }
#endif
        xSemaphoreGive(p_rmt_obj[channel]->tx_sem);
    }
    if(p_rmt_obj[channel]->rx_buf == NULL && rx_buf_size > 0) {
        p_rmt_obj[channel]->rx_buf = xRingbufferCreate(rx_buf_size, RINGBUF_TYPE_NOSPLIT);
        rmt_set_rx_intr_en(channel, 1);
        rmt_set_err_intr_en(channel, 1);
    }

    _lock_acquire_recursive(&rmt_driver_isr_lock);

    if(s_rmt_driver_channels == 0) { // first RMT channel using driver
        err = rmt_isr_register(rmt_driver_isr_default, NULL, intr_alloc_flags, &s_rmt_driver_intr_handle);
    }
    if (err == ESP_OK) {
        s_rmt_driver_channels |= BIT(channel);
        rmt_set_tx_intr_en(channel, 1);
    }

    _lock_release_recursive(&rmt_driver_isr_lock);

    return err;
}

esp_err_t rmt_write_items(rmt_channel_t channel, const rmt_item32_t* rmt_item, int item_num, bool wait_tx_done)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    RMT_CHECK(rmt_item != NULL, RMT_ADDR_ERROR_STR, ESP_FAIL);
    RMT_CHECK(item_num > 0, RMT_DRIVER_LENGTH_ERROR_STR, ESP_ERR_INVALID_ARG);
#if CONFIG_SPIRAM_USE_MALLOC
    if( p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM ) {
        if( !esp_ptr_internal(rmt_item) ) {
            ESP_LOGE(RMT_TAG, RMT_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    rmt_obj_t* p_rmt = p_rmt_obj[channel];
    int block_num = RMT.conf_ch[channel].conf0.mem_size;
    int item_block_len = block_num * RMT_MEM_ITEM_NUM;
    int item_sub_len = block_num * RMT_MEM_ITEM_NUM / 2;
    int len_rem = item_num;
    xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
    // fill the memory block first
    if(item_num >= item_block_len) {
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
        RMTMEM.chan[channel].data32[len_rem].val = 0;
        p_rmt->tx_len_rem = 0;
    }
    rmt_tx_start(channel, true);
    p_rmt->wait_done = wait_tx_done;
    if(wait_tx_done) {
        xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
        xSemaphoreGive(p_rmt->tx_sem);
    }
    return ESP_OK;
}

esp_err_t rmt_wait_tx_done(rmt_channel_t channel, TickType_t wait_time)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    if(xSemaphoreTake(p_rmt_obj[channel]->tx_sem, wait_time) == pdTRUE) {
        p_rmt_obj[channel]->wait_done = false;
        xSemaphoreGive(p_rmt_obj[channel]->tx_sem);
        return ESP_OK;
    }
    else {
        if (wait_time != 0) {  // Don't emit error message if just polling.
            ESP_LOGE(RMT_TAG, "Timeout on wait_tx_done");
        }
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t rmt_get_ringbuf_handle(rmt_channel_t channel, RingbufHandle_t* buf_handle)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    RMT_CHECK(buf_handle != NULL, RMT_ADDR_ERROR_STR, ESP_ERR_INVALID_ARG);
    *buf_handle = p_rmt_obj[channel]->rx_buf;
    return ESP_OK;
}

rmt_tx_end_callback_t rmt_register_tx_end_callback(rmt_tx_end_fn_t function, void *arg)
{
    rmt_tx_end_callback_t previous = rmt_tx_end_callback;
    rmt_tx_end_callback.function = function;
    rmt_tx_end_callback.arg = arg;
    return previous;
}

esp_err_t rmt_translator_init(rmt_channel_t channel, sample_to_rmt_t fn)
{
    RMT_CHECK(fn != NULL, RMT_TRANSLATOR_NULL_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    const uint32_t block_size = RMT.conf_ch[channel].conf0.mem_size * RMT_MEM_ITEM_NUM * sizeof(rmt_item32_t);
    if (p_rmt_obj[channel]->tx_buf == NULL) {
#if !CONFIG_SPIRAM_USE_MALLOC
        p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)malloc(block_size);
#else
        if( p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM ) {
            p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)malloc(block_size);
        } else {
            p_rmt_obj[channel]->tx_buf = (rmt_item32_t *)heap_caps_calloc(1, block_size, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        }
#endif
        if(p_rmt_obj[channel]->tx_buf == NULL) {
            ESP_LOGE(RMT_TAG, "RMT translator buffer create fail");
            return ESP_FAIL;
        }
    }
    p_rmt_obj[channel]->sample_to_rmt = fn;
    p_rmt_obj[channel]->sample_size_remain = 0;
    p_rmt_obj[channel]->sample_cur = NULL;
    ESP_LOGD(RMT_TAG, "RMT translator init done");
    return ESP_OK;
}

esp_err_t rmt_write_sample(rmt_channel_t channel, const uint8_t *src, size_t src_size, bool wait_tx_done)
{
    RMT_CHECK(channel < RMT_CHANNEL_MAX, RMT_CHANNEL_ERROR_STR, ESP_ERR_INVALID_ARG);
    RMT_CHECK(p_rmt_obj[channel] != NULL, RMT_DRIVER_ERROR_STR, ESP_FAIL);
    RMT_CHECK(p_rmt_obj[channel]->sample_to_rmt != NULL,RMT_TRANSLATOR_UNINIT_STR, ESP_FAIL);
#if CONFIG_SPIRAM_USE_MALLOC
    if( p_rmt_obj[channel]->intr_alloc_flags & ESP_INTR_FLAG_IRAM ) {
        if( !esp_ptr_internal(src) ) {
            ESP_LOGE(RMT_TAG, RMT_PSRAM_BUFFER_WARN_STR);
            return ESP_ERR_INVALID_ARG;
        }
    }
#endif
    size_t item_num = 0;
    size_t translated_size = 0;
    rmt_obj_t* p_rmt = p_rmt_obj[channel];
    const uint32_t item_block_len = RMT.conf_ch[channel].conf0.mem_size * RMT_MEM_ITEM_NUM;
    const uint32_t item_sub_len = item_block_len / 2;
    xSemaphoreTake(p_rmt->tx_sem, portMAX_DELAY);
    p_rmt->sample_to_rmt((void *)src, p_rmt->tx_buf, src_size, item_block_len, &translated_size, &item_num);
    p_rmt->sample_size_remain = src_size - translated_size;
    p_rmt->sample_cur = src + translated_size;
    rmt_fill_memory(channel, p_rmt->tx_buf, item_num, 0);
    if (item_num == item_block_len) {
        rmt_set_tx_thr_intr_en(channel, 1, item_sub_len);
        p_rmt->tx_data = p_rmt->tx_buf;
        p_rmt->tx_offset = 0;
        p_rmt->tx_sub_len = item_sub_len;
        p_rmt->translator = true;
    } else {
        RMTMEM.chan[channel].data32[item_num].val = 0;
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
    for(int i = 0; i < RMT_CHANNEL_MAX; i++) {
        channel_status->status[i]= RMT_CHANNEL_UNINIT;
        if( p_rmt_obj[i] != NULL ) {
            if( p_rmt_obj[i]->tx_sem != NULL ) {
                if( xSemaphoreTake(p_rmt_obj[i]->tx_sem, (TickType_t)0) == pdTRUE ) {
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
