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

#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "soc/mcpwm_periph.h"
#include "soc/gpio_periph.h"
#include "driver/mcpwm.h"
#include "driver/periph_ctrl.h"
#include "sdkconfig.h"


static mcpwm_dev_t *MCPWM[2] = {&MCPWM0, &MCPWM1};
static const char *MCPWM_TAG = "MCPWM";
static portMUX_TYPE mcpwm_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define MCPWM_CHECK(a, str, ret_val) if (!(a)) {                       \
    ESP_LOGE(MCPWM_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);    \
    return (ret_val);                                  \
    }

#define MCPWM_UNIT_NUM_ERROR    "MCPWM UNIT NUM ERROR"
#define MCPWM_TIMER_ERROR       "MCPWM TIMER NUM ERROR"
#define MCPWM_PARAM_ADDR_ERROR  "MCPWM PARAM ADDR ERROR"
#define MCPWM_DUTY_TYPE_ERROR   "MCPWM DUTY TYPE ERROR"
#define MCPWM_GPIO_ERROR        "MCPWM GPIO NUM ERROR"
#define MCPWM_OP_ERROR          "MCPWM OPERATOR ERROR"
#define MCPWM_DB_ERROR          "MCPWM DEADTIME TYPE ERROR"

#define MCPWM_BASE_CLK (2 * APB_CLK_FREQ)   //2*APB_CLK_FREQ 160Mhz
#define MCPWM_CLK_PRESCL 15       //MCPWM clock prescale
#define TIMER_CLK_PRESCALE 9      //MCPWM timer prescales
#define MCPWM_CLK (MCPWM_BASE_CLK/(MCPWM_CLK_PRESCL +1))
#define MCPWM_PIN_IGNORE    (-1)
#define OFFSET_FOR_GPIO_IDX_1  6
#define OFFSET_FOR_GPIO_IDX_2 75

esp_err_t mcpwm_gpio_init(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num)
{
    if (gpio_num == MCPWM_PIN_IGNORE) {
        //IGNORE
        return ESP_OK;
    }

    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK((GPIO_IS_VALID_GPIO(gpio_num)), MCPWM_GPIO_ERROR, ESP_ERR_INVALID_ARG);

    periph_module_enable(PERIPH_PWM0_MODULE + mcpwm_num);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    bool mcpwm_gpio_sig = (io_signal <= MCPWM2B);
    if (mcpwm_num == MCPWM_UNIT_0) {
        if (mcpwm_gpio_sig) {
            MCPWM_CHECK((GPIO_IS_VALID_OUTPUT_GPIO(gpio_num)), MCPWM_GPIO_ERROR, ESP_ERR_INVALID_ARG);
            gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(gpio_num, PWM0_OUT0A_IDX + io_signal, 0, 0);
        } else {
            gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
            gpio_matrix_in(gpio_num, PWM0_SYNC0_IN_IDX + io_signal - OFFSET_FOR_GPIO_IDX_1, 0);
        }
    } else { //MCPWM_UNIT_1
        if (mcpwm_gpio_sig) {
            MCPWM_CHECK((GPIO_IS_VALID_OUTPUT_GPIO(gpio_num)), MCPWM_GPIO_ERROR, ESP_ERR_INVALID_ARG);
            gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
            gpio_matrix_out(gpio_num, PWM1_OUT0A_IDX + io_signal, 0, 0);
        } else if (io_signal >= MCPWM_SYNC_0 && io_signal < MCPWM_FAULT_2) {
            gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
            gpio_matrix_in(gpio_num, PWM1_SYNC0_IN_IDX + io_signal - OFFSET_FOR_GPIO_IDX_1, 0);
        } else {
            gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
            gpio_matrix_in(gpio_num, PWM1_SYNC0_IN_IDX + io_signal - OFFSET_FOR_GPIO_IDX_2, 0);
        }
    }
    return ESP_OK;
}

