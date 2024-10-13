/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The 32bit debug probe output can be split into two 16bit parts
 */
typedef enum {
    DEBUG_PROBE_SPLIT_LOWER16, ///< The lower 16 signals of the debug probe output
    DEBUG_PROBE_SPLIT_UPPER16, ///< The upper 16 signals of the debug probe output
} debug_probe_split_u16_t;

#ifdef __cplusplus
}
#endif
