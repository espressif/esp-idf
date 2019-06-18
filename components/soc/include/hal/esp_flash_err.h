// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include "esp_err.h"

/*
 * Possible errors returned from esp flash internal functions, these error codes
 * should be consistent with esp_err_t codes. But in order to make the source
 * files less dependent to esp_err_t, they use the error codes defined in this
 * replacable header. This header should ensure the consistency to esp_err_t.
 */

/* These should be consistent with esp_err_t errors */
#define ESP_ERR_FLASH_SIZE_NOT_MATCH    ESP_ERR_INVALID_SIZE        ///< The chip doesn't have enough space for the current partition table
#define ESP_ERR_FLASH_NO_RESPONSE       ESP_ERR_INVALID_RESPONSE    ///< Chip did not respond to the command, or timed out.


#define ESP_ERR_FLASH_ERR_BASE          0x6000  ///< Starting number of Flash error codes */
//The ROM code has already taken 1 and 2, to avoid possible conflicts, start from 3.
#define ESP_ERR_FLASH_NOT_INITIALISED   (ESP_ERR_FLASH_ERR_BASE+3) ///< esp_flash_chip_t structure not correctly initialised by esp_flash_init().
#define ESP_ERR_FLASH_UNSUPPORTED_HOST  (ESP_ERR_FLASH_ERR_BASE+4) ///< Requested operation isn't supported via this host SPI bus (chip->spi field).
#define ESP_ERR_FLASH_UNSUPPORTED_CHIP  (ESP_ERR_FLASH_ERR_BASE+5) ///< Requested operation isn't supported by this model of SPI flash chip.
#define ESP_ERR_FLASH_PROTECTED         (ESP_ERR_FLASH_ERR_BASE+6) ///< Write operation failed due to chip's write protection being enabled.


