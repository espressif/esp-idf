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
#include "soc/gpio_periph.h"
#include "driver/mcpwm.h"
#include "driver/periph_ctrl.h"
#include "sdkconfig.h"
#include "hal/mcpwm_hal.h"


typedef struct {
    mcpwm_hal_context_t hal;
    portMUX_TYPE spinlock;
} mcpwm_context_t;

#define CONTEXT_INITIALIZER() { \
    .spinlock = portMUX_INITIALIZER_UNLOCKED, \
    .hal = { \
        .prescale = MCPWM_CLK_PRESCL, \
    }, \
}


static const char *MCPWM_TAG = "MCPWM";

#define MCPWM_CHECK(a, str, ret_val) if (!(a)) {                       \
    ESP_LOGE(MCPWM_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);    \
    return (ret_val);                                  \
    }

#define MCPWM_DRIVER_INIT_ERROR "MCPWM DRIVER NOT INITIALIZED"
#define MCPWM_UNIT_NUM_ERROR    "MCPWM UNIT NUM ERROR"
#define MCPWM_TIMER_ERROR       "MCPWM TIMER NUM ERROR"
#define MCPWM_PARAM_ADDR_ERROR  "MCPWM PARAM ADDR ERROR"
#define MCPWM_DUTY_TYPE_ERROR   "MCPWM DUTY TYPE ERROR"
#define MCPWM_GPIO_ERROR        "MCPWM GPIO NUM ERROR"
#define MCPWM_GEN_ERROR         "MCPWM GENERATOR ERROR"
#define MCPWM_DB_ERROR          "MCPWM DEADTIME TYPE ERROR"

#define MCPWM_CLK_PRESCL 15       //MCPWM clock prescale
#define TIMER_CLK_PRESCALE 9      //MCPWM timer prescales
#define MCPWM_CLK (MCPWM_BASE_CLK/(MCPWM_CLK_PRESCL +1))
#define MCPWM_PIN_IGNORE    (-1)
#define OFFSET_FOR_GPIO_IDX_1  6
#define OFFSET_FOR_GPIO_IDX_2 75

_Static_assert(SOC_MCPWM_OP_NUM >= SOC_MCPWM_TIMER_NUM, "This driver assumes the timer num equals to the operator num.");
_Static_assert(SOC_MCPWM_COMPARATOR_NUM >= SOC_MCPWM_GENERATOR_NUM, "This driver assumes the generator num equals to the generator num.");
_Static_assert(SOC_MCPWM_GENERATOR_NUM == 2, "This driver assumes the generator num equals to 2.");

#define MCPWM_TIMER_ID_CHECK(mcpwm_num, timer_num) do {\
    MCPWM_CHECK((mcpwm_num) < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG); \
    MCPWM_CHECK((timer_num) < SOC_MCPWM_TIMER_NUM, MCPWM_TIMER_ERROR, ESP_ERR_INVALID_ARG); \
} while(0)

#define MCPWM_TIMER_CHECK(mcpwm_num, timer_num) do{\
    MCPWM_TIMER_ID_CHECK(mcpwm_num, timer_num); \
    MCPWM_CHECK(context[mcpwm_num].hal.dev != NULL, MCPWM_DRIVER_INIT_ERROR, ESP_ERR_INVALID_STATE); \
} while(0)

#define MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen) do{ \
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num); \
    MCPWM_CHECK(gen < MCPWM_GEN_MAX, MCPWM_GEN_ERROR, ESP_ERR_INVALID_ARG); \
} while(0)


static mcpwm_context_t context[SOC_MCPWM_PERIPH_NUM] = {
    CONTEXT_INITIALIZER(),
    CONTEXT_INITIALIZER(),
};


static inline void mcpwm_critical_enter(mcpwm_unit_t mcpwm_num)
{
    portENTER_CRITICAL(&context[mcpwm_num].spinlock);
}

static inline void mcpwm_critical_exit(mcpwm_unit_t mcpwm_num)
{
    portEXIT_CRITICAL(&context[mcpwm_num].spinlock);
}

esp_err_t mcpwm_gpio_init(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num)
{
    if (gpio_num == MCPWM_PIN_IGNORE) {
        //IGNORE
        return ESP_OK;
    }

    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
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
        } else if (io_signal >= MCPWM_SYNC_0 && io_signal <= MCPWM_FAULT_2) {
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
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
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
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_1, mcpwm_pin->mcpwm_fault1_in_num);  //FAULT1
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_2, mcpwm_pin->mcpwm_fault2_in_num);  //FAULT2
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_0,   mcpwm_pin->mcpwm_cap0_in_num);    //CAP0
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_1,   mcpwm_pin->mcpwm_cap1_in_num);    //CAP1
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_2,   mcpwm_pin->mcpwm_cap2_in_num);    //CAP2
    return ESP_OK;
}

