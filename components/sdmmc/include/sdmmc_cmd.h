// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include "esp_err.h"
#include "driver/sdmmc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Probe and initialize SD/MMC card using given host
 *
 * @note Only SD cards (SDSC and SDHC/SDXC) are supported now.
 *       Support for MMC/eMMC cards will be added later.
 *
 * @param host  pointer to structure defining host controller
 * @param out_card  pointer to structure which will receive information about the card when the function completes
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_card_init(const sdmmc_host_t* host,
        sdmmc_card_t* out_card);

/**
 * @brief Print information about the card to a stream
 * @param stream  stream obtained using fopen or fdopen
 * @param card  card information structure initialized using sdmmc_card_init
 */
void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card);

/**
 * Write given number of sectors to SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized using sdmmc_card_init
 * @param src   pointer to data buffer to read data from; data size must be equal to sector_count * card->csd.sector_size
 * @param start_sector  sector where to start writing
 * @param sector_count  number of sectors to write
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_write_sectors(sdmmc_card_t* card, const void* src,
        size_t start_sector, size_t sector_count);

/**
 * Write given number of sectors to SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized using sdmmc_card_init
 * @param dst   pointer to data buffer to write into; buffer size must be at least sector_count * card->csd.sector_size
 * @param start_sector  sector where to start reading
 * @param sector_count  number of sectors to read
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_read_sectors(sdmmc_card_t* card, void* dst,
        size_t start_sector, size_t sector_count);

#ifdef __cplusplus
}
#endif
