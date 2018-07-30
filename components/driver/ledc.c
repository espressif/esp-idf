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
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "soc/gpio_sig_map.h"
#include "driver/ledc.h"
#include "soc/ledc_reg.h"
#include "soc/ledc_struct.h"
#include "esp_log.h"

static const char* LEDC_TAG = "ledc";
static portMUX_TYPE ledc_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define LEDC_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(LEDC_TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define LEDC_ARG_CHECK(a, param) LEDC_CHECK(a, param " argument is invalid", ESP_ERR_INVALID_ARG)

typedef struct {
    uint16_t speed_mode;
    uint16_t direction;
    uint32_t target_duty;
    int cycle_num;
    int scale;
    ledc_fade_mode_t mode;
    xSemaphoreHandle ledc_fade_sem;
    xSemaphoreHandle ledc_fade_mux;
#if CONFIG_SPIRAM_USE_MALLOC
    StaticQueue_t ledc_fade_sem_storage;
#endif
} ledc_fade_t;

static ledc_fade_t *s_ledc_fade_rec[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];
static ledc_isr_handle_t s_ledc_fade_isr_handle = NULL;

#define LEDC_VAL_NO_CHANGE        (-1)
#define LEDC_STEP_NUM_MAX         (1023)
#define LEDC_DUTY_DECIMAL_BIT_NUM (4)
#define LEDC_HPOINT_VAL_MAX       (LEDC_HPOINT_HSCH1_V)
#define LEDC_FADE_TOO_SLOW_STR    "LEDC FADE TOO SLOW"
#define LEDC_FADE_TOO_FAST_STR    "LEDC FADE TOO FAST"
static const char *LEDC_FADE_SERVICE_ERR_STR = "LEDC fade service not installed";
static const char *LEDC_FADE_INIT_ERROR_STR = "LEDC fade channel init error, not enough memory or service not installed";

static void ledc_ls_timer_update(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        LEDC.timer_group[speed_mode].timer[timer_sel].conf.low_speed_update = 1;
    }
}

static IRAM_ATTR void ledc_ls_channel_update(ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        LEDC.channel_group[speed_mode].channel[channel_num].conf0.low_speed_update = 1;
    }
}

static esp_err_t ledc_enable_intr_type(ledc_mode_t speed_mode, uint32_t channel, ledc_intr_type_t type)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    uint32_t value;
    uint32_t intr_type = type;
    portENTER_CRITICAL(&ledc_spinlock);
    value = LEDC.int_ena.val;
    uint8_t int_en_base = LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_S;
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        int_en_base = LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S;
    }
    if (intr_type == LEDC_INTR_FADE_END) {
        LEDC.int_ena.val = value | BIT(int_en_base + channel);
    } else {
        LEDC.int_ena.val = (value & (~(BIT(int_en_base + channel))));
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static void _ledc_fade_hw_acquire(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t* fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreTake(fade->ledc_fade_sem, portMAX_DELAY);
        ledc_enable_intr_type(mode, channel, LEDC_INTR_DISABLE);
    }
}

static void _ledc_fade_hw_release(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t* fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreGive(fade->ledc_fade_sem);
    }
}

static void _ledc_op_lock_acquire(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t* fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreTake(fade->ledc_fade_mux, portMAX_DELAY);
    }
}

static void _ledc_op_lock_release(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t* fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreGive(fade->ledc_fade_mux);
    }
}

static int ledc_get_max_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    // The arguments are checked before internally calling this function.
    int timer_sel = LEDC.channel_group[speed_mode].channel[channel].conf0.timer_sel;
    int max_duty = (1 << (LEDC.timer_group[speed_mode].timer[timer_sel].conf.duty_resolution));
    return max_duty;
}

esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider, uint32_t duty_resolution,
        ledc_clk_src_t clk_src)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.clock_divider = clock_divider;
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.tick_sel = clk_src;
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.duty_resolution = duty_resolution;
    ledc_ls_timer_update(speed_mode, timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t ledc_duty_config(ledc_mode_t speed_mode, ledc_channel_t channel_num, int hpoint_val, int duty_val,
    uint32_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    portENTER_CRITICAL(&ledc_spinlock);
    if (hpoint_val >= 0) {
        LEDC.channel_group[speed_mode].channel[channel_num].hpoint.hpoint = hpoint_val & LEDC_HPOINT_HSCH1_V;
    }
    if (duty_val >= 0) {
        LEDC.channel_group[speed_mode].channel[channel_num].duty.duty = duty_val;
    }
    LEDC.channel_group[speed_mode].channel[channel_num].conf1.val = ((duty_direction & LEDC_DUTY_INC_HSCH0_V) << LEDC_DUTY_INC_HSCH0_S) |
                                                                    ((duty_num & LEDC_DUTY_NUM_HSCH0_V) << LEDC_DUTY_NUM_HSCH0_S) |
                                                                    ((duty_cycle & LEDC_DUTY_CYCLE_HSCH0_V) << LEDC_DUTY_CYCLE_HSCH0_S) |
                                                                    ((duty_scale & LEDC_DUTY_SCALE_HSCH0_V) << LEDC_DUTY_SCALE_HSCH0_S);
    ledc_ls_channel_update(speed_mode, channel_num);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t timer_idx)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_idx < LEDC_TIMER_MAX, "timer_select");    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel].conf0.timer_sel = timer_idx;
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.rst = 1;
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.rst = 0;
    ledc_ls_timer_update(speed_mode, timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.pause = 1;
    ledc_ls_timer_update(speed_mode, timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.pause = 0;
    ledc_ls_timer_update(speed_mode, timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, ledc_isr_handle_t *handle)
{
    esp_err_t ret;
    LEDC_ARG_CHECK(fn, "fn");
    portENTER_CRITICAL(&ledc_spinlock);
    ret = esp_intr_alloc(ETS_LEDC_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

esp_err_t ledc_timer_config(const ledc_timer_config_t* timer_conf)
{
    LEDC_ARG_CHECK(timer_conf != NULL, "timer_conf");
    uint32_t freq_hz = timer_conf->freq_hz;
    uint32_t duty_resolution = timer_conf->duty_resolution;
    uint32_t timer_num = timer_conf->timer_num;
    uint32_t speed_mode = timer_conf->speed_mode;
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    periph_module_enable(PERIPH_LEDC_MODULE);
    if (freq_hz == 0 || duty_resolution == 0 || duty_resolution >= LEDC_TIMER_BIT_MAX) {
        ESP_LOGE(LEDC_TAG, "freq_hz=%u duty_resolution=%u", freq_hz, duty_resolution);
        return ESP_ERR_INVALID_ARG;
    }
    if (timer_num > LEDC_TIMER_3) {
        ESP_LOGE(LEDC_TAG, "invalid timer #%u", timer_num);
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    uint32_t precision = ( 0x1 << duty_resolution );  // 2**depth
    // Try calculating divisor based on LEDC_APB_CLK
    ledc_clk_src_t timer_clk_src = LEDC_APB_CLK;
    // div_param is a Q10.8 fixed point value
    uint64_t div_param = ( (uint64_t) LEDC_APB_CLK_HZ << 8 ) / freq_hz / precision;
    if (div_param < 256) {
        // divisor is too low
        ESP_LOGE(LEDC_TAG, "requested frequency and duty resolution can not be achieved, try reducing freq_hz or duty_resolution. div_param=%d",
            (uint32_t ) div_param);
        ret = ESP_FAIL;
    }
    if (div_param > LEDC_DIV_NUM_HSTIMER0_V) {
        // APB_CLK results in divisor which too high. Try using REF_TICK as clock source.
        timer_clk_src = LEDC_REF_TICK;
        div_param = ((uint64_t) LEDC_REF_CLK_HZ << 8) / freq_hz / precision;
        if (div_param < 256 || div_param > LEDC_DIV_NUM_HSTIMER0_V) {
            ESP_LOGE(LEDC_TAG, "requested frequency and duty resolution can not be achieved, try increasing freq_hz or duty_resolution. div_param=%d",
                (uint32_t ) div_param);
            ret = ESP_FAIL;
        }
    } else {
        if (speed_mode == LEDC_LOW_SPEED_MODE) {
            //for now, we only select 80mhz for slow clk of LEDC low speed channels.
            LEDC.conf.slow_clk_sel = 1;
        }
    }
    // set timer parameters
    ledc_timer_set(speed_mode, timer_num, div_param, duty_resolution, timer_clk_src);
    // reset timer
    ledc_timer_rst(speed_mode, timer_num);
    return ret;
}

esp_err_t ledc_set_pin(int gpio_num, ledc_mode_t speed_mode, ledc_channel_t ledc_channel)
{
    LEDC_ARG_CHECK(ledc_channel < LEDC_CHANNEL_MAX, "ledc_channel");
    LEDC_ARG_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "gpio_num");
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    if (speed_mode == LEDC_HIGH_SPEED_MODE) {
        gpio_matrix_out(gpio_num, LEDC_HS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    } else {
        gpio_matrix_out(gpio_num, LEDC_LS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    }
    return ESP_OK;
}

esp_err_t ledc_channel_config(const ledc_channel_config_t* ledc_conf)
{
    LEDC_ARG_CHECK(ledc_conf, "ledc_conf");
    uint32_t speed_mode = ledc_conf->speed_mode;
    uint32_t gpio_num = ledc_conf->gpio_num;
    uint32_t ledc_channel = ledc_conf->channel;
    uint32_t timer_select = ledc_conf->timer_sel;
    uint32_t intr_type = ledc_conf->intr_type;
    uint32_t duty = ledc_conf->duty;
    uint32_t hpoint = ledc_conf->hpoint;
    LEDC_ARG_CHECK(ledc_channel < LEDC_CHANNEL_MAX, "ledc_channel");
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "gpio_num");
    LEDC_ARG_CHECK(timer_select < LEDC_TIMER_MAX, "timer_select");
    periph_module_enable(PERIPH_LEDC_MODULE);
    esp_err_t ret = ESP_OK;
    /*set channel parameters*/
    /*   channel parameters decide how the waveform looks like in one period*/
    /*   set channel duty and hpoint value, duty range is (0 ~ ((2 ** duty_resolution) - 1)), max hpoint value is 0xfffff*/
    ledc_set_duty_with_hpoint(speed_mode, ledc_channel, duty, hpoint);
    /*update duty settings*/
    ledc_update_duty(speed_mode, ledc_channel);
    /*bind the channel with the timer*/
    ledc_bind_channel_timer(speed_mode, ledc_channel, timer_select);
    /*set interrupt type*/
    ledc_enable_intr_type(speed_mode, ledc_channel, intr_type);
    ESP_LOGD(LEDC_TAG, "LEDC_PWM CHANNEL %1u|GPIO %02u|Duty %04u|Time %01u",
        ledc_channel, gpio_num, duty, timer_select
    );
    /*set LEDC signal in gpio matrix*/
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    if (speed_mode == LEDC_HIGH_SPEED_MODE) {
        gpio_matrix_out(gpio_num, LEDC_HS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    } else {
        gpio_matrix_out(gpio_num, LEDC_LS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    }
    return ret;
}

esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 1;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 1;
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel].conf0.idle_lv = idle_level & 0x1;
    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 0;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 0;
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_set_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, ledc_duty_direction_t fade_direction,
    uint32_t step_num, uint32_t duty_cyle_num, uint32_t duty_scale)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_direction < LEDC_DUTY_DIR_MAX, "fade_direction");
    LEDC_ARG_CHECK(step_num <= LEDC_DUTY_NUM_HSCH0_V, "step_num");
    LEDC_ARG_CHECK(duty_cyle_num <= LEDC_DUTY_CYCLE_HSCH0_V, "duty_cycle_num");
    LEDC_ARG_CHECK(duty_scale <= LEDC_DUTY_SCALE_HSCH0_V, "duty_scale");
    _ledc_fade_hw_acquire(speed_mode, channel);
    ledc_duty_config(speed_mode,
                     channel,        //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty << 4,      //uint32_t duty_val,the least 4 bits are decimal part
                     fade_direction, //uint32_t increase,
                     step_num,       //uint32_t duty_num,
                     duty_cyle_num,  //uint32_t duty_cycle,
                     duty_scale      //uint32_t duty_scale
                     );
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_duty_with_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(hpoint <= LEDC_HPOINT_VAL_MAX, "hpoint");
    /* The channel configuration should not be changed before the fade operation is done. */
    _ledc_fade_hw_acquire(speed_mode, channel);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     hpoint,          //uint32_t hpoint_val,
                     duty << 4,       //uint32_t duty_val,the least 4 bits are decimal part
                     1,               //uint32_t increase,
                     1,               //uint32_t duty_num,
                     1,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                     );
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    /* The channel configuration should not be changed before the fade operation is done. */
    _ledc_fade_hw_acquire(speed_mode, channel);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty << 4,       //uint32_t duty_val,the least 4 bits are decimal part
                     1,               //uint32_t increase,
                     1,               //uint32_t duty_num,
                     1,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                     );
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    uint32_t duty = (LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> 4);
    return duty;
}

int ledc_get_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode argument is invalid", LEDC_ERR_VAL);
    LEDC_CHECK(channel < LEDC_CHANNEL_MAX, "channel argument is invalid", LEDC_ERR_VAL);
    uint32_t hpoint = LEDC.channel_group[speed_mode].channel[channel].hpoint.hpoint;
    return hpoint;
}

esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    portENTER_CRITICAL(&ledc_spinlock);
    esp_err_t ret = ESP_OK;
    uint32_t clock_divider = 0;
    uint32_t duty_resolution = LEDC.timer_group[speed_mode].timer[timer_num].conf.duty_resolution;
    uint32_t timer_source_clk = LEDC.timer_group[speed_mode].timer[timer_num].conf.tick_sel;
    uint32_t precision = (0x1 << duty_resolution);
    if (timer_source_clk == LEDC_APB_CLK) {
        clock_divider = ((uint64_t) LEDC_APB_CLK_HZ << 8) / freq_hz / precision;
    } else {
        clock_divider = ((uint64_t) LEDC_REF_CLK_HZ << 8) / freq_hz / precision;
    }
    if (clock_divider <= 256 || clock_divider > LEDC_DIV_NUM_HSTIMER0) {
        ESP_LOGE(LEDC_TAG, "div param err,div_param=%u", clock_divider);
        ret = ESP_FAIL;
    }
    LEDC.timer_group[speed_mode].timer[timer_num].conf.clock_divider = clock_divider;
    ledc_ls_timer_update(speed_mode, timer_num);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t freq = 0;
    uint32_t timer_source_clk = LEDC.timer_group[speed_mode].timer[timer_num].conf.tick_sel;
    uint32_t duty_resolution = LEDC.timer_group[speed_mode].timer[timer_num].conf.duty_resolution;
    uint32_t clock_divider = LEDC.timer_group[speed_mode].timer[timer_num].conf.clock_divider;
    uint32_t precision = (0x1 << duty_resolution);
    if (timer_source_clk == LEDC_APB_CLK) {
        freq = ((uint64_t) LEDC_APB_CLK_HZ << 8) / precision / clock_divider;
    } else {
        freq = ((uint64_t) LEDC_REF_CLK_HZ << 8) / precision / clock_divider;
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return freq;
}

