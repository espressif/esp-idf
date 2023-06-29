/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_FREERTOS_USE_TICKLESS_IDLE
/**
 * @brief This function initializes the OpenThread sleep.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_sleep_init(void);

/**
 * @brief This function performs the OpenThread sleep process.
 *
 */
void esp_openthread_sleep_process(void);

/**
 * @brief This function performs the OpenThread wakeup process.
 *
 */
void esp_openthread_wakeup_process(void);
#endif

#ifdef __cplusplus
}
#endif
