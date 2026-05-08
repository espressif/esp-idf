/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Debug probe unit identifier (HP vs LP)
 */
typedef enum {
    DEBUG_PROBE_UNIT_HP = 0,
    DEBUG_PROBE_UNIT_LP = 1,
} debug_probe_unit_id_t;

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
