/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_flash.h"

esp_err_t esp_flash_get_size(esp_flash_t *chip, uint32_t *out_size)
{
    *out_size = 1024 * 1024 * 1024;
    return ESP_OK;
}