void IRAM_ATTR ledc_fade_isr(void* arg)
{
    int channel;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    uint32_t intr_status = LEDC.int_st.val;  //read LEDC interrupt status.
    LEDC.int_clr.val = intr_status;  //clear LEDC interrupt status.
    int speed_mode = LEDC_HIGH_SPEED_MODE;
    for (channel = 0; channel < LEDC_CHANNEL_MAX; channel++) {
        if (intr_status & (BIT(LEDC_DUTY_CHNG_END_HSCH0_INT_ST_S + channel) | BIT(LEDC_DUTY_CHNG_END_LSCH0_INT_ST_S + channel))) {
            if (intr_status & BIT(LEDC_DUTY_CHNG_END_HSCH0_INT_ST_S + channel)) {
                speed_mode = LEDC_HIGH_SPEED_MODE;
            } else {
                speed_mode = LEDC_LOW_SPEED_MODE;
            }
            if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
                //fade object not initialized yet.
                continue;
            }
            uint32_t duty_cur = LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> LEDC_DUTY_DECIMAL_BIT_NUM;
            if (duty_cur == s_ledc_fade_rec[speed_mode][channel]->target_duty) {
                xSemaphoreGiveFromISR(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem, &HPTaskAwoken);
                if (HPTaskAwoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
                continue;
            }
            uint32_t duty_tar = s_ledc_fade_rec[speed_mode][channel]->target_duty;
            int scale = s_ledc_fade_rec[speed_mode][channel]->scale;
            if (scale == 0) {
                xSemaphoreGiveFromISR(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem, &HPTaskAwoken);
                continue;
            }
            int cycle = s_ledc_fade_rec[speed_mode][channel]->cycle_num;
            int delta = s_ledc_fade_rec[speed_mode][channel]->direction == LEDC_DUTY_DIR_DECREASE ? duty_cur - duty_tar : duty_tar - duty_cur;
            int step = delta / scale > LEDC_STEP_NUM_MAX ? LEDC_STEP_NUM_MAX : delta / scale;
            if (delta > scale) {
                ledc_duty_config(
                    speed_mode,
                    channel,
                    LEDC_VAL_NO_CHANGE,
                    duty_cur << LEDC_DUTY_DECIMAL_BIT_NUM,
                    s_ledc_fade_rec[speed_mode][channel]->direction,
                    step,
                    cycle,
                    scale);
            } else {
                ledc_duty_config(
                    speed_mode,
                    channel,
                    LEDC_VAL_NO_CHANGE,
                    duty_tar << LEDC_DUTY_DECIMAL_BIT_NUM,
                    s_ledc_fade_rec[speed_mode][channel]->direction,
                    1,
                    1,
                    0);
            }
            LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 1;
        }
    }
    LEDC.int_clr.val = intr_status;  //clear LEDC interrupt status.
}