esp_err_t mcpwm_start(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_timer_start(&context[mcpwm_num].hal, timer_num);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_timer_stop(&context[mcpwm_num].hal, timer_num);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint32_t frequency)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    hal->timer[timer_num].freq = frequency;
    mcpwm_hal_timer_update_basic(hal, timer_num);
    //update the operator to update the duty
    mcpwm_hal_operator_update_basic(hal, op);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, float duty)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    const int cmp = gen;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    hal->op[op].duty[cmp] = duty;
    mcpwm_hal_operator_update_comparator(hal, op, gen);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty_in_us(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, uint32_t duty_in_us)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    const int cmp = gen;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    hal->op[op].duty[cmp] = (100 * duty_in_us * hal->timer[timer_num].freq) / (1000 * 1000.);
    mcpwm_hal_operator_update_comparator(hal, op, gen);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty_type(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen,
                              mcpwm_duty_type_t duty_type)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    MCPWM_CHECK(duty_type < MCPWM_DUTY_MODE_MAX, MCPWM_DUTY_TYPE_ERROR, ESP_ERR_INVALID_ARG);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    hal->op[op].gen[gen] = (mcpwm_hal_generator_config_t) {
        .comparator = gen, //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
        .duty_type = duty_type,
    };
    mcpwm_hal_operator_update_generator(hal, op, gen);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}


esp_err_t mcpwm_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_config_t *mcpwm_conf)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_ID_CHECK(mcpwm_num, op);
    periph_module_enable(PERIPH_PWM0_MODULE + mcpwm_num);

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_hal_init_config_t init_config = {
        .host_id = mcpwm_num,
    };

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_init(hal, &init_config);
    mcpwm_hal_hw_init(hal);

    hal->timer[timer_num].timer_prescale = TIMER_CLK_PRESCALE;
    hal->timer[timer_num].freq = mcpwm_conf->frequency;
    hal->timer[timer_num].count_mode = mcpwm_conf->counter_mode;

    //the driver currently always use the timer x for operator x
    hal->op[op].timer = timer_num;
    hal->op[op].duty[0] = mcpwm_conf->cmpr_a;
    hal->op[op].duty[1] = mcpwm_conf->cmpr_b;

    mcpwm_hal_timer_update_basic(hal, timer_num);
    //update the comparer to keep the same duty rate
    mcpwm_hal_operator_update_basic(hal, op);

    for (int gen = 0; gen < SOC_MCPWM_GENERATOR_NUM; gen++) {
        hal->op[op].gen[gen] = (mcpwm_hal_generator_config_t) {
            .comparator = gen, //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
            .duty_type = mcpwm_conf->duty_mode,
        };
        mcpwm_hal_operator_update_generator(hal, op, gen);
    }

    mcpwm_hal_timer_start(hal, timer_num);
    mcpwm_critical_exit(mcpwm_num);

    return ESP_OK;
}

uint32_t mcpwm_get_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    return context[mcpwm_num].hal.timer[timer_num].freq;
}

float mcpwm_get_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    return context[mcpwm_num].hal.op[op].duty[gen];
}

STATIC_HAL_REG_CHECK(MCPWM, MCPWM_GEN_A, 0);
STATIC_HAL_REG_CHECK(MCPWM, MCPWM_GEN_B, 1);

