/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sd_protocol_types.h"

typedef unsigned char BYTE;

/**
 * @brief Get count of all registered drives corresponding to a card
 *
 * @param card The card to get its drive number
 * @return Number of registered drives to the card
 */
BYTE ff_diskio_get_pdrv_cnt_card(const sdmmc_card_t* card);

#ifdef __cplusplus
}
#endif
