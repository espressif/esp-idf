/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_periph.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_hal.h"
#include "hal/gpio_hal.h"
#include "hal/mcpwm_ll.h"
#include "driver/mcpwm.h"
#include "driver/periph_ctrl.h"

static const char *TAG = "mcpwm";

#define MCPWM_DRIVER_INIT_ERROR "MCPWM DRIVER NOT INITIALIZED"
#define MCPWM_GROUP_NUM_ERROR   "MCPWM GROUP NUM ERROR"
#define MCPWM_PRESCALE_ERROR    "MCPWM PRESCALE ERROR"
#define MCPWM_TIMER_ERROR       "MCPWM TIMER NUM ERROR"
#define MCPWM_CAPTURE_ERROR     "MCPWM CAPTURE NUM ERROR"
#define MCPWM_PARAM_ADDR_ERROR  "MCPWM PARAM ADDR ERROR"
#define MCPWM_DUTY_TYPE_ERROR   "MCPWM DUTY TYPE ERROR"
#define MCPWM_GPIO_ERROR        "MCPWM GPIO NUM ERROR"
#define MCPWM_GEN_ERROR         "MCPWM GENERATOR ERROR"
#define MCPWM_DT_ERROR          "MCPWM DEADTIME TYPE ERROR"

#define MCPWM_GROUP_CLK_PRESCALE (16)
#define MCPWM_GROUP_CLK_HZ (SOC_MCPWM_BASE_CLK_HZ / MCPWM_GROUP_CLK_PRESCALE)
#define MCPWM_TIMER_CLK_HZ (MCPWM_GROUP_CLK_HZ / 10)

_Static_assert(SOC_MCPWM_OPERATORS_PER_GROUP >= SOC_MCPWM_TIMERS_PER_GROUP, "This driver assumes the timer num equals to the operator num.");
_Static_assert(SOC_MCPWM_COMPARATORS_PER_OPERATOR >= SOC_MCPWM_GENERATORS_PER_OPERATOR, "This driver assumes the generator num equals to the generator num.");
_Static_assert(SOC_MCPWM_GENERATORS_PER_OPERATOR == 2, "This driver assumes the generator num equals to 2.");

#define MCPWM_TIMER_ID_CHECK(mcpwm_num, timer_num)                                                                  \
    do {                                                                                                            \
        ESP_RETURN_ON_FALSE((mcpwm_num) < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);       \
        ESP_RETURN_ON_FALSE((timer_num) < SOC_MCPWM_TIMERS_PER_GROUP, ESP_ERR_INVALID_ARG, TAG, MCPWM_TIMER_ERROR); \
    } while (0)

#define MCPWM_TIMER_CHECK(mcpwm_num, timer_num)                                                               \
    do {                                                                                                      \
        MCPWM_TIMER_ID_CHECK(mcpwm_num, timer_num);                                                           \
        ESP_RETURN_ON_FALSE(context[mcpwm_num].hal.dev, ESP_ERR_INVALID_STATE, TAG, MCPWM_DRIVER_INIT_ERROR); \
    } while (0)

#define MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen)                                             \
    do {                                                                                       \
        MCPWM_TIMER_CHECK(mcpwm_num, timer_num);                                               \
        ESP_RETURN_ON_FALSE((gen) < MCPWM_GEN_MAX, ESP_ERR_INVALID_ARG, TAG, MCPWM_GEN_ERROR); \
    } while (0)

typedef struct {
    mcpwm_hal_context_t hal;
    portMUX_TYPE spinlock;
} mcpwm_context_t;

static mcpwm_context_t context[SOC_MCPWM_GROUPS] = {
    [0 ... SOC_MCPWM_GROUPS - 1] = {
        .spinlock = portMUX_INITIALIZER_UNLOCKED,
    }
};

