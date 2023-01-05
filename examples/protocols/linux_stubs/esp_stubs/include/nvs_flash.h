/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_err.h"

#define ESP_ERR_NVS_BASE                    0x1100                     /*!< Starting number of error codes */
#define ESP_ERR_NVS_NO_FREE_PAGES           (ESP_ERR_NVS_BASE + 0x0d)  /*!< NVS partition doesn't contain any empty pages. This may happen if NVS partition was truncated. Erase the whole partition and call nvs_flash_init again. */
#define ESP_ERR_NVS_NEW_VERSION_FOUND       (ESP_ERR_NVS_BASE + 0x10)  /*!< NVS partition contains data in new format and cannot be recognized by this version of code */

esp_err_t nvs_flash_init(void);

esp_err_t nvs_flash_erase(void);
