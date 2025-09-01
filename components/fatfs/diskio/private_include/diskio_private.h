/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "diskio.h"

/**
 * @brief Check if a drive number is registered
 *
 * @param ldrv  logical or physical drive number
 *
 * @return true if the drive number is registered, false otherwise
 */
bool ff_diskio_is_registered(BYTE ldrv);

/**
 * @brief Get sector size of a drive
 *
 * @param ldrv  logical or physical drive number
 * @param out_bytes_per_sector  output parameter to receive the sector size in bytes
 *
 * @return DRESULT indicating success or failure
 */
DRESULT ff_diskio_get_sector_size(BYTE ldrv, UINT* out_bytes_per_sector);

#ifdef __cplusplus
}
#endif