typedef void (*mcpwm_ll_gen_set_event_action_t)(mcpwm_dev_t *mcpwm, int op, int gen, int action);

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
    if (gpio_num < 0) { // ignore on minus gpio number
        return ESP_OK;
    }

    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, MCPWM_GPIO_ERROR);

    if (io_signal <= MCPWM2B) { // Generator output signal
        ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, MCPWM_GPIO_ERROR);
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        int operator_id = io_signal / 2;
        int generator_id = io_signal % 2;
        esp_rom_gpio_connect_out_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].operators[operator_id].generators[generator_id].pwm_sig, 0, 0);
    } else if (io_signal <= MCPWM_SYNC_2) { // External sync input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        int gpio_sync_id = io_signal - MCPWM_SYNC_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].gpio_synchros[gpio_sync_id].sync_sig, 0);
    } else if (io_signal <= MCPWM_FAULT_2) { // Fault input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        int fault_id = io_signal - MCPWM_FAULT_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].gpio_faults[fault_id].fault_sig, 0);
    } else if (io_signal >= MCPWM_CAP_0 && io_signal <= MCPWM_CAP_2) { // Capture input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        int capture_id = io_signal - MCPWM_CAP_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].captures[capture_id].cap_sig, 0);
    }
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    return ESP_OK;
}

esp_err_t mcpwm_set_pin(mcpwm_unit_t mcpwm_num, const mcpwm_pin_config_t *mcpwm_pin)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
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
    mcpwm_ll_timer_set_execute_command(context[mcpwm_num].hal.dev, timer_num, MCPWM_TIMER_START_NO_STOP);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_set_execute_command(context[mcpwm_num].hal.dev, timer_num, MCPWM_TIMER_STOP_AT_ZERO);
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

    mcpwm_ll_timer_update_period_at_once(hal->dev, timer_num);
    uint32_t previous_peak = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false);
    uint32_t new_peak = MCPWM_TIMER_CLK_HZ / frequency;
    mcpwm_ll_timer_set_peak(hal->dev, timer_num, new_peak, false);
    // keep the duty cycle unchanged
    float scale = ((float)new_peak) / previous_peak;
    // the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    uint32_t previous_cmp_a = mcpwm_ll_operator_get_compare_value(hal->dev, op, 0);
    uint32_t previous_cmp_b = mcpwm_ll_operator_get_compare_value(hal->dev, op, 1);
    // update compare value immediately
    mcpwm_ll_operator_update_compare_at_once(hal->dev, op, 0);
    mcpwm_ll_operator_update_compare_at_once(hal->dev, op, 1);
    mcpwm_ll_operator_set_compare_value(hal->dev, op, 0, (uint32_t)(previous_cmp_a * scale));
    mcpwm_ll_operator_set_compare_value(hal->dev, op, 1, (uint32_t)(previous_cmp_b * scale));

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
    uint32_t set_duty = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false) * duty / 100;
    mcpwm_ll_operator_set_compare_value(hal->dev, op, cmp, set_duty);
    mcpwm_ll_operator_enable_update_compare_on_tez(hal->dev, op, cmp, true);
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
    // the timer resolution is fixed to 1us in the driver, so duty_in_us is the same to compare value
    mcpwm_ll_operator_set_compare_value(hal->dev, op, cmp, duty_in_us);
    mcpwm_ll_operator_enable_update_compare_on_tez(hal->dev, op, cmp, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty_type(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen,
                              mcpwm_duty_type_t duty_type)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    ESP_RETURN_ON_FALSE(duty_type < MCPWM_DUTY_MODE_MAX, ESP_ERR_INVALID_ARG, TAG, MCPWM_DUTY_TYPE_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    mcpwm_critical_enter(mcpwm_num);
    switch (mcpwm_ll_timer_get_count_mode(hal->dev, timer_num)) {
    case MCPWM_TIMER_COUNT_MODE_UP:
        if (duty_type == MCPWM_DUTY_MODE_0) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_GEN_ACTION_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_PEAK, MCPWM_GEN_ACTION_KEEP);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_1) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_GEN_ACTION_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_NO_CHANGE);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
        } else if (duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_LOW) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
        }
        break;
    case MCPWM_TIMER_COUNT_MODE_DOWN:
        if (duty_type == MCPWM_DUTY_MODE_0) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_NO_CHANGE);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
        } else if (duty_type == MCPWM_DUTY_MODE_1) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_NO_CHANGE);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_LOW) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
        }
        break;
    case MCPWM_TIMER_COUNT_MODE_UP_DOWN:
        if (duty_type == MCPWM_DUTY_MODE_0) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
        } else if (duty_type == MCPWM_DUTY_MODE_1) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_LOW) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_ZERO, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_PEAK, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
        }
        break;
    default:
        break;
    }
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_config_t *mcpwm_conf)
{
    const int op = timer_num;
    MCPWM_TIMER_ID_CHECK(mcpwm_num, op);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    periph_module_enable(mcpwm_periph_signals.groups[mcpwm_num].module);
    mcpwm_hal_init_config_t config = {
        .host_id = mcpwm_num
    };
    mcpwm_hal_init(hal, &config);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_group_set_clock_prescale(hal->dev, MCPWM_GROUP_CLK_PRESCALE);
    mcpwm_ll_group_enable_shadow_mode(hal->dev);
    mcpwm_ll_group_flush_shadow(hal->dev);
    mcpwm_ll_timer_set_clock_prescale(hal->dev, timer_num, MCPWM_GROUP_CLK_HZ / MCPWM_TIMER_CLK_HZ);
    mcpwm_ll_timer_set_count_mode(hal->dev, timer_num, mcpwm_conf->counter_mode);
    mcpwm_ll_timer_update_period_at_once(hal->dev, timer_num);
    mcpwm_ll_timer_set_peak(hal->dev, timer_num, MCPWM_TIMER_CLK_HZ / mcpwm_conf->frequency, false);
    mcpwm_ll_operator_select_timer(hal->dev, timer_num, timer_num); //the driver currently always use the timer x for operator x
    mcpwm_critical_exit(mcpwm_num);

    mcpwm_set_duty(mcpwm_num, timer_num, 0, mcpwm_conf->cmpr_a);
    mcpwm_set_duty(mcpwm_num, timer_num, 1, mcpwm_conf->cmpr_b);
    mcpwm_set_duty_type(mcpwm_num, timer_num, 0, mcpwm_conf->duty_mode);
    mcpwm_set_duty_type(mcpwm_num, timer_num, 1, mcpwm_conf->duty_mode);
    mcpwm_start(mcpwm_num, timer_num);

    return ESP_OK;
}

