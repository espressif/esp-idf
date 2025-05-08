/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Outputs a char to the configured primary and secondary console
 *
 * @param c char to output
 */
void esp_system_console_put_char(char c);

#ifdef __cplusplus
}
#endif
