/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_private/glitch_filter_priv.h"

static const char *TAG = "gpio-filter";

/////////// Public abstract functions ///////////

esp_err_t gpio_del_glitch_filter(gpio_glitch_filter_handle_t filter)
{
    ESP_RETURN_ON_FALSE(filter, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return filter->del(filter);
}

esp_err_t gpio_glitch_filter_enable(gpio_glitch_filter_handle_t filter)
{
    ESP_RETURN_ON_FALSE(filter, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return filter->enable(filter);
}

esp_err_t gpio_glitch_filter_disable(gpio_glitch_filter_handle_t filter)
{
    ESP_RETURN_ON_FALSE(filter, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    return filter->disable(filter);
}
