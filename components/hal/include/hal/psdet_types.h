/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Brownout reset level
 */
typedef enum {
    BROWNOUT_RESET_LEVEL_CHIP,   // Reset the whole chip, including the analog part
    BROWNOUT_RESET_LEVEL_SYSTEM, // Reset the whole digital system, including RTC sub-system
} brownout_reset_level_t;

#ifdef __cplusplus
}
#endif