esp_err_t mcpwm_set_pin(mcpwm_unit_t mcpwm_num, const mcpwm_pin_config_t *mcpwm_pin)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    mcpwm_gpio_init(mcpwm_num, MCPWM0A, mcpwm_pin->mcpwm0a_out_num);    //MCPWM0A
    mcpwm_gpio_init(mcpwm_num, MCPWM0B, mcpwm_pin->mcpwm0b_out_num);    //MCPWM0B
    mcpwm_gpio_init(mcpwm_num, MCPWM1A, mcpwm_pin->mcpwm1a_out_num);    //MCPWM1A
    mcpwm_gpio_init(mcpwm_num, MCPWM1B, mcpwm_pin->mcpwm1b_out_num);    //MCPWM1B
    mcpwm_gpio_init(mcpwm_num, MCPWM2A, mcpwm_pin->mcpwm2a_out_num);    //MCPWM2A
    mcpwm_gpio_init(mcpwm_num, MCPWM2B, mcpwm_pin->mcpwm2b_out_num);    //MCPWM2B
    mcpwm_gpio_init(mcpwm_num, MCPWM_SYNC_0,  mcpwm_pin->mcpwm_sync0_in_num);   //SYNC0
    mcpwm_gpio_init(mcpwm_num, MCPWM_SYNC_1,  mcpwm_pin->mcpwm_sync1_in_num);   //SYNC1
    mcpwm_gpio_init(mcpwm_num, MCPWM_SYNC_2,  mcpwm_pin->mcpwm_sync2_in_num);   //SYNC2
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_0, mcpwm_pin->mcpwm_fault0_in_num);  //FAULT0
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_0, mcpwm_pin->mcpwm_fault1_in_num);  //FAULT1
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_0, mcpwm_pin->mcpwm_fault2_in_num);  //FAULT2
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_0,   mcpwm_pin->mcpwm_cap0_in_num);    //CAP0
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_1,   mcpwm_pin->mcpwm_cap1_in_num);    //CAP1
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_2,   mcpwm_pin->mcpwm_cap2_in_num);    //CAP2
    return ESP_OK;
}


esp_err_t mcpwm_start(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->timer[timer_num].mode.start = 2;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->timer[timer_num].mode.start = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_set_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint32_t frequency)
{
    uint32_t mcpwm_num_of_pulse;
    uint32_t previous_period;
    uint32_t set_duty_a, set_duty_b;
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    mcpwm_num_of_pulse = MCPWM_CLK / (frequency * (TIMER_CLK_PRESCALE + 1));
    previous_period = MCPWM[mcpwm_num]->timer[timer_num].period.period;
    MCPWM[mcpwm_num]->timer[timer_num].period.prescale = TIMER_CLK_PRESCALE;
    MCPWM[mcpwm_num]->timer[timer_num].period.period = mcpwm_num_of_pulse;
    MCPWM[mcpwm_num]->timer[timer_num].period.upmethod = 0;
    set_duty_a = (((MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[0].cmpr_val) * mcpwm_num_of_pulse) / previous_period);
    set_duty_b = (((MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[1].cmpr_val) * mcpwm_num_of_pulse) / previous_period);
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[0].cmpr_val = set_duty_a;
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[1].cmpr_val = set_duty_b;
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_cfg.a_upmethod = 0;
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_cfg.b_upmethod = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t op_num, float duty)
{
    uint32_t set_duty;
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(op_num < MCPWM_OPR_MAX, MCPWM_OP_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    set_duty = (MCPWM[mcpwm_num]->timer[timer_num].period.period) * (duty) / 100;
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[op_num].cmpr_val = set_duty;
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_cfg.a_upmethod = BIT(0);
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_cfg.b_upmethod = BIT(0);
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}
esp_err_t mcpwm_set_duty_in_us(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t op_num, uint32_t duty)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(op_num < MCPWM_OPR_MAX, MCPWM_OP_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[op_num].cmpr_val = duty;
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_cfg.a_upmethod = BIT(0);
    MCPWM[mcpwm_num]->channel[timer_num].cmpr_cfg.b_upmethod = BIT(0);
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}
esp_err_t mcpwm_set_duty_type(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t op_num,
                              mcpwm_duty_type_t duty_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(op_num < MCPWM_OPR_MAX, MCPWM_OP_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(duty_num < MCPWM_DUTY_MODE_MAX, MCPWM_DUTY_TYPE_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    if (op_num == MCPWM_OPR_A) {
        if (MCPWM[mcpwm_num]->timer[timer_num].mode.mode == MCPWM_UP_COUNTER) {
            if (duty_num == MCPWM_DUTY_MODE_1) {
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utea = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 0;
            } else {   //MCPWM_DUTY_MODE_0
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utea = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 0;
            }
        } else if (MCPWM[mcpwm_num]->timer[timer_num].mode.mode == MCPWM_DOWN_COUNTER) {
            if (duty_num == MCPWM_DUTY_MODE_1) {
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtea = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 0;
            } else {   //MCPWM_DUTY_MODE_0
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtea = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 0;
            }
        } else {   //Timer count up-down
            if (duty_num == MCPWM_DUTY_MODE_1) {
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utea = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtea = 1;
            } else {   //MCPWM_DUTY_MODE_0
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utea = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtea = 2;
            }
        }
    }
    if (op_num == MCPWM_OPR_B) {
        if (MCPWM[mcpwm_num]->timer[timer_num].mode.mode == MCPWM_UP_COUNTER) {
            if (duty_num == MCPWM_DUTY_MODE_1) {
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].uteb = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 0;
            } else {   //MCPWM_DUTY_MODE_0
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].uteb = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 0;
            }
        } else if (MCPWM[mcpwm_num]->timer[timer_num].mode.mode == MCPWM_DOWN_COUNTER) {
            if (duty_num == MCPWM_DUTY_MODE_1) {
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dteb = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 0;
            } else {    //MCPWM_DUTY_MODE_0
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dteb = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 0;
            }
        } else {    //Timer count up-down
            if (duty_num == MCPWM_DUTY_MODE_1) {
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].uteb = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dteb = 1;
            } else {    //MCPWM_DUTY_MODE_0
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 2;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].uteb = 1;
                MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dteb = 2;
            }
        }
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_config_t *mcpwm_conf)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    periph_module_enable(PERIPH_PWM0_MODULE + mcpwm_num);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->clk_cfg.prescale = MCPWM_CLK_PRESCL;
    mcpwm_set_frequency(mcpwm_num, timer_num, mcpwm_conf->frequency);
    MCPWM[mcpwm_num]->timer[timer_num].mode.mode = mcpwm_conf ->counter_mode;
    mcpwm_set_duty(mcpwm_num, timer_num, 0, mcpwm_conf->cmpr_a);
    mcpwm_set_duty(mcpwm_num, timer_num, 1, mcpwm_conf->cmpr_b);
    mcpwm_set_duty_type(mcpwm_num, timer_num, 0, mcpwm_conf->duty_mode);
    mcpwm_set_duty_type(mcpwm_num, timer_num, 1, mcpwm_conf->duty_mode);
    mcpwm_start(mcpwm_num, timer_num);
    MCPWM[mcpwm_num]->timer_sel.operator0_sel = 0;
    MCPWM[mcpwm_num]->timer_sel.operator1_sel = 1;
    MCPWM[mcpwm_num]->timer_sel.operator2_sel = 2;
    MCPWM[mcpwm_num]->update_cfg.global_up_en = 1;
    MCPWM[mcpwm_num]->update_cfg.global_force_up = 1;
    MCPWM[mcpwm_num]->update_cfg.global_force_up = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

