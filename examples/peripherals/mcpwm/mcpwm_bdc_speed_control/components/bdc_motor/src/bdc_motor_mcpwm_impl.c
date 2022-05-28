/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "driver/mcpwm_prelude.h"
#include "bdc_motor.h"
#include "bdc_motor_interface.h"

static const char *TAG = "bdc_motor_mcpwm";

typedef struct {
    bdc_motor_t base;
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t operator;
    mcpwm_cmpr_handle_t cmpa;
    mcpwm_cmpr_handle_t cmpb;
    mcpwm_gen_handle_t gena;
    mcpwm_gen_handle_t genb;
} bdc_motor_mcpwm_obj;

static esp_err_t bdc_motor_mcpwm_set_speed(bdc_motor_t *motor, uint32_t speed)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(mcpwm_motor->cmpa, speed), TAG, "set compare value failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(mcpwm_motor->cmpb, speed), TAG, "set compare value failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_enable(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_timer_enable(mcpwm_motor->timer), TAG, "enable timer failed");
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(mcpwm_motor->timer, MCPWM_TIMER_START_NO_STOP), TAG, "start timer failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_disable(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(mcpwm_motor->timer, MCPWM_TIMER_STOP_EMPTY), TAG, "stop timer failed");
    ESP_RETURN_ON_ERROR(mcpwm_timer_disable(mcpwm_motor->timer), TAG, "disable timer failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_forward(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->gena, -1, true), TAG, "disable force level for gena failed");
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->genb, 0, true), TAG, "set force level for genb failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_reverse(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->genb, -1, true), TAG, "disable force level for genb failed");
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->gena, 0, true), TAG, "set force level for gena failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_coast(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->gena, 0, true), TAG, "set force level for gena failed");
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->genb, 0, true), TAG, "set force level for genb failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_brake(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->gena, 1, true), TAG, "set force level for gena failed");
    ESP_RETURN_ON_ERROR(mcpwm_generator_set_force_level(mcpwm_motor->genb, 1, true), TAG, "set force level for genb failed");
    return ESP_OK;
}

static esp_err_t bdc_motor_mcpwm_del(bdc_motor_t *motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = __containerof(motor, bdc_motor_mcpwm_obj, base);
    mcpwm_del_generator(mcpwm_motor->gena);
    mcpwm_del_generator(mcpwm_motor->genb);
    mcpwm_del_comparator(mcpwm_motor->cmpa);
    mcpwm_del_comparator(mcpwm_motor->cmpb);
    mcpwm_del_operator(mcpwm_motor->operator);
    mcpwm_del_timer(mcpwm_motor->timer);
    free(mcpwm_motor);
    return ESP_OK;
}

esp_err_t bdc_motor_new_mcpwm_device(const bdc_motor_config_t *motor_config, const bdc_motor_mcpwm_config_t *mcpwm_config, bdc_motor_handle_t *ret_motor)
{
    bdc_motor_mcpwm_obj *mcpwm_motor = NULL;
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(motor_config && mcpwm_config && ret_motor, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    mcpwm_motor = calloc(1, sizeof(bdc_motor_mcpwm_obj));
    ESP_GOTO_ON_FALSE(mcpwm_motor, ESP_ERR_NO_MEM, err, TAG, "no mem for rmt motor");

    // mcpwm timer
    mcpwm_timer_config_t timer_config = {
        .group_id = mcpwm_config->group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = mcpwm_config->resolution_hz,
        .period_ticks = mcpwm_config->resolution_hz / motor_config->pwm_freq_hz,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_GOTO_ON_ERROR(mcpwm_new_timer(&timer_config, &mcpwm_motor->timer), err, TAG, "create MCPWM timer failed");

    mcpwm_operator_config_t operator_config = {
        .group_id = mcpwm_config->group_id,
    };
    ESP_GOTO_ON_ERROR(mcpwm_new_operator(&operator_config, &mcpwm_motor->operator), err, TAG, "create MCPWM operator failed");

    ESP_GOTO_ON_ERROR(mcpwm_operator_connect_timer(mcpwm_motor->operator, mcpwm_motor->timer), err, TAG, "connect timer and operator failed");

    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_GOTO_ON_ERROR(mcpwm_new_comparator(mcpwm_motor->operator, &comparator_config, &mcpwm_motor->cmpa), err, TAG, "create comparator failed");
    ESP_GOTO_ON_ERROR(mcpwm_new_comparator(mcpwm_motor->operator, &comparator_config, &mcpwm_motor->cmpb), err, TAG, "create comparator failed");

    // set the initial compare value for both comparators
    mcpwm_comparator_set_compare_value(mcpwm_motor->cmpa, 0);
    mcpwm_comparator_set_compare_value(mcpwm_motor->cmpb, 0);

    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = motor_config->pwma_gpio_num,
    };
    ESP_GOTO_ON_ERROR(mcpwm_new_generator(mcpwm_motor->operator, &generator_config, &mcpwm_motor->gena), err, TAG, "create generator failed");
    generator_config.gen_gpio_num = motor_config->pwmb_gpio_num;
    ESP_GOTO_ON_ERROR(mcpwm_new_generator(mcpwm_motor->operator, &generator_config, &mcpwm_motor->genb), err, TAG, "create generator failed");

    mcpwm_generator_set_actions_on_timer_event(mcpwm_motor->gena,
            MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
            MCPWM_GEN_TIMER_EVENT_ACTION_END());
    mcpwm_generator_set_actions_on_compare_event(mcpwm_motor->gena,
            MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_motor->cmpa, MCPWM_GEN_ACTION_LOW),
            MCPWM_GEN_COMPARE_EVENT_ACTION_END());
    mcpwm_generator_set_actions_on_timer_event(mcpwm_motor->genb,
            MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
            MCPWM_GEN_TIMER_EVENT_ACTION_END());
    mcpwm_generator_set_actions_on_compare_event(mcpwm_motor->genb,
            MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, mcpwm_motor->cmpb, MCPWM_GEN_ACTION_LOW),
            MCPWM_GEN_COMPARE_EVENT_ACTION_END());

    mcpwm_motor->base.enable = bdc_motor_mcpwm_enable;
    mcpwm_motor->base.disable = bdc_motor_mcpwm_disable;
    mcpwm_motor->base.forward = bdc_motor_mcpwm_forward;
    mcpwm_motor->base.reverse = bdc_motor_mcpwm_reverse;
    mcpwm_motor->base.coast = bdc_motor_mcpwm_coast;
    mcpwm_motor->base.brake = bdc_motor_mcpwm_brake;
    mcpwm_motor->base.set_speed = bdc_motor_mcpwm_set_speed;
    mcpwm_motor->base.del = bdc_motor_mcpwm_del;
    *ret_motor = &mcpwm_motor->base;
    return ESP_OK;

err:
    if (mcpwm_motor) {
        if (mcpwm_motor->gena) {
            mcpwm_del_generator(mcpwm_motor->gena);
        }
        if (mcpwm_motor->genb) {
            mcpwm_del_generator(mcpwm_motor->genb);
        }
        if (mcpwm_motor->cmpa) {
            mcpwm_del_comparator(mcpwm_motor->cmpa);
        }
        if (mcpwm_motor->cmpb) {
            mcpwm_del_comparator(mcpwm_motor->cmpb);
        }
        if (mcpwm_motor->operator) {
            mcpwm_del_operator(mcpwm_motor->operator);
        }
        if (mcpwm_motor->timer) {
            mcpwm_del_timer(mcpwm_motor->timer);
        }
        free(mcpwm_motor);
    }
    return ret;
}
