/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN YOUR APPLICATIONS
// The following APIs are for internal use, public to other IDF components, but not for users' applications.

#pragma once

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "driver/gptimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get GPTimer interrupt handle
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer()`
 * @param[out] ret_intr_handle Timer's internal interrupt handle
 * @return
 *      - ESP_OK: Get GPTimer interrupt handle successfully
 *      - ESP_ERR_INVALID_ARG: Get GPTimer interrupt handle failed because of invalid argument
 *      - ESP_FAIL: Get GPTimer interrupt handle failed because of other error
 */
esp_err_t gptimer_get_intr_handle(gptimer_handle_t timer, intr_handle_t *ret_intr_handle);

/**
 * @brief Get GPTimer power management lock
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer()`
 * @param[out] ret_pm_lock Timer's internal power management lock
 * @return
 *      - ESP_OK: Get GPTimer power management lock successfully
 *      - ESP_ERR_INVALID_ARG: Get GPTimer power management lock failed because of invalid argument
 *      - ESP_FAIL: Get GPTimer power management lock failed because of other error
 */
esp_err_t gptimer_get_pm_lock(gptimer_handle_t timer, esp_pm_lock_handle_t *ret_pm_lock);

/**
 * @brief Get the group_id from the timer handle
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer()`
 * @return
 *      - ESP_OK: Get GPTimer group_id from handler successfully
 *      - ESP_ERR_INVALID_ARG: Get GPTimer group_id failed because of invalid argument
 */
esp_err_t gptimer_get_group_id(gptimer_handle_t timer, int *group_id);

#ifdef __cplusplus
}
#endif
