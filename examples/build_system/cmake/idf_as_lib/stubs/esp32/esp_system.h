/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_OK          0       /*!< esp_err_t value indicating success (no error) */

typedef int esp_err_t;

void esp_restart(void);

#ifdef __cplusplus
}
#endif
