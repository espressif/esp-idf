/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct timg_dev_t *gptimer_soc_handle_t; // GPTimer SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    gptimer_soc_handle_t dev; // Timer SOC layer handle (i.e. register base address)
    uint32_t timer_id;        // Timer ID (i.e. index of the timer in the group)
} timer_hal_context_t;

/**
 * @brief Init the timer hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param group_num The timer group number
 * @param timer_num The timer number
 */
void timer_hal_init(timer_hal_context_t *hal, uint32_t group_num, uint32_t timer_num);

/**
 * @brief Deinit timer hal context.
 *
 * @param hal Context of HAL layer
 */
void timer_hal_deinit(timer_hal_context_t *hal);

/**
 * @brief Load counter value into time-base counter
 *
 * @param hal Context of the HAL layer
 * @param load_val Counter value
 */
void timer_hal_set_counter_value(timer_hal_context_t *hal, uint64_t load_val);

/**
 * @brief Trigger a software capture event and then return the captured count value
 *
 * @param hal Context of the HAL layer
 * @return Counter value
 */
uint64_t timer_hal_capture_and_get_counter_value(timer_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
