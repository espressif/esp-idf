/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "esp_svpwm.h"

static const char *TAG = "esp_svpwm";

// mcpwm handler type
typedef struct mcpwm_svpwm_ctx {
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t operators[3];
    mcpwm_cmpr_handle_t comparators[3];
    mcpwm_gen_handle_t  generators[3][2];
} mcpwm_svpwm_ctx_t;

esp_err_t svpwm_new_inverter(const inverter_config_t *config, inverter_handle_t *ret_inverter)
{
    esp_err_t ret;
    ESP_RETURN_ON_FALSE(config && ret_inverter, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    mcpwm_svpwm_ctx_t *svpwm_dev = calloc(1, sizeof(mcpwm_svpwm_ctx_t));
    if (!svpwm_dev) {
        ESP_LOGE(TAG, "no memory");
        return ESP_ERR_NO_MEM;
    }
    ESP_GOTO_ON_ERROR(mcpwm_new_timer(&config->timer_config, &svpwm_dev->timer), err, TAG, "Create MCPWM timer failed");

    for (int i = 0; i < 3; i++) {
        ESP_GOTO_ON_ERROR(mcpwm_new_operator(&config->operator_config, &svpwm_dev->operators[i]), err, TAG, "Create MCPWM operator failed");
        ESP_GOTO_ON_ERROR(mcpwm_operator_connect_timer(svpwm_dev->operators[i], svpwm_dev->timer), err, TAG, "Connect operators to the same timer failed");
    }

    for (int i = 0; i < 3; i++) {
        ESP_GOTO_ON_ERROR(mcpwm_new_comparator(svpwm_dev->operators[i], &config->compare_config, &svpwm_dev->comparators[i]), err, TAG, "Create comparators failed");
        ESP_GOTO_ON_ERROR(mcpwm_comparator_set_compare_value(svpwm_dev->comparators[i], 0), err, TAG, "Set comparators failed");
    }

    mcpwm_generator_config_t gen_config = {};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            gen_config.gen_gpio_num = config->gen_gpios[i][j];
            ESP_GOTO_ON_ERROR(mcpwm_new_generator(svpwm_dev->operators[i], &gen_config, &svpwm_dev->generators[i][j]), err, TAG, "Create PWM generator pin %d failed", gen_config.gen_gpio_num);
        }
    }

    for (int i = 0; i < 3; i++) {
        ESP_GOTO_ON_ERROR(mcpwm_generator_set_actions_on_compare_event(svpwm_dev->generators[i][0],
                                                                       MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, svpwm_dev->comparators[i], MCPWM_GEN_ACTION_LOW),
                                                                       MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, svpwm_dev->comparators[i], MCPWM_GEN_ACTION_HIGH),
                                                                       MCPWM_GEN_COMPARE_EVENT_ACTION_END()), err, TAG, "Set generator actions failed");
    }

    for (int i = 0; i < 3; i++) {
        ESP_GOTO_ON_ERROR(mcpwm_generator_set_dead_time(svpwm_dev->generators[i][0], svpwm_dev->generators[i][0], &config->dt_config), err, TAG, "Setup deadtime failed");
        ESP_GOTO_ON_ERROR(mcpwm_generator_set_dead_time(svpwm_dev->generators[i][0], svpwm_dev->generators[i][1], &config->inv_dt_config), err, TAG, "Setup inv deadtime failed");
    }

    *ret_inverter = svpwm_dev;
    return ESP_OK;

err:
    free(svpwm_dev);
    return ret;
}

esp_err_t svpwm_inverter_register_cbs(inverter_handle_t handle, const mcpwm_timer_event_callbacks_t *event, void *user_ctx)
{
    ESP_RETURN_ON_FALSE(handle && event, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_ERROR(mcpwm_timer_register_event_callbacks(handle->timer, event, user_ctx), TAG, "register callbacks failed");
    return ESP_OK;
}

esp_err_t svpwm_inverter_start(inverter_handle_t handle, mcpwm_timer_start_stop_cmd_t command)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    if ((command != MCPWM_TIMER_STOP_EMPTY) && (command != MCPWM_TIMER_STOP_FULL)) {
        ESP_RETURN_ON_ERROR(mcpwm_timer_enable(handle->timer), TAG, "mcpwm timer enable failed");
    }
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(handle->timer, command), TAG, "mcpwm timer start failed");
    return ESP_OK;
}

esp_err_t svpwm_inverter_set_duty(inverter_handle_t handle, uint16_t u, uint16_t v, uint16_t w)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(handle->comparators[0], u), TAG, "set duty failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(handle->comparators[1], v), TAG, "set duty failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(handle->comparators[2], w), TAG, "set duty failed");
    return ESP_OK;
}

esp_err_t svpwm_del_inverter(inverter_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ESP_RETURN_ON_ERROR(mcpwm_timer_disable(handle->timer), TAG, "mcpwm timer disable failed");
    for (int i = 0; i < 3; i++) {
        ESP_RETURN_ON_ERROR(mcpwm_del_generator(handle->generators[i][0]), TAG, "free mcpwm positive generator failed");
        ESP_RETURN_ON_ERROR(mcpwm_del_generator(handle->generators[i][1]), TAG, "free mcpwm negative generator failed");
        ESP_RETURN_ON_ERROR(mcpwm_del_comparator(handle->comparators[i]), TAG, "free mcpwm comparator failed");
        ESP_RETURN_ON_ERROR(mcpwm_del_operator(handle->operators[i]), TAG, "free mcpwm operator failed");
    }
    ESP_RETURN_ON_ERROR(mcpwm_del_timer(handle->timer), TAG, "free mcpwm timer failed");
    free(handle);
    return ESP_OK;
}
