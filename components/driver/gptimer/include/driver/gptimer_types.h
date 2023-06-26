/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/timer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of General Purpose Timer handle
 */
typedef struct gptimer_t *gptimer_handle_t;

/**
 * @brief GPTimer alarm event data
 */
typedef struct {
    uint64_t count_value; /*!< Current count value */
    uint64_t alarm_value; /*!< Current alarm value */
} gptimer_alarm_event_data_t;

/**
 * @brief Timer alarm callback prototype
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[in] edata Alarm event data, fed by driver
 * @param[in] user_ctx User data, passed from `gptimer_register_event_callbacks`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*gptimer_alarm_cb_t) (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

#ifdef __cplusplus
}
#endif