static esp_err_t ledc_fade_channel_deinit(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (s_ledc_fade_rec[speed_mode][channel]) {
        if (s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux) {
            vSemaphoreDelete(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux);
            s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux = NULL;
        }
        if (s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem) {
            vSemaphoreDelete(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem);
            s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = NULL;
        }
        free(s_ledc_fade_rec[speed_mode][channel]);
        s_ledc_fade_rec[speed_mode][channel] = NULL;
    }
    return ESP_OK;
}

static esp_err_t ledc_fade_channel_init_check(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (s_ledc_fade_isr_handle == NULL) {
        ESP_LOGE(LEDC_TAG, "Fade service not installed, call ledc_fade_func_install");
        return ESP_FAIL;
    }
    if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
#if CONFIG_SPIRAM_USE_MALLOC
        s_ledc_fade_rec[speed_mode][channel] = (ledc_fade_t *) heap_caps_calloc(1, sizeof(ledc_fade_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
        if (!s_ledc_fade_rec[speed_mode][channel]) {
            ledc_fade_channel_deinit(speed_mode, channel);
            return ESP_FAIL;
        }

        memset(&s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem_storage, 0, sizeof(StaticQueue_t));
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = xSemaphoreCreateBinaryStatic(&s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem_storage);
#else
        s_ledc_fade_rec[speed_mode][channel] = (ledc_fade_t *) calloc(1, sizeof(ledc_fade_t));
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = xSemaphoreCreateBinary();
#endif
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux = xSemaphoreCreateMutex();
        xSemaphoreGive(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem);
    }
    if (s_ledc_fade_rec[speed_mode][channel]
        && s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux
        && s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem) {
        return ESP_OK;
    } else {
        ledc_fade_channel_deinit(speed_mode, channel);
        return ESP_FAIL;
    }
}

static esp_err_t _ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int scale, int cycle_num)
{
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t duty_cur = LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> LEDC_DUTY_DECIMAL_BIT_NUM;
    // When duty == max_duty, meanwhile, if scale == 1 and fade_down == 1, counter would overflow.
    if (duty_cur == ledc_get_max_duty(speed_mode, channel)) {
        duty_cur -= 1;
    }
    s_ledc_fade_rec[speed_mode][channel]->speed_mode = speed_mode;
    s_ledc_fade_rec[speed_mode][channel]->target_duty = target_duty;
    s_ledc_fade_rec[speed_mode][channel]->cycle_num = cycle_num;
    s_ledc_fade_rec[speed_mode][channel]->scale = scale;
    int step_num = 0;
    int dir = LEDC_DUTY_DIR_DECREASE;
    if (scale > 0) {
        if (duty_cur > target_duty) {
            s_ledc_fade_rec[speed_mode][channel]->direction = LEDC_DUTY_DIR_DECREASE;
            step_num = (duty_cur - target_duty) / scale;
            step_num = step_num > LEDC_STEP_NUM_MAX ? LEDC_STEP_NUM_MAX : step_num;
        } else {
            s_ledc_fade_rec[speed_mode][channel]->direction = LEDC_DUTY_DIR_INCREASE;
            dir = LEDC_DUTY_DIR_INCREASE;
            step_num = (target_duty - duty_cur) / scale;
            step_num = step_num > LEDC_STEP_NUM_MAX ? LEDC_STEP_NUM_MAX : step_num;
        }
    }

    portEXIT_CRITICAL(&ledc_spinlock);
    if (scale > 0 && step_num > 0) {
        ledc_duty_config(speed_mode, channel, LEDC_VAL_NO_CHANGE, duty_cur << 4, dir, step_num, cycle_num, scale);
        ESP_LOGD(LEDC_TAG, "cur duty: %d; target: %d, step: %d, cycle: %d; scale: %d; dir: %d\n",
                duty_cur, target_duty, step_num, cycle_num, scale, dir);
    } else {
        ledc_duty_config(speed_mode, channel, LEDC_VAL_NO_CHANGE, target_duty << 4, dir, 0, 1, 0);
        ESP_LOGD(LEDC_TAG, "Set to target duty: %d", target_duty);
    }
    return ESP_OK;
}

