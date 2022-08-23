/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdmmc_cmd.h"
#include "driver/sdmmc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable/disable SD card status checking
 *
 * @param pdrv   drive number
 * @param enable mock ff_sdmmc_status function (return 0)
 */
void ff_sdmmc_set_disk_status_check(BYTE pdrv, bool enable);

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
