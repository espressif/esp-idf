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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "soc/dport_reg.h"
#include "soc/gpio_sig_map.h"
#include "driver/ledc.h"

//TODO: Move these debug options to menuconfig
#define LEDC_DBG_WARING_ENABLE (0)
#define LEDC_DBG_ERROR_ENABLE  (0)
#define LEDC_INFO_ENABLE       (0)
#define LEDC_DBG_ENABLE        (0)

//DBG INFOR
#if LEDC_DBG_ENABLE
#define LEDC_DBG(format,...) do{\
        ets_printf("[dbg][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define LEDC_DBG(...)
#endif

#if LEDC_INFO_ENABLE
#define LEDC_INFO(format,...) do{\
        ets_printf("[info][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define LEDC_INFO(...)
#endif

#if LEDC_DBG_WARING_ENABLE
#define LEDC_WARING(format,...) do{\
        ets_printf("[waring][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define LEDC_WARING(...)
#endif
#if LEDC_DBG_ERROR_ENABLE
#define LEDC_ERROR(format,...) do{\
        ets_printf("[error][%s#%u]",__FUNCTION__,__LINE__);\
        ets_printf(format,##__VA_ARGS__);\
}while(0)
#else
#define LEDC_ERROR(...)
#endif

static portMUX_TYPE ledc_spinlock = portMUX_INITIALIZER_UNLOCKED;

static int ledc_is_valid_channel(uint32_t channel)
{
    if(channel > LEDC_CHANNEL_7) {
        LEDC_ERROR("LEDC CHANNEL ERR: %d\n",channel);
        return 0;
    }
    return 1;
}

static int ledc_is_valid_mode(uint32_t mode)
{
    if(mode >= LEDC_SPEED_MODE_MAX) {
        LEDC_ERROR("LEDC MODE ERR: %d\n",mode);
        return 0;
    }
    return 1;
}


static esp_err_t ledc_timer_config(ledc_mode_t speed_mode, uint32_t timer_sel, uint32_t div_num, uint32_t timer_lim,ledc_timer_source_t clk)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.high_speed_timer[timer_sel].conf.div_num = div_num;
    LEDC.high_speed_timer[timer_sel].conf.tick_sel = clk;
    LEDC.high_speed_timer[timer_sel].conf.timer_lim = timer_lim;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static esp_err_t ledc_duty_config(ledc_mode_t speed_mode, uint32_t channel_num, uint32_t hpoint_val, uint32_t duty_val,
    uint32_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.high_speed_channel[channel_num].hpoint.hpoint = hpoint_val;
    LEDC.high_speed_channel[channel_num].duty.duty = duty_val;
    LEDC.high_speed_channel[channel_num].conf1.duty_inc = duty_direction;
    LEDC.high_speed_channel[channel_num].conf1.duty_num = duty_num;
    LEDC.high_speed_channel[channel_num].conf1.duty_cycle = duty_cycle;
    LEDC.high_speed_channel[channel_num].conf1.duty_scale = duty_scale;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static esp_err_t ledc_set_channel_timer(ledc_mode_t speed_mode, uint32_t channel, uint32_t timer_idx)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.high_speed_channel[channel].conf0.timer_sel = timer_idx;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, uint32_t timer_sel)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.high_speed_timer[timer_sel].conf.rst = 1;
    LEDC.high_speed_timer[timer_sel].conf.rst = 0;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}
static esp_err_t ledc_enable_intr_type(ledc_mode_t speed_mode, uint32_t channel, ledc_intr_type_t type)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    uint32_t value;
    uint32_t intr_type = type;
    portENTER_CRITICAL(&ledc_spinlock);
    value = LEDC.int_ena.val;
    if(intr_type & LEDC_INTR_FADE_END) {
        LEDC.int_ena.val = value | BIT(8 + channel);
    } else {
        LEDC.int_ena.val = (value & (~(BIT(8 + channel))));
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_isr_register(uint32_t ledc_intr_num, void (*fn)(void*), void * arg)
{
    if(fn == NULL)
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    ESP_INTR_DISABLE(ledc_intr_num);
    intr_matrix_set(xPortGetCoreID(), ETS_LEDC_INTR_SOURCE, ledc_intr_num);
    xt_set_interrupt_handler(ledc_intr_num, fn, arg);
    ESP_INTR_ENABLE(ledc_intr_num);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_config(ledc_config_t* ledc_conf)
{
    SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_LEDC_CLK_EN);
    CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_LEDC_RST);

    uint32_t speed_mode = ledc_conf->speed_mode;
    uint32_t gpio_num = ledc_conf->gpio_num;
    uint32_t ledc_channel = ledc_conf->channel;
    uint32_t freq_hz = ledc_conf->freq_hz;
    uint32_t timer_select = ledc_conf->timer_src;
    uint32_t duty_depth = ledc_conf->duty_depth;
    uint32_t intr_type = ledc_conf->intr_type;
    uint32_t duty = ledc_conf->duty;
    uint64_t div_param = 0;
    uint32_t precision = 0;
    if(!ledc_is_valid_channel(ledc_channel))
        return ESP_ERR_INVALID_ARG;
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    if(gpio_num >= GPIO_NUM_34 || gpio_num == 20 || gpio_num == 24 || gpio_num == 28 || gpio_num == 29 || gpio_num == 30
        || gpio_num == 31) {
        LEDC_ERROR("GPIO number error: IO%d\n ", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }
    if(gpio_num >= GPIO_PIN_COUNT || 0 == GPIO_PIN_MUX_REG[gpio_num]) {
        LEDC_ERROR("io_num=%d does not exist\n", gpio_num);
        return ESP_ERR_INVALID_ARG;
    }
    if(freq_hz == 0 || duty_depth == 0 || duty_depth > LEDC_DUTY_DEPTH_15_BIT) {
        LEDC_ERROR("freq_hz=%u duty_depth=%u\n", div_param, duty_depth);
        return ESP_ERR_INVALID_ARG;
    }
    if(timer_select > LEDC_TIMER3) {
        LEDC_ERROR("Time Select %u\n", timer_select);
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&ledc_spinlock);
    esp_err_t ret = ESP_OK;
    /*gpio matrix ledc pwm signal*/
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], 2);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    gpio_matrix_out(gpio_num, LEDC_HS_SIG_OUT0_IDX + ledc_channel, 0, 0);
    /*configure ledc param*/
    /*calculate the div_param and select which base clock and first we will select the apb_clk */
    precision = (0x1 << duty_depth);  //2**depth
    div_param = (uint64_t) LEDC_APB_CLK_HZ * 256 / freq_hz / precision; //8bit fragment
    /*Fail ,because the div num overflow or too small*/
    if(div_param <= 256 || div_param > LEDC_DIV_NUM_HSTIMER0) { //REF TICK
        /*Selet the reference tick*/
        div_param = (uint64_t) LEDC_REF_CLK_HZ * 256 / freq_hz / precision;
        if(div_param <= 256 || div_param > LEDC_DIV_NUM_HSTIMER0_V) {
            LEDC_ERROR("div param err,div_param=%u\n", div_param);
            ret = ESP_FAIL;
        }
        ledc_timer_config(speed_mode, timer_select, div_param, duty_depth, LEDC_REF_TICK);
        ledc_set_channel_timer(speed_mode, ledc_channel, timer_select);
    } else { //APB TICK
        ledc_timer_config(speed_mode, timer_select, div_param, duty_depth, LEDC_APB_CLK);
        ledc_set_channel_timer(speed_mode, ledc_channel, timer_select);
    }
    ledc_timer_rst(speed_mode, timer_select);
    ledc_set_duty(speed_mode, ledc_channel, duty);
    ledc_enable_intr_type(speed_mode, ledc_channel, intr_type);
    LEDC_INFO("LEDC_PWM CHANNEL %1u|GPIO %02u|FreHz %05u|Duty %04u|Depth %04u|Time %01u|SourceClk %01u|Divparam %u\n",
        ledc_channel, gpio_num, freq_hz, duty, duty_depth, timer_select, timer_source_clk_flag, div_param
    );
    ledc_update(speed_mode, ledc_channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

esp_err_t ledc_update(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    if(!ledc_is_valid_channel(channel))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.high_speed_channel[channel].conf0.sig_out_en = 1;
    LEDC.high_speed_channel[channel].conf1.duty_start = 1;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    if(!ledc_is_valid_channel(channel))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    LEDC.high_speed_channel[channel].conf0.idle_lv = idle_level;
    LEDC.high_speed_channel[channel].conf0.sig_out_en = 0;
    LEDC.high_speed_channel[channel].conf1.duty_start = 0;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}
esp_err_t ledc_set_fade(ledc_mode_t speed_mode, uint32_t channel, uint32_t duty, ledc_duty_direction_t fade_direction,
    uint32_t step_num, uint32_t duty_cyle_num, uint32_t duty_scale)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    if(!ledc_is_valid_channel(channel))
        return ESP_ERR_INVALID_ARG;
    if(fade_direction > LEDC_DUTY_DIR_INCREASE) {
        LEDC_ERROR("Duty direction err\n");
        return ESP_ERR_INVALID_ARG;
    }
    if(step_num > 0X3FF || duty_cyle_num > 0X3FF || duty_scale > 0X3FF) {
        LEDC_ERROR("step_num=%u duty_cyle_num=%u duty_scale=%u\n", step_num, duty_cyle_num, duty_scale);
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,        //uint32_t chan_num,
                     0,              //uint32_t hpoint_val,
                     duty << 4,      //uint32_t duty_val,the least 4 bits are decimal part
                     fade_direction, //uint32_t increase,
                     step_num,       //uint32_t duty_num,
                     duty_cyle_num,  //uint32_t duty_cycle,
                     duty_scale      //uint32_t duty_scale
                     );
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    if(!ledc_is_valid_channel(channel))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     0,               //uint32_t hpoint_val,
                     duty << 4,       //uint32_t duty_val,the least 4 bits are decimal part
                     1,               //uint32_t increase,
                     1,               //uint32_t duty_num,
                     1,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                     );
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    uint32_t duty = 0;
    duty = (LEDC.high_speed_channel[channel].duty_rd.duty_read >> 4);
    return duty;
}

esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t freq_hz)
{
    if(!ledc_is_valid_mode(speed_mode))
        return ESP_ERR_INVALID_ARG;
    if(!ledc_is_valid_channel(channel))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    esp_err_t ret = ESP_OK;
    uint32_t div_num = 0;
    /*Select timer*/
    uint32_t timer_select = LEDC.high_speed_channel[channel].conf0.timer_sel;
    /*Get timer limit*/
    uint32_t duty_depth = LEDC.high_speed_timer[timer_select].conf.timer_lim;
    uint32_t timer_source_clk = LEDC.high_speed_timer[timer_select].conf.tick_sel;
    uint32_t precision = (0x1 << duty_depth);
    if(timer_source_clk == LEDC_APB_CLK) {
        div_num = (uint64_t) LEDC_APB_CLK_HZ * 256 / freq_hz / precision;
    } else {
        div_num = (uint64_t) LEDC_REF_CLK_HZ * 256 / freq_hz / precision;
    }
    if(div_num <= 256 || div_num > LEDC_DIV_NUM_HSTIMER0) {
        LEDC_ERROR("channel %u,div param err,div_param=%u\n", channel, div_num);
        ret = ESP_FAIL;
    }
    LEDC.high_speed_timer[timer_select].conf.div_num = div_num;
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if(!ledc_is_valid_mode(speed_mode))
        return 0;
    if(!ledc_is_valid_channel(channel))
        return ESP_ERR_INVALID_ARG;
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t freq = 0;
    uint32_t timer_select = LEDC.high_speed_channel[channel].conf0.timer_sel;
    uint32_t timer_source_clk = LEDC.high_speed_timer[timer_select].conf.tick_sel;
    uint32_t duty_depth = LEDC.high_speed_timer[timer_select].conf.timer_lim;
    uint32_t div_num = LEDC.high_speed_timer[timer_select].conf.div_num;
    uint32_t precision = (0x1 << duty_depth);
    if(timer_source_clk == LEDC_APB_CLK) {
        freq = ((uint64_t) LEDC_APB_CLK_HZ) * 256 / precision / div_num;
    } else {
        freq = ((uint64_t) LEDC_REF_CLK_HZ) * 256 / precision / div_num;
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    return freq;
}
