/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN YOUR APPLICATIONS
// The following APIs are for internal use, public to other IDF components, but not for users' applications.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get MCPWM timer phase
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @param[out] count_value Returned MCPWM timer phase
 * @param[out] direction Returned MCPWM timer counting direction
 * @return
 *      - ESP_OK: Get MCPWM timer status successfully
 *      - ESP_ERR_INVALID_ARG: Get MCPWM timer status failed because of invalid argument
 *      - ESP_FAIL: Get MCPWM timer status failed because of other error
 */
esp_err_t mcpwm_timer_get_phase(mcpwm_timer_handle_t timer, uint32_t *count_value, mcpwm_timer_direction_t *direction);

#ifdef __cplusplus
}
#endif
