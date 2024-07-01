/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This file contains APIs which have different implementation across different targets e.g., Linux, ESP.

#pragma once

#include <stdint.h>

/* @brief
 *
 *      Behaviour
 *      Linux:
 *          Returns the system time (64 bit) using gettimeofday in microseconds. This shall get changed if someone changes the system time using settimeofday
 *      ESP targets:
 *          Returns the time (64 bit) since boot obtained using esp_timer_get_time() in microseconds
 * @return
 *      time    uint64_t bit time value
 *
 */
uint64_t esp_tls_get_platform_time(void);
