// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "sdmmc_cmd.h"
#include "driver/sdmmc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register SD/MMC diskio driver
 *
 * @param pdrv  drive number
 * @param card  pointer to sdmmc_card_t structure describing a card; card should be initialized before calling f_mount.
 */
void ff_diskio_register_sdmmc(unsigned char pdrv, sdmmc_card_t* card);

/**
 * @brief Get the driver number corresponding to a card
 *
 * @param card The card to get its driver
 * @return Driver number to the card
 */
BYTE ff_diskio_get_pdrv_card(const sdmmc_card_t* card);

#ifdef __cplusplus
}
#endif

