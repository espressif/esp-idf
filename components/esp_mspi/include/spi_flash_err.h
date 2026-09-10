/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_FLASH_OP_FAIL           (ESP_ERR_FLASH_BASE+1) ///< aligned with rom
#define ESP_ERR_FLASH_OP_TIMEOUT        (ESP_ERR_FLASH_BASE+2) ///< aligned with rom

#ifdef __cplusplus
}
#endif
