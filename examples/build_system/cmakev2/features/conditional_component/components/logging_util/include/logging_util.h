/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log an informational message
 *
 * @param message The message to log
 */
void log_info(const char *message);

/**
 * @brief Log a warning message
 *
 * @param message The message to log
 */
void log_warn(const char *message);

/**
 * @brief Log an error message
 *
 * @param message The message to log
 */
void log_error(const char *message);

#ifdef __cplusplus
}
#endif
