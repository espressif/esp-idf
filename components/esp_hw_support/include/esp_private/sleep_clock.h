/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_regdma.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/startup_internal.h"
#include "soc/soc_caps.h"

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
 * @brief SoC system clock retention initialize.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for system clock retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t sleep_clock_system_retention_init(void *arg);

#if CONFIG_MAC_BB_PD || CONFIG_BT_LE_SLEEP_ENABLE || CONFIG_IEEE802154_SLEEP_ENABLE
/**
 * @brief Modem system clock retention initialize.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for modem clock retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t sleep_clock_modem_retention_init(void *arg);
#endif

#ifdef __cplusplus
}
#endif
