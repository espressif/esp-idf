/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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

#ifdef __cplusplus
}
#endif
