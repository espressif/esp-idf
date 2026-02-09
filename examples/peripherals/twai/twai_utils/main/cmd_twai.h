/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* =============================================================================
 * MACRO DEFINITIONS
 * =============================================================================*/

/**
 * @brief Register TWAI commands with the console
 */
void register_twai_commands(void);
void unregister_twai_commands(void);

#ifdef __cplusplus
}
#endif
