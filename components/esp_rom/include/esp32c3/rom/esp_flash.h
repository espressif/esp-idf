// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Note: Most of esp_flash APIs in ROM are compatible with headers in ESP-IDF, this function
   just adds ROM-specific parts
*/

struct spi_flash_chip_t;
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