uint32_t mcpwm_get_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    unsigned int group_clock = SOC_MCPWM_BASE_CLK_HZ / mcpwm_ll_group_get_clock_prescale(hal->dev);
    unsigned int timer_clock = group_clock / mcpwm_ll_timer_get_clock_prescale(hal->dev, timer_num);
    mcpwm_critical_exit(mcpwm_num);
    return (uint32_t)timer_clock;
}

float mcpwm_get_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    float duty = 100.0 * mcpwm_ll_operator_get_compare_value(hal->dev, op, gen) / mcpwm_ll_timer_get_peak(hal->dev, timer_num, false);
    mcpwm_critical_exit(mcpwm_num);
    return duty;
}

esp_err_t mcpwm_set_signal_high(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    return mcpwm_set_duty_type(mcpwm_num, timer_num, gen, MCPWM_HAL_GENERATOR_MODE_FORCE_HIGH);
}

esp_err_t mcpwm_set_signal_low(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    return mcpwm_set_duty_type(mcpwm_num, timer_num, gen, MCPWM_HAL_GENERATOR_MODE_FORCE_LOW);
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
    mcpwm_ll_carrier_set_prescale(context[mcpwm_num].hal.dev, op, carrier_period + 1);
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
    mcpwm_ll_carrier_set_oneshot_width(context[mcpwm_num].hal.dev, op, pulse_width + 1);
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

    mcpwm_carrier_enable(mcpwm_num, timer_num);
    mcpwm_carrier_set_period(mcpwm_num, timer_num, carrier_conf->carrier_period);
    mcpwm_carrier_set_duty_cycle(mcpwm_num, timer_num, carrier_conf->carrier_duty);
    if (carrier_conf->carrier_os_mode == MCPWM_ONESHOT_MODE_EN) {
        mcpwm_carrier_oneshot_mode_enable(mcpwm_num, timer_num, carrier_conf->pulse_width_in_os);
    } else {
        mcpwm_carrier_oneshot_mode_disable(mcpwm_num, timer_num);
    }
    mcpwm_carrier_output_invert(mcpwm_num, timer_num, carrier_conf->carrier_ivt_mode);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_in_invert(hal->dev, op, false);
    mcpwm_critical_exit(mcpwm_num);

    return ESP_OK;
}

