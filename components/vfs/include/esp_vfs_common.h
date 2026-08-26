/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Line ending settings
 */
typedef enum {
    ESP_LINE_ENDINGS_CRLF,//!< CR + LF
    ESP_LINE_ENDINGS_CR,  //!< CR
    ESP_LINE_ENDINGS_LF,  //!< LF
} esp_line_endings_t;


#ifdef __cplusplus
}
#endif
