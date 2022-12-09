/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "esp_check.h"
#include "glitch_filter_priv.h"
#include "hal/gpio_ll.h"

static const char *TAG = "gpio-filter";

/**
 * @brief Type of GPIO pin glitch filter
 */
typedef struct gpio_pin_glitch_filter_t {
    gpio_glitch_filter_t base;
} gpio_pin_glitch_filter_t;

static esp_err_t gpio_pin_glitch_filter_del(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "filter not in init state");
    gpio_pin_glitch_filter_t *pin_filter = __containerof(filter, gpio_pin_glitch_filter_t, base);
    free(pin_filter);
    return ESP_OK;
}

static esp_err_t gpio_pin_glitch_filter_enable(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "filter not in init state");
    gpio_ll_pin_filter_enable(NULL, filter->gpio_num);
    filter->fsm = GLITCH_FILTER_FSM_ENABLE;
    return ESP_OK;
}

static esp_err_t gpio_pin_glitch_filter_disable(gpio_glitch_filter_t *filter)
{
    ESP_RETURN_ON_FALSE(filter->fsm == GLITCH_FILTER_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "filter not in enable state");
    gpio_ll_pin_filter_disable(NULL, filter->gpio_num);
    filter->fsm = GLITCH_FILTER_FSM_INIT;
    return ESP_OK;
}

esp_err_t gpio_new_pin_glitch_filter(const gpio_pin_glitch_filter_config_t *config, gpio_glitch_filter_handle_t *ret_filter)
{
    esp_err_t ret = ESP_OK;
    gpio_pin_glitch_filter_t *filter = NULL;
    ESP_GOTO_ON_FALSE(config && ret_filter, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(GPIO_IS_VALID_GPIO(config->gpio_num), ESP_ERR_INVALID_ARG, err, TAG, "invalid gpio number");

    filter = heap_caps_calloc(1, sizeof(gpio_pin_glitch_filter_t), FILTER_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(filter, ESP_ERR_NO_MEM, err, TAG, "no memory for pin glitch filter");

    filter->base.gpio_num = config->gpio_num;
    filter->base.fsm = GLITCH_FILTER_FSM_INIT;
    filter->base.del = gpio_pin_glitch_filter_del;
    filter->base.enable = gpio_pin_glitch_filter_enable;
    filter->base.disable = gpio_pin_glitch_filter_disable;

    *ret_filter = &(filter->base);
    return ESP_OK;
err:
    if (filter) {
        free(filter);
    }
    return ret;
}
