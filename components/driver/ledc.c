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
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "soc/gpio_sig_map.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char* LEDC_TAG = "ledc";
static portMUX_TYPE ledc_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define LEDC_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(LEDC_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t div_num, uint32_t bit_num, ledc_clk_src_t clk_src)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(timer_sel <= LEDC_TIMER_3, "ledc timer error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.div_num = div_num;
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.tick_sel = clk_src;
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.bit_num = bit_num;
    if(speed_mode != LEDC_HIGH_SPEED_MODE) {
        LEDC.timer_group[speed_mode].timer[timer_sel].conf.low_speed_update = 1;
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static esp_err_t ledc_duty_config(ledc_mode_t speed_mode, uint32_t channel_num, uint32_t hpoint_val, uint32_t duty_val,
    uint32_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel_num].hpoint.hpoint = hpoint_val;
    LEDC.channel_group[speed_mode].channel[channel_num].duty.duty = duty_val;
    LEDC.channel_group[speed_mode].channel[channel_num].conf1.val = ((duty_direction & LEDC_DUTY_INC_HSCH0_V) << LEDC_DUTY_INC_HSCH0_S) |
                                                                    ((duty_num & LEDC_DUTY_NUM_HSCH0_V) << LEDC_DUTY_NUM_HSCH0_S) |
                                                                    ((duty_cycle & LEDC_DUTY_CYCLE_HSCH0_V) << LEDC_DUTY_CYCLE_HSCH0_S) |
                                                                    ((duty_scale & LEDC_DUTY_SCALE_HSCH0_V) << LEDC_DUTY_SCALE_HSCH0_S);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, uint32_t channel, uint32_t timer_idx)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(timer_idx <= LEDC_TIMER_3, "ledc timer error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel].conf0.timer_sel = timer_idx;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(timer_sel <= LEDC_TIMER_3, "ledc timer error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.rst = 1;
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.rst = 0;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(timer_sel <= LEDC_TIMER_3, "ledc timer error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.pause = 1;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(timer_sel <= LEDC_TIMER_3, "ledc timer error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.timer_group[speed_mode].timer[timer_sel].conf.pause = 0;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static esp_err_t ledc_enable_intr_type(ledc_mode_t speed_mode, uint32_t channel, ledc_intr_type_t type)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    uint32_t value;
    uint32_t intr_type = type;
    portENTER_CRITICAL(&ledc_spinlock);
    value = LEDC.int_ena.val;
    if(intr_type == LEDC_INTR_FADE_END) {
        LEDC.int_ena.val = value | BIT(LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_S + channel);
    } else {
        LEDC.int_ena.val = (value & (~(BIT(LEDC_DUTY_CHNG_END_HSCH0_INT_ENA_S + channel))));
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_isr_register(void (*fn)(void*), void * arg, int intr_alloc_flags, ledc_isr_handle_t *handle)
{
    esp_err_t ret;
    LEDC_CHECK(fn, "ledc isr null", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    ret=esp_intr_alloc(ETS_LEDC_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

esp_err_t ledc_timer_config(ledc_timer_config_t* timer_conf)
{
    int freq_hz = timer_conf->freq_hz;
    int bit_num = timer_conf->bit_num;
    int timer_num = timer_conf->timer_num;
    int speed_mode = timer_conf->speed_mode;
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    if(freq_hz == 0 || bit_num == 0 || bit_num > LEDC_TIMER_15_BIT) {
        ESP_LOGE(LEDC_TAG, "freq_hz=%u bit_num=%u", freq_hz, bit_num);
        return ESP_ERR_INVALID_ARG;
    }
    if(timer_num > LEDC_TIMER_3) {
        ESP_LOGE(LEDC_TAG, "invalid timer #%u", timer_num);
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    uint32_t precision = (0x1 << bit_num);  // 2**depth
    // Try calculating divisor based on LEDC_APB_CLK
    ledc_clk_src_t timer_clk_src = LEDC_APB_CLK;
    // div_param is a Q10.8 fixed point value
    uint64_t div_param = ((uint64_t) LEDC_APB_CLK_HZ << 8) / freq_hz / precision;
    if (div_param < 256) {
        // divisor is too low
        ESP_LOGE(LEDC_TAG, "requested frequency and bit depth can not be achieved, try reducing freq_hz or bit_num. div_param=%d", (uint32_t) div_param);
        ret = ESP_FAIL;
    }
    if (div_param > LEDC_DIV_NUM_HSTIMER0_V) {
        // APB_CLK results in divisor which too high. Try using REF_TICK as clock source.
        timer_clk_src = LEDC_REF_TICK;
        div_param = ((uint64_t) LEDC_REF_CLK_HZ << 8) / freq_hz / precision;
        if(div_param < 256 || div_param > LEDC_DIV_NUM_HSTIMER0_V) {
            ESP_LOGE(LEDC_TAG, "requested frequency and bit depth can not be achieved, try increasing freq_hz or bit_num. div_param=%d", (uint32_t) div_param);
            ret = ESP_FAIL;
        }
    }
    // set timer parameters
    ledc_timer_set(speed_mode, timer_num, div_param, bit_num, timer_clk_src);
    // reset timer
    ledc_timer_rst(speed_mode, timer_num);
    return ret;
}

esp_err_t ledc_set_pin(int gpio_num, ledc_mode_t speed_mode, ledc_channel_t ledc_channel)
{
    LEDC_CHECK(ledc_channel <= LEDC_CHANNEL_7, "ledc channel error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "ledc GPIO output number error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    if(speed_mode == LEDC_HIGH_SPEED_MODE) {
        gpio_matrix_out(gpio_num, LEDC_HS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    } else {
        ESP_LOGE(LEDC_TAG, "low speed mode is not implemented");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}

esp_err_t ledc_channel_config(ledc_channel_config_t* ledc_conf)
{
    uint32_t speed_mode = ledc_conf->speed_mode;
    uint32_t gpio_num = ledc_conf->gpio_num;
    uint32_t ledc_channel = ledc_conf->channel;
    uint32_t timer_select = ledc_conf->timer_sel;
    uint32_t intr_type = ledc_conf->intr_type;
    uint32_t duty = ledc_conf->duty;
    LEDC_CHECK(ledc_channel <= LEDC_CHANNEL_7, "ledc channel error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "ledc GPIO output number error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(timer_select <= LEDC_TIMER_3, "ledc timer error", ESP_ERR_INVALID_ARG);
    periph_module_enable(PERIPH_LEDC_MODULE);
    esp_err_t ret = ESP_OK;
    /*set channel parameters*/
    /*   channel parameters decide how the waveform looks like in one period*/
    /*   set channel duty, duty range is (0 ~ ((2 ** bit_num) - 1))*/
    ledc_set_duty(speed_mode, ledc_channel, duty);
    /*update duty settings*/
    ledc_update_duty(speed_mode, ledc_channel);
    /*bind the channel with the timer*/
    ledc_bind_channel_timer(speed_mode, ledc_channel, timer_select);
    /*set interrupt type*/
    ledc_enable_intr_type(speed_mode, ledc_channel, intr_type);
    ESP_LOGI(LEDC_TAG, "LEDC_PWM CHANNEL %1u|GPIO %02u|Duty %04u|Time %01u",
        ledc_channel, gpio_num, duty, timer_select
    );
    /*set LEDC signal in gpio matrix*/
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    gpio_matrix_out(gpio_num, LEDC_HS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    return ret;
}

esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(channel <= LEDC_CHANNEL_7, "ledc channel error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 1;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 1;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(channel <= LEDC_CHANNEL_7, "ledc channel error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.channel_group[speed_mode].channel[channel].conf0.idle_lv = idle_level & 0x1;
    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 0;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 0;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}
esp_err_t ledc_set_fade(ledc_mode_t speed_mode, uint32_t channel, uint32_t duty, ledc_duty_direction_t fade_direction,
    uint32_t step_num, uint32_t duty_cyle_num, uint32_t duty_scale)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(channel <= LEDC_CHANNEL_7, "ledc channel error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(fade_direction <= LEDC_DUTY_DIR_INCREASE, "ledc fade direction error", ESP_ERR_INVALID_ARG);
    if(step_num > LEDC_DUTY_NUM_HSCH0_V || duty_cyle_num > LEDC_DUTY_CYCLE_HSCH0_V || duty_scale > LEDC_DUTY_SCALE_HSCH0_V) {
        ESP_LOGE(LEDC_TAG, "step_num=%u duty_cyle_num=%u duty_scale=%u", step_num, duty_cyle_num, duty_scale);
        return ESP_ERR_INVALID_ARG;
    }
    ledc_duty_config(speed_mode,
                     channel,        //uint32_t chan_num,
                     0,              //uint32_t hpoint_val,
                     duty << 4,      //uint32_t duty_val,the least 4 bits are decimal part
                     fade_direction, //uint32_t increase,
                     step_num,       //uint32_t duty_num,
                     duty_cyle_num,  //uint32_t duty_cycle,
                     duty_scale      //uint32_t duty_scale
                     );
    return ESP_OK;
}

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    LEDC_CHECK(channel <= LEDC_CHANNEL_7, "ledc channel error", ESP_ERR_INVALID_ARG);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     0,               //uint32_t hpoint_val,
                     duty << 4,       //uint32_t duty_val,the least 4 bits are decimal part
                     1,               //uint32_t increase,
                     1,               //uint32_t duty_num,
                     1,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                     );
    return ESP_OK;
}

int ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", (-1));
    uint32_t duty = (LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> 4);
    return duty;
}

esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&ledc_spinlock);
    esp_err_t ret = ESP_OK;
    uint32_t div_num = 0;
    uint32_t bit_num = LEDC.timer_group[speed_mode].timer[timer_num].conf.bit_num;
    uint32_t timer_source_clk = LEDC.timer_group[speed_mode].timer[timer_num].conf.tick_sel;
    uint32_t precision = (0x1 << bit_num);
    if(timer_source_clk == LEDC_APB_CLK) {
        div_num = ((uint64_t) LEDC_APB_CLK_HZ << 8) / freq_hz / precision;
    } else {
        div_num = ((uint64_t) LEDC_REF_CLK_HZ << 8) / freq_hz / precision;
    }
    if(div_num <= 256 || div_num > LEDC_DIV_NUM_HSTIMER0) {
        ESP_LOGE(LEDC_TAG, "div param err,div_param=%u", div_num);
        ret = ESP_FAIL;
    }
    LEDC.timer_group[speed_mode].timer[timer_num].conf.div_num = div_num;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "ledc mode error", (0));
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t freq = 0;
    uint32_t timer_source_clk = LEDC.timer_group[speed_mode].timer[timer_num].conf.tick_sel;
    uint32_t bit_num = LEDC.timer_group[speed_mode].timer[timer_num].conf.bit_num;
    uint32_t div_num = LEDC.timer_group[speed_mode].timer[timer_num].conf.div_num;
    uint32_t precision = (0x1 << bit_num);
    if(timer_source_clk == LEDC_APB_CLK) {
        freq = ((uint64_t) LEDC_APB_CLK_HZ << 8) / precision / div_num;
    } else {
        freq = ((uint64_t) LEDC_REF_CLK_HZ << 8) / precision / div_num;
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return freq;
}
