/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_clock.h
 *
 * This file contains declarations of digital peripheral clock retention related functions in light sleep mode.
 */

/**
 * @brief Whether to allow the TOP power domain to be powered off.
 *
 * In light sleep mode, only when the system can provide enough memory
 * for digital peripheral clock retention, the TOP power domain can be
 * powered off.
 *
 * @return True to allow power off
 */
bool clock_domain_pd_allowed(void);

/**
 * @brief  PCR module power down initialize
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on invalid sleep_retention_entries_create args
 *         No memory for the retention link
 */
esp_err_t sleep_clock_system_retention_init(void);

/**
 * @brief  PCR module power down deinitialize
 */
void sleep_clock_system_retention_deinit(void);

/**
 * @brief  Modem syscon module power down initialize
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on invalid sleep_retention_entries_create args
 *         No memory for the retention link
 */
esp_err_t sleep_clock_modem_retention_init(void);

/**
 * @brief  Modem syscon module power down deinitialize
 */
void sleep_clock_modem_retention_deinit(void);

#ifdef __cplusplus
}
#endif