static esp_err_t _ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    int timer_sel = LEDC.channel_group[speed_mode].channel[channel].conf0.timer_sel;
    uint32_t freq = ledc_get_freq(speed_mode, timer_sel);
    uint32_t duty_cur = LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> LEDC_DUTY_DECIMAL_BIT_NUM;
    uint32_t duty_delta = target_duty > duty_cur ? target_duty - duty_cur : duty_cur - target_duty;

    if (duty_delta == 0) {
        return _ledc_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }
    int total_cycles = max_fade_time_ms * freq / 1000;
    if (total_cycles == 0) {
        ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
        return _ledc_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }
    int scale, cycle_num;
    if (total_cycles > duty_delta) {
        scale = 1;
        cycle_num = total_cycles / duty_delta;
        if (cycle_num > LEDC_DUTY_NUM_HSCH0_V) {
            ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_SLOW_STR);
            cycle_num = LEDC_DUTY_NUM_HSCH0_V;
        }
    } else {
        cycle_num = 1;
        scale = duty_delta / total_cycles;
    }
    return _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
}

static void _ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode)
{
    s_ledc_fade_rec[speed_mode][channel]->mode = fade_mode;
    // Clear interrupt status of channel
    int duty_resolution_ch0 = (speed_mode == LEDC_HIGH_SPEED_MODE) ? LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_S : LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S;
    LEDC.int_clr.val |= BIT(duty_resolution_ch0 + channel);
    // Enable interrupt for channel
    ledc_enable_intr_type(speed_mode, channel, LEDC_INTR_FADE_END);
    ledc_update_duty(speed_mode, channel);
    if (fade_mode == LEDC_FADE_WAIT_DONE) {
        xSemaphoreTake(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem, portMAX_DELAY);
    }
}

