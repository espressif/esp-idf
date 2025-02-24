/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "esp_system.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void * esp_flash_t;

esp_err_t esp_flash_get_size(esp_flash_t *chip, uint32_t *out_size);

#ifdef __cplusplus
}
#endif