esp_err_t mcpwm_set_signal_high(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    hal->op[op].gen[gen] = (mcpwm_hal_generator_config_t) {
        .comparator = gen, //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
        .duty_type = MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH,
    };
    mcpwm_hal_operator_update_generator(hal, op, gen);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_signal_low(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    hal->op[op].gen[gen] = (mcpwm_hal_generator_config_t) {
        .comparator = gen, //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
        .duty_type = MCPWM_HAL_GENERATOR_MODE_FORCE_LOW,
    };
    mcpwm_hal_operator_update_generator(hal, op, gen);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_enable(context[mcpwm_num].hal.dev, op, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_enable(context[mcpwm_num].hal.dev, op, false);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_set_period(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_period)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_set_prescale(context[mcpwm_num].hal.dev, op, carrier_period);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_set_duty_cycle(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_duty)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_set_duty(context[mcpwm_num].hal.dev, op, carrier_duty);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_oneshot_mode_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t pulse_width)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_set_oneshot_width(context[mcpwm_num].hal.dev, op, pulse_width);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_oneshot_mode_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    return mcpwm_carrier_oneshot_mode_enable(mcpwm_num, timer_num, 0);
}

esp_err_t mcpwm_carrier_output_invert(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num,
                                      mcpwm_carrier_out_ivt_t carrier_ivt_mode)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_out_invert(context[mcpwm_num].hal.dev, op, carrier_ivt_mode);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_carrier_config_t *carrier_conf)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_hal_carrier_conf_t carrier = {
        .period = carrier_conf->carrier_period,
        .duty = carrier_conf->carrier_duty,
        .inverted = carrier_conf->carrier_ivt_mode,
    };
    if (carrier_conf->carrier_os_mode == MCPWM_ONESHOT_MODE_EN) {
        carrier.oneshot_pulse_width = carrier_conf->pulse_width_in_os;
    } else {
        carrier.oneshot_pulse_width = 0;
    }

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_operator_enable_carrier(hal, op, &carrier);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_deadtime_type_t dt_mode,
                                uint32_t red, uint32_t fed)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    MCPWM_CHECK(dt_mode < MCPWM_DEADTIME_TYPE_MAX, MCPWM_DB_ERROR, ESP_ERR_INVALID_ARG);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_hal_deadzone_conf_t deadzone = {
        .red = red,
        .fed = fed,
        .mode = dt_mode,
    };

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_operator_update_deadzone(hal, op, &deadzone);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_hal_deadzone_conf_t deadzone = { .mode = MCPWM_DEADTIME_BYPASS };

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_operator_update_deadzone(hal, op, &deadzone);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_init(mcpwm_unit_t mcpwm_num, mcpwm_fault_input_level_t intput_level, mcpwm_fault_signal_t fault_sig)
{
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_fault_init(&context[mcpwm_num].hal, fault_sig, intput_level);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_deinit(mcpwm_unit_t mcpwm_num, mcpwm_fault_signal_t fault_sig)
{
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_fault_disable(&context[mcpwm_num].hal, fault_sig);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_set_cyc_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                   mcpwm_output_action_t action_on_pwmxa, mcpwm_output_action_t action_on_pwmxb)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, op);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_fault_cbc_enable_signal(hal->dev, op, fault_sig, true);
    mcpwm_ll_fault_oneshot_enable_signal(hal->dev, op, fault_sig, false);
    mcpwm_ll_fault_set_cyc_action(hal->dev, op, 0, action_on_pwmxa, action_on_pwmxa);
    mcpwm_ll_fault_set_cyc_action(hal->dev, op, 1, action_on_pwmxb, action_on_pwmxb);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_set_oneshot_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                       mcpwm_action_on_pwmxa_t action_on_pwmxa, mcpwm_action_on_pwmxb_t action_on_pwmxb)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, op);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_fault_oneshot_clear(hal, op);
    mcpwm_ll_fault_cbc_enable_signal(hal->dev, op, fault_sig, false);
    mcpwm_ll_fault_oneshot_enable_signal(hal->dev, op, fault_sig, true);
    mcpwm_ll_fault_set_oneshot_action(hal->dev, op, 0, action_on_pwmxa, action_on_pwmxa);
    mcpwm_ll_fault_set_oneshot_action(hal->dev, op, 1, action_on_pwmxb, action_on_pwmxb);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_capture_enable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge,
                               uint32_t num_of_pulse)
{
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    mcpwm_hal_init_config_t init_config = {
        .host_id = mcpwm_num,
    };
    mcpwm_hal_capture_config_t cap_conf = {
        .cap_edge = cap_edge,
        .prescale = num_of_pulse,
    };
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    //We have to do this here, since there is no standalone init function
    //without enabling any PWM channels.
    mcpwm_hal_init(hal, &init_config);
    mcpwm_hal_hw_init(hal);
    mcpwm_hal_capture_enable(hal, cap_sig, &cap_conf);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_capture_disable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_capture_disable(&context[mcpwm_num].hal, cap_sig);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

uint32_t mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);

    uint32_t captured_value;
    mcpwm_hal_capture_get_result(&context[mcpwm_num].hal, cap_sig, &captured_value, NULL);
    return captured_value;
}

uint32_t mcpwm_capture_signal_get_edge(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    mcpwm_capture_on_edge_t edge;
    mcpwm_hal_capture_get_result(&context[mcpwm_num].hal, cap_sig, NULL, &edge);
    return (edge == MCPWM_NEG_EDGE ? 2 : 1);
}

esp_err_t mcpwm_sync_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_sync_signal_t sync_sig,
                            uint32_t phase_val)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_hal_sync_config_t sync_config = {
        .reload_permillage = phase_val,
        .sync_sig = sync_sig,
    };

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_timer_enable_sync(hal, timer_num, &sync_config);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_sync_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_timer_disable_sync(&context[mcpwm_num].hal, timer_num);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_isr_register(mcpwm_unit_t mcpwm_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle)
{
    esp_err_t ret;
    MCPWM_CHECK(mcpwm_num < SOC_MCPWM_PERIPH_NUM, MCPWM_UNIT_NUM_ERROR, ESP_ERR_INVALID_ARG);
    MCPWM_CHECK(fn != NULL, MCPWM_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG);
    ret = esp_intr_alloc((ETS_PWM0_INTR_SOURCE + mcpwm_num), intr_alloc_flags, fn, arg, handle);
    return ret;
}
