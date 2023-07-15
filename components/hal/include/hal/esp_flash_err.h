/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Possible errors returned from esp flash internal functions, these error codes
 * should be consistent with esp_err_t codes. But in order to make the source
 * files less dependent to esp_err_t, they use the error codes defined in this
 * replacable header. This header should ensure the consistency to esp_err_t.
 */

enum {
    /* These codes should be consistent with esp_err_t errors. However, error codes with the same values are not
     * allowed in ESP-IDF. This is a workaround in order to not introduce a dependency between the "soc" and
     * "esp_common" components. The disadvantage is that the output of esp_err_to_name(ESP_ERR_FLASH_SIZE_NOT_MATCH)
     * will be ESP_ERR_INVALID_SIZE. */
    ESP_ERR_FLASH_SIZE_NOT_MATCH = ESP_ERR_INVALID_SIZE,  ///< The chip doesn't have enough space for the current partition table
    ESP_ERR_FLASH_NO_RESPONSE = ESP_ERR_INVALID_RESPONSE, ///< Chip did not respond to the command, or timed out.
};

//The ROM code has already taken 1 and 2, to avoid possible conflicts, start from 3.
#define ESP_ERR_FLASH_NOT_INITIALISED   (ESP_ERR_FLASH_BASE+3) ///< esp_flash_chip_t structure not correctly initialised by esp_flash_init().
#define ESP_ERR_FLASH_UNSUPPORTED_HOST  (ESP_ERR_FLASH_BASE+4) ///< Requested operation isn't supported via this host SPI bus (chip->spi field).
#define ESP_ERR_FLASH_UNSUPPORTED_CHIP  (ESP_ERR_FLASH_BASE+5) ///< Requested operation isn't supported by this model of SPI flash chip.
#define ESP_ERR_FLASH_PROTECTED         (ESP_ERR_FLASH_BASE+6) ///< Write operation failed due to chip's write protection being enabled.

#ifdef __cplusplus
}
#endif