uint32_t mcpwm_get_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    uint32_t frequency;
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    frequency = MCPWM_CLK / ((MCPWM[mcpwm_num]->timer[timer_num].period.period) * (TIMER_CLK_PRESCALE + 1));
    return frequency;
}

float mcpwm_get_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t op_num)
{
    float duty;
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(op_num < MCPWM_OPR_MAX, MCPWM_OP_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    duty = 100.0 * (MCPWM[mcpwm_num]->channel[timer_num].cmpr_value[op_num].cmpr_val) / (MCPWM[mcpwm_num]->timer[timer_num].period.period);
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return duty;
}

esp_err_t mcpwm_set_signal_high(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t op_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(op_num < MCPWM_OPR_MAX, MCPWM_OP_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    if (op_num == MCPWM_OPR_A) {
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utea = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtea = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 2;
    } else {    //MCPWM_OPR_B
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].uteb = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dteb = 2;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 2;
    }

    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_set_signal_low(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t op_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(op_num < MCPWM_OPR_MAX, MCPWM_OP_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    if (op_num == 0) {
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utea = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtea = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 1;
    } if (op_num == 1) {
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utez = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].uteb = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].utep = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtez = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dteb = 1;
        MCPWM[mcpwm_num]->channel[timer_num].generator[op_num].dtep = 1;
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.en = 1;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.en = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_set_period(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_period)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.prescale = carrier_period;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}
esp_err_t mcpwm_carrier_set_duty_cycle(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_duty)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.duty = carrier_duty;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_oneshot_mode_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t pulse_width)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.oshtwth = pulse_width;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_oneshot_mode_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.oshtwth = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_output_invert(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num,
                                      mcpwm_carrier_out_ivt_t carrier_ivt_mode)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.out_invert = carrier_ivt_mode;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_carrier_config_t *carrier_conf)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    mcpwm_carrier_enable(mcpwm_num, timer_num);
    mcpwm_carrier_set_period(mcpwm_num, timer_num, carrier_conf->carrier_period);
    mcpwm_carrier_set_duty_cycle(mcpwm_num, timer_num, carrier_conf->carrier_duty);
    if (carrier_conf->carrier_os_mode == MCPWM_ONESHOT_MODE_EN) {
        mcpwm_carrier_oneshot_mode_enable(mcpwm_num, timer_num, carrier_conf->pulse_width_in_os);
    } else {
        mcpwm_carrier_oneshot_mode_disable(mcpwm_num, timer_num);
    }
    mcpwm_carrier_output_invert(mcpwm_num, timer_num, carrier_conf->carrier_ivt_mode);
    MCPWM[mcpwm_num]->channel[timer_num].carrier_cfg.in_invert = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_deadtime_type_t dt_mode,
                                uint32_t red, uint32_t fed)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(dt_mode < MCPWM_DEADTIME_TYPE_MAX, MCPWM_DB_ERROR, ESP_ERR_INVALID_ARG );
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_upmethod = BIT(0);
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_upmethod = BIT(0);
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.clk_sel = 0;
    MCPWM[mcpwm_num]->channel[timer_num].db_red_cfg.red = red;
    MCPWM[mcpwm_num]->channel[timer_num].db_fed_cfg.fed = fed;
    switch (dt_mode) {
    case MCPWM_BYPASS_RED:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 1;      //S1
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 0;    //S2
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 1;        //S5
        break;
    case MCPWM_BYPASS_FED:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 1;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 0;      //S1
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 0;    //S2
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 0;        //S5
        break;
    case MCPWM_ACTIVE_HIGH_MODE:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 0;      //S1
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 0;    //S2
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 1;        //S5
        break;
    case MCPWM_ACTIVE_LOW_MODE:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 0;      //S1
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 1;    //S2
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 1;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 1;        //S5
        break;
    case MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 0;      //S1
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 0;    //S2
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 1;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 1;        //S5
        break;
    case MCPWM_ACTIVE_LOW_COMPLIMENT_MODE:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 0;      //S1
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 1;    //S2
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 1;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 0;        //S5
        break;
    case MCPWM_ACTIVE_RED_FED_FROM_PWMXA:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 1;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outswap = 1;        //S6
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outswap = 0;        //S7
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.deb_mode = 1;         //S8
        break;
    case MCPWM_ACTIVE_RED_FED_FROM_PWMXB:
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 0;      //S0
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outswap = 1;        //S6
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outswap = 0;        //S7
        MCPWM[mcpwm_num]->channel[timer_num].db_cfg.deb_mode = 1;         //S8
        break;
    default :
        break;
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outbypass = 1;      //S0
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outbypass = 1;      //S1
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_outinvert = 0;    //S2
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_outinvert = 0;    //S3
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.red_insel = 0;        //S4
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.fed_insel = 0;        //S5
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.a_outswap = 0;        //S6
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.b_outswap = 0;        //S7
    MCPWM[mcpwm_num]->channel[timer_num].db_cfg.deb_mode = 0;         //S8
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_fault_init(mcpwm_unit_t mcpwm_num, mcpwm_fault_input_level_t intput_level, mcpwm_fault_signal_t fault_sig)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    switch (fault_sig) {
    case MCPWM_SELECT_F0:
        MCPWM[mcpwm_num]->fault_detect.f0_en = 1;
        MCPWM[mcpwm_num]->fault_detect.f0_pole = intput_level;
        break;
    case MCPWM_SELECT_F1:
        MCPWM[mcpwm_num]->fault_detect.f1_en = 1;
        MCPWM[mcpwm_num]->fault_detect.f1_pole = intput_level;
        break;
    case MCPWM_SELECT_F2:
        MCPWM[mcpwm_num]->fault_detect.f2_en = 1;
        MCPWM[mcpwm_num]->fault_detect.f2_pole = intput_level;

        break;
    default :
        break;
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_fault_deinit(mcpwm_unit_t mcpwm_num, mcpwm_fault_signal_t fault_sig)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    if (fault_sig == MCPWM_SELECT_F0) {
        MCPWM[mcpwm_num]->fault_detect.f0_en = 0;
    } else if (fault_sig == MCPWM_SELECT_F1) {
        MCPWM[mcpwm_num]->fault_detect.f1_en = 0;
    } else {   //MCPWM_SELECT_F2
        MCPWM[mcpwm_num]->fault_detect.f2_en = 0;
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_fault_set_cyc_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                   mcpwm_action_on_pwmxa_t action_on_pwmxa, mcpwm_action_on_pwmxb_t action_on_pwmxb)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->channel[timer_num].tz_cfg1.cbcpulse = BIT(0);
    if (fault_sig == MCPWM_SELECT_F0) {
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f0_cbc = 1;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f0_ost = 0;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_cbc_d = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_cbc_u = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_cbc_d = action_on_pwmxb;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_cbc_u = action_on_pwmxb;
    } else if (fault_sig == MCPWM_SELECT_F1) {
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f1_cbc = 1;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f1_ost = 0;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_cbc_d = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_cbc_u = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_cbc_d = action_on_pwmxb;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_cbc_u = action_on_pwmxb;
    } else {  //MCPWM_SELECT_F2
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f2_cbc = 1;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f2_ost = 0;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_cbc_d = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_cbc_u = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_cbc_d = action_on_pwmxb;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_cbc_u = action_on_pwmxb;
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_fault_set_oneshot_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                       mcpwm_action_on_pwmxa_t action_on_pwmxa, mcpwm_action_on_pwmxb_t action_on_pwmxb)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    if (fault_sig == MCPWM_SELECT_F0) {
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f0_ost = 1;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f0_cbc = 0;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_ost_d = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_ost_u = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_ost_d = action_on_pwmxb;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_ost_u = action_on_pwmxb;
    } else if (fault_sig == MCPWM_SELECT_F1) {
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f1_ost = 1;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f1_cbc = 0;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_ost_d = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_ost_u = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_ost_d = action_on_pwmxb;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_ost_u = action_on_pwmxb;
    } else {   //MCPWM_SELECT_F2
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f2_ost = 1;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.f2_cbc = 0;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_ost_d = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.a_ost_u = action_on_pwmxa;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_ost_d = action_on_pwmxb;
        MCPWM[mcpwm_num]->channel[timer_num].tz_cfg0.b_ost_u = action_on_pwmxb;
    }
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_capture_enable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge,
                               uint32_t num_of_pulse)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->cap_timer_cfg.timer_en = 1;
    MCPWM[mcpwm_num]->cap_cfg_ch[cap_sig].en = 1;
    MCPWM[mcpwm_num]->cap_cfg_ch[cap_sig].mode = (1 << cap_edge);
    MCPWM[mcpwm_num]->cap_cfg_ch[cap_sig].prescale = num_of_pulse;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_capture_disable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->cap_cfg_ch[cap_sig].en = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

uint32_t mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    return MCPWM[mcpwm_num]->cap_val_ch[cap_sig];
}

