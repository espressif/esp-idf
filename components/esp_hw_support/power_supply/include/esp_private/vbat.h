/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the VBAT (RTC Backup Battery) management system.
 *
 * @return
 *  - ESP_OK: Initialization was successful.
 */
esp_err_t esp_vbat_init(void);

#ifdef __cplusplus
}
#endif