esp_err_t ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK , LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_time(speed_mode, channel, target_duty, max_fade_time_ms);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK((scale > 0) && (scale <= LEDC_DUTY_SCALE_HSCH0_V), "fade scale");
    LEDC_ARG_CHECK((cycle_num > 0) && (cycle_num <= LEDC_DUTY_CYCLE_HSCH0_V), "cycle_num");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK , LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode)
{
    LEDC_CHECK(s_ledc_fade_rec != NULL, LEDC_FADE_SERVICE_ERR_STR, ESP_ERR_INVALID_STATE);
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_fade_func_install(int intr_alloc_flags)
{
    //OR intr_alloc_flags with ESP_INTR_FLAG_IRAM because the fade isr is in IRAM
    return ledc_isr_register(ledc_fade_isr, NULL, intr_alloc_flags | ESP_INTR_FLAG_IRAM, &s_ledc_fade_isr_handle);
}

void ledc_fade_func_uninstall()
{
    if (s_ledc_fade_rec == NULL) {
        return;
    }
    if (s_ledc_fade_isr_handle) {
        esp_intr_free(s_ledc_fade_isr_handle);
        s_ledc_fade_isr_handle = NULL;
    }
    int channel, mode;
    for (mode = 0; mode < LEDC_SPEED_MODE_MAX; mode++) {
        for (channel = 0; channel < LEDC_CHANNEL_MAX; channel++) {
            ledc_fade_channel_deinit(mode, channel);
        }
    }
    return;
}

/*
 * The functions below are thread-safe version of APIs for duty and fade control.
 * These APIs can be called from different tasks.
 */
esp_err_t ledc_set_duty_and_update(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK , LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    uint32_t cur_duty = ledc_get_duty(speed_mode, channel);
    if (duty == cur_duty) {
        return ESP_OK;
    }
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    int scale = cur_duty > duty ? cur_duty - duty : duty - cur_duty;
    _ledc_set_fade_with_step(speed_mode, channel, duty, scale, 1);
    _ledc_fade_start(speed_mode, channel, LEDC_FADE_WAIT_DONE);
    _ledc_fade_hw_release(speed_mode, channel);
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_time_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t max_fade_time_ms, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK , LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_time(speed_mode, channel, target_duty, max_fade_time_ms);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    if (fade_mode == LEDC_FADE_WAIT_DONE) {
        _ledc_fade_hw_release(speed_mode, channel);
    }
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_step_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK , LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    LEDC_ARG_CHECK((scale > 0) && (scale <= LEDC_DUTY_SCALE_HSCH0_V), "fade scale");
    LEDC_ARG_CHECK((cycle_num > 0) && (cycle_num <= LEDC_DUTY_CYCLE_HSCH0_V), "cycle_num");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    if (fade_mode == LEDC_FADE_WAIT_DONE) {
        _ledc_fade_hw_release(speed_mode, channel);
    }
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}
