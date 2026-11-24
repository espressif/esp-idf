/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initializes the OpenThread alarm.
 *
 * @return
 *  -ESP_OK on success
 *  -ESP_ERR_NO_MEM on allocation failure
 *  -ESP_FAIL on other failures
 *
 */
esp_err_t esp_openthread_alarm_init(void);

/**
 * @brief This function deinitializes the OpenThread alarm.
 */
void esp_openthread_alarm_deinit(void);

/**
 * @brief  Updates the file descriptor with the OpenThread alarm timeout.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_alarm_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief  Performs the alarm process and triggers the fired timers for OpenThread.
 *
 * @param[in]   instance    The OpenThread instance.
 * @param[in]   mainloop    The main loop context.
 *
 * return
 *  - ESP_OK on success
 *  - ESP_ERROR on other failures
 *
 */
esp_err_t esp_openthread_alarm_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief Returns whether a target time has been reached.
 *
 * The comparison treats the timestamps as signed 32-bit values, so @p target must be within 2^31 ticks of @p now.
 *
 * @param[in] target    The target timestamp.
 * @param[in] now       The current timestamp, in the same unit as @p target.
 *
 * @return true when @p now is at or past @p target.
 */
bool esp_openthread_alarm_is_expired(uint32_t target, uint32_t now);

#ifdef __cplusplus
}
#endif
