/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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

#ifdef __cplusplus
}
#endif
