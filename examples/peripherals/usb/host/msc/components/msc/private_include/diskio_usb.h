/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mass storage disk initialization structure
 */
typedef struct {
    uint32_t block_size;    /**< Block size */
    uint32_t block_count;   /**< Block count */
} usb_disk_t;

/**
 * @brief Register mass storage disk to fat file system
 *
 * @param[in] pdrv Number of free drive obtained from ff_diskio_get_drive() function
 * @param[in] disk usb_disk_t structure
 */
void ff_diskio_register_msc(uint8_t pdrv, usb_disk_t *disk);

/**
 * @brief Obtains number of drive assigned to usb disk upon calling ff_diskio_register_msc()
 *
 * @param[in] disk usb_disk_t structure
 * @return Drive number
 */
uint8_t ff_diskio_get_pdrv_disk(const usb_disk_t *disk);

#ifdef __cplusplus
}
#endif //__cplusplus
