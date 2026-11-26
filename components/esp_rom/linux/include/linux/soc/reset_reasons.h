/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dummy to satisfy the requirement for this type on Linux targets.
 * Look at other reset_reasons.h files in IDF.
 */
typedef enum {
    RESET_REASON_CHIP_POWER_ON   = 0x01, // Power on reset
} soc_reset_reason_t;

/**
 * Linux stub keeps the level API local so esp_rom need not REQUIRES soc.
 */
typedef enum {
    SOC_RESET_LEVEL_CPU = 1,
    SOC_RESET_LEVEL_CORE,
    SOC_RESET_LEVEL_SYSTEM,
    SOC_RESET_LEVEL_CHIP,
} soc_reset_level_t;

static inline soc_reset_level_t soc_reset_get_level(soc_reset_reason_t reason)
{
    (void)reason;
    return SOC_RESET_LEVEL_CHIP;
}

#ifdef __cplusplus
}
#endif
