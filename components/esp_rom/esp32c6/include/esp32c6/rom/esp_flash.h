/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note: Most of esp_flash APIs in ROM are compatible with headers in ESP-IDF, this function
   just adds ROM-specific parts
*/

typedef struct esp_flash_t esp_flash_t;

/* Structure to wrap "global" data used by esp_flash in ROM */
typedef struct {
    /* Default SPI flash chip, ie main chip attached to the MCU
       This chip is used if the 'chip' argument passed to esp_flash_xxx API functions is ever NULL
    */
    esp_flash_t *default_chip;

    /* Global API OS notification start/end/chip_check functions

       These are used by ROM if no other host functions are configured.
    */
    struct {
        esp_err_t (*start)(esp_flash_t *chip);
        esp_err_t (*end)(esp_flash_t *chip, esp_err_t err);
        esp_err_t (*chip_check)(esp_flash_t **inout_chip);
    } api_funcs;
} esp_flash_rom_global_data_t;

/** Access a pointer to the global data used by the ROM spi_flash driver
 */
esp_flash_rom_global_data_t *esp_flash_get_rom_global_data(void);

#ifdef __cplusplus
}
#endif