uint32_t mcpwm_capture_signal_get_edge(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    if (cap_sig == MCPWM_SELECT_CAP0) {
        return ( MCPWM[mcpwm_num]->cap_status.cap0_edge + 1);
    } else if (cap_sig == MCPWM_SELECT_CAP1) {
        return (MCPWM[mcpwm_num]->cap_status.cap1_edge + 1);
    } else {   //MCPWM_SELECT_CAP2
        return (MCPWM[mcpwm_num]->cap_status.cap2_edge + 1);
    }
    return 0;
}

esp_err_t mcpwm_sync_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_sync_signal_t sync_sig,
                            uint32_t phase_val)
{
    uint32_t set_phase;
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    set_phase = (MCPWM[mcpwm_num]->timer[timer_num].period.period) * (phase_val) / 1000;
    MCPWM[mcpwm_num]->timer[timer_num].sync.timer_phase = set_phase;
    if (timer_num == MCPWM_TIMER_0) {
        MCPWM[mcpwm_num]->timer_synci_cfg.t0_in_sel = sync_sig;
    } else if (timer_num == MCPWM_TIMER_1) {
        MCPWM[mcpwm_num]->timer_synci_cfg.t1_in_sel = sync_sig;
    } else {   //MCPWM_TIMER_2
        MCPWM[mcpwm_num]->timer_synci_cfg.t2_in_sel = sync_sig;
    }
    MCPWM[mcpwm_num]->timer[timer_num].sync.out_sel = 0;
    MCPWM[mcpwm_num]->timer[timer_num].sync.in_en = 1;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}

esp_err_t mcpwm_sync_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(timer_num < MCPWM_TIMER_MAX, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&mcpwm_spinlock);
    MCPWM[mcpwm_num]->timer[timer_num].sync.in_en = 0;
    portEXIT_CRITICAL(&mcpwm_spinlock);
    return ESP_OK;
}
esp_err_t mcpwm_isr_register(mcpwm_unit_t mcpwm_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle)
{
    esp_err_t ret;
    MCPWM_CHECK(mcpwm_num < MCPWM_UNIT_MAX, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(fn != NULL, MCPWM_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    ret = esp_intr_alloc((ETS_PWM0_INTR_SOURCE + mcpwm_num), intr_alloc_flags, fn, arg, handle);
    return ret;
}
