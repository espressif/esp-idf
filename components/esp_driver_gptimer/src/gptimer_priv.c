/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_private/gptimer.h"
#include "gptimer_priv.h"

static const char *TAG = "gptimer";

esp_err_t gptimer_get_intr_handle(gptimer_handle_t timer, intr_handle_t *ret_intr_handle)
{
    ESP_RETURN_ON_FALSE(timer && ret_intr_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_intr_handle = timer->intr;
    return ESP_OK;
}

esp_err_t gptimer_get_pm_lock(gptimer_handle_t timer, esp_pm_lock_handle_t *ret_pm_lock)
{
    ESP_RETURN_ON_FALSE(timer && ret_pm_lock, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *ret_pm_lock = timer->pm_lock;
    return ESP_OK;
}