esp_err_t mcpwm_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_deadtime_type_t dt_mode,
                                uint32_t red, uint32_t fed)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    ESP_RETURN_ON_FALSE(dt_mode < MCPWM_DEADTIME_TYPE_MAX, ESP_ERR_INVALID_ARG, TAG, MCPWM_DT_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_deadtime_enable_update_delay_on_tez(hal->dev, op, true);
    // The dead time delay unit equals to MCPWM group resolution
    mcpwm_ll_deadtime_resolution_to_timer(hal->dev, op, false);
    mcpwm_ll_deadtime_set_rising_delay(hal->dev, op, red + 1);
    mcpwm_ll_deadtime_set_falling_delay(hal->dev, op, fed + 1);
    switch (dt_mode) {
    case MCPWM_BYPASS_RED:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, true);     // S1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 1);  // S5
        break;
    case MCPWM_BYPASS_FED:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, true);     // S0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5
        break;
    case MCPWM_ACTIVE_HIGH_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 1);  // S5
        break;
    case MCPWM_ACTIVE_LOW_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);   // S0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);   // S1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, true); // S2
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, true); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0); // S4
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 1); // S5
        break;
    case MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, true);  // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 1);  // S5
        break;
    case MCPWM_ACTIVE_LOW_COMPLIMENT_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, true);  // S2
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 1);  // S4
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5
        break;
    case MCPWM_ACTIVE_RED_FED_FROM_PWMXA:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 1);  // S4
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 0, true);   // S6
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 1, false);  // S7
        mcpwm_ll_deadtime_enable_deb(hal->dev, op, true);         // S8
        break;
    case MCPWM_ACTIVE_RED_FED_FROM_PWMXB:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 0, true);   // S6
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 1, false);  // S7
        mcpwm_ll_deadtime_enable_deb(hal->dev, op, true);         // S8
        break;
    default :
        break;
    }
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, true);     // S0
    mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, true);     // S1
    mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2
    mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
    mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
    mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5
    mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 0, false);  // S6
    mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 1, false);  // S7
    mcpwm_ll_deadtime_enable_deb(hal->dev, op, false);        // S8
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_init(mcpwm_unit_t mcpwm_num, mcpwm_fault_input_level_t intput_level, mcpwm_fault_signal_t fault_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_fault_enable_detection(hal->dev, fault_sig, true);
    mcpwm_ll_fault_set_active_level(hal->dev, fault_sig, intput_level);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_deinit(mcpwm_unit_t mcpwm_num, mcpwm_fault_signal_t fault_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_fault_enable_detection(hal->dev, fault_sig, false);
    for (int i = 0; i < SOC_MCPWM_OPERATORS_PER_GROUP; i++) {
        mcpwm_ll_fault_clear_ost(hal->dev, i); // make sure operator has exit the ost fault state totally
    }
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
    mcpwm_ll_fault_enable_cbc_mode(hal->dev, op, fault_sig, true);
    mcpwm_ll_fault_enable_cbc_refresh_on_tez(hal->dev, op, true);
    mcpwm_ll_fault_enable_oneshot_mode(hal->dev, op, fault_sig, false);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TRIP_TYPE_CBC, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_UP, MCPWM_TRIP_TYPE_CBC, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TRIP_TYPE_CBC, action_on_pwmxb);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_UP, MCPWM_TRIP_TYPE_CBC, action_on_pwmxb);
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
    mcpwm_ll_fault_clear_ost(hal->dev, op);
    mcpwm_ll_fault_enable_oneshot_mode(hal->dev, op, fault_sig, true);
    mcpwm_ll_fault_enable_cbc_mode(hal->dev, op, fault_sig, false);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TRIP_TYPE_OST, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_UP, MCPWM_TRIP_TYPE_OST, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TRIP_TYPE_OST, action_on_pwmxb);
    mcpwm_ll_generator_set_action_on_trip_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_UP, MCPWM_TRIP_TYPE_OST, action_on_pwmxb);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_capture_enable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge,
                               uint32_t num_of_pulse)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(num_of_pulse <= MCPWM_LL_MAX_CAPTURE_PRESCALE, ESP_ERR_INVALID_ARG, TAG, MCPWM_PRESCALE_ERROR);
    ESP_RETURN_ON_FALSE(cap_sig < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER, ESP_ERR_INVALID_ARG, TAG, MCPWM_CAPTURE_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    // enable MCPWM module incase user don't use `mcpwm_init` at all
    periph_module_enable(mcpwm_periph_signals.groups[mcpwm_num].module);
    mcpwm_hal_init_config_t init_config = {
        .host_id = mcpwm_num,
    };
    mcpwm_critical_enter(mcpwm_num);
    mcpwm_hal_init(hal, &init_config);
    mcpwm_ll_group_set_clock_prescale(hal->dev, MCPWM_GROUP_CLK_PRESCALE);
    mcpwm_ll_capture_enable_timer(hal->dev, true);
    mcpwm_ll_capture_enable_channel(hal->dev, cap_sig, true);
    mcpwm_ll_capture_enable_negedge(hal->dev, cap_sig, cap_edge & MCPWM_NEG_EDGE);
    mcpwm_ll_capture_enable_posedge(hal->dev, cap_sig, cap_edge & MCPWM_POS_EDGE);
    mcpwm_ll_capture_set_prescale(hal->dev, cap_sig, num_of_pulse + 1);
    // capture feature should be used with interupt, so enable it by default
    mcpwm_ll_intr_enable_capture(hal->dev, cap_sig, true);
    mcpwm_ll_intr_clear_capture_status(hal->dev, 1 << cap_sig);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_capture_disable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(cap_sig < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER, ESP_ERR_INVALID_ARG, TAG, MCPWM_CAPTURE_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_capture_enable_channel(hal->dev, cap_sig, false);
    mcpwm_ll_intr_enable_capture(hal->dev, cap_sig, false);
    mcpwm_critical_exit(mcpwm_num);
    periph_module_disable(mcpwm_periph_signals.groups[mcpwm_num].module);
    return ESP_OK;
}

uint32_t mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(cap_sig < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER, ESP_ERR_INVALID_ARG, TAG, MCPWM_CAPTURE_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    return mcpwm_ll_capture_get_value(hal->dev, cap_sig);
}

uint32_t mcpwm_capture_signal_get_edge(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(cap_sig < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER, ESP_ERR_INVALID_ARG, TAG, MCPWM_CAPTURE_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    return mcpwm_ll_capture_is_negedge(hal->dev, cap_sig) ? 2 : 1;
}

esp_err_t mcpwm_sync_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_sync_signal_t sync_sig,
                            uint32_t phase_val)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    uint32_t set_phase = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false) * phase_val / 1000;
    mcpwm_ll_timer_set_sync_phase_value(hal->dev, timer_num, set_phase);
    if (sync_sig >= MCPWM_SELECT_SYNC0) {
        mcpwm_ll_timer_set_timer_synchro(hal->dev, timer_num, sync_sig - MCPWM_SELECT_SYNC0);
    }
    mcpwm_ll_timer_sync_out_penetrate(hal->dev, timer_num);
    mcpwm_ll_timer_enable_sync_input(hal->dev, timer_num, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_sync_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_enable_sync_input(hal->dev, timer_num, false);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_isr_register(mcpwm_unit_t mcpwm_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle)
{
    esp_err_t ret;
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, MCPWM_PARAM_ADDR_ERROR);
    ret = esp_intr_alloc(mcpwm_periph_signals.groups[mcpwm_num].irq_id, intr_alloc_flags, fn, arg, handle);
    return ret;
}
