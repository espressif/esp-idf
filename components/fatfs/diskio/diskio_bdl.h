/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_blockdev.h"
#include "esp_err.h"

/**
 * @brief Register a BDL (Block Device Layer) device as a FatFS diskio driver
 *
 * The FatFS logical sector size is derived from BDL geometry at registration
 * time as:  LCM(FF_MIN_SS, read_size, write_size [, erase_size]).
 * erase_size is included only when the result still fits within FF_MAX_SS;
 * this makes the sector erase-aligned for NOR-style devices (small erase
 * blocks) and page-aligned for NAND-style devices (large erase blocks,
 * erase handled internally by an FTL/WL layer).  No BDL flags are
 * inspected — the NOR/NAND distinction is implicit in the geometry.
 *
 * The computed sector size is cached and used for all subsequent I/O.
 * GET_BLOCK_SIZE returns erase_size / sector_size (minimum 1) so that
 * FatFS can align clusters to erase boundaries.
 *
 * Erase-before-write is performed when device_flags.erase_before_write or
 * device_flags.and_type_write is set and the write range is aligned to
 * geometry.erase_size.  Read-only
 * devices are supported (write returns RES_WRPRT, status returns
 * STA_PROTECT).
 *
 * @param pdrv          Drive number (0..FF_VOLUMES-1)
 * @param bdl_handle    BDL device handle providing the storage
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if pdrv is out of range, bdl_handle is invalid,
 *        or geometry is incompatible with FatFS (sector size not a power of
 *        two or exceeds FF_MAX_SS)
 */
esp_err_t ff_diskio_register_bdl(unsigned char pdrv, esp_blockdev_handle_t bdl_handle);

/**
 * @brief Get the drive number associated with a BDL handle
 *
 * @param bdl_handle    BDL device handle to look up
 *
 * @return Drive number (0..FF_VOLUMES-1) or 0xFF if not found
 */
unsigned char ff_diskio_get_pdrv_bdl(esp_blockdev_handle_t bdl_handle);

/**
 * @brief Clear the internal BDL handle association for a given handle
 *
 * @param bdl_handle    BDL device handle to clear
 */
void ff_diskio_clear_pdrv_bdl(esp_blockdev_handle_t bdl_handle);

#ifdef __cplusplus
}
#endif
