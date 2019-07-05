// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_image_format.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Update the flash id in g_rom_flashchip(global esp_rom_spiflash_chip_t structure).
 *
 * @return None
 */
void bootloader_flash_update_id();

/**
 * @brief Set the flash CS setup and hold time.
 *
 * @note CS setup time is recomemded to be 1.5T, and CS hold time is recommended to be 2.5T.
 *       cs_setup = 1, cs_setup_time = 0; cs_hold = 1, cs_hold_time = 1.
 *
 * @return None
 */
void bootloader_flash_cs_timing_config();

/**
 * @brief Configure SPI flash clock.
 *
 * @note This function only set clock frequency for SPI0.
 *
 * @param pfhdr Pointer to App image header, from where to fetch flash settings.
 *
 * @return None
 */
void bootloader_flash_clock_config(const esp_image_header_t* pfhdr);

/**
 * @brief Configure SPI flash gpio, include the IO matrix and drive strength configuration.
 *
 * @param pfhdr Pointer to App image header, from where to fetch flash settings.
 *
 * @return None
 */
void bootloader_flash_gpio_config(const esp_image_header_t* pfhdr);

/**
 * @brief Configure SPI flash read dummy based on different mode and frequency.
 *
 * @param pfhdr Pointer to App image header, from where to fetch flash settings.
 *
 * @return None
 */
void bootloader_flash_dummy_config(const esp_image_header_t* pfhdr);

#ifdef __cplusplus
}
#endif
