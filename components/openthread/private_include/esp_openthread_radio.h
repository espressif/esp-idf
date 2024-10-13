/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread radio.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *
 */
esp_err_t esp_openthread_radio_init(const esp_openthread_platform_config_t *config);

/**
 * @brief This function deinitializes the OpenThread radio.
 *
 */
void esp_openthread_radio_deinit(void);

/**
 * @brief This function updates the radio fds and timeouts to the main loop.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_radio_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function performs the OpenThread radio process.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_radio_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

#ifdef __cplusplus
}
#endif
