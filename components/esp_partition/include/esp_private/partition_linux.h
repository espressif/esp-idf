/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file partition_linux.h
 *
 * @brief Private API functions used for Linux-target emulation of the Partition APIs (host-side testing)
 */

/** @brief emulated sector size for the partition API on Linux */
#define ESP_PARTITION_EMULATED_SECTOR_SIZE 0x1000

/**
 * @brief Partition type to string conversion routine
 *
 * @param type Partition type, see esp_partition_type_t
 *
 * @return string equivalent of given partition type or "unknown" on mismatch
 */
const char* esp_partition_type_to_str(const uint32_t type);

/**
 * @brief Partition subtype to string conversion routine
 *
 * @param type Partition type, see esp_partition_type_t
 * @param subtype Partition subtype, see esp_partition_subtype_t
 *
 * @return string equivalent of given partition subtype or "unknown" on mismatch
 */
const char *esp_partition_subtype_to_str(const uint32_t type, const uint32_t subtype);

/**
 * @brief Creates memory emulation of SPI FLASH device (Linux host)
 *
 * The function creates a memory buffer to emulate SPI FLASH device and provides a pointer to its beginning, in order
 * to allow relevant Partition APIs run in host-emulated environment without any code change.
 *
 * The emulation buffer is actually a disk file mapped to the host memory, current version implements the following:
 * 1. create temporary file /tmp/idf-partition-XXXXXX (fixed size 4MB)
 * 2. mmap() whole file to the memory and set its contents to all 1s (SPI NOR flash default)
 * 3. upload build/partition_table/partition-table.bin (hard-wired path for now) to ESP_PARTITION_TABLE_OFFSET
 *    (from the beginning of the memory buffer, ie to the same offset as in real SPI FLASH)
 * 4. [optional: iterate through the partitions uploaded and print esp_partition_info_t details for each]
 * 5. set part_desc_addr_start[out] parameter to the memory buffer starting address
 *
 * The pointer returned in part_desc_addr_start is then used as it was regular SPI FLASH address.
 *
 * NOTES:
 * 1. the temporary file generated is not deleted automatically - the cleanup happens during the next host system reset
 * 2. the mmapped() section remains active until esp_partition_file_unmmap() is called
 * 3. mmap() is called with MAP_SHARED so the emulated SPI FLASH can be shared among processes
 *
 * @param[out] part_desc_addr_start output pointer to receive memory SPI FLASH buffer address
 *
 * @return
 *          - ESP_OK: Operation successful
 *          - ESP_ERR_NOT_FINISHED: Failed to generate temporary file
 *          - ESP_ERR_INVALID_SIZE (one of the following):
 *              - Failed to resize temporary file to required value
 *              - Failed to set filepointer in partition-table.bin
 *          - ESP_ERR_NO_MEM: Failed to mmap() the temporary file into the memory
 *          - ESP_ERR_NOT_FOUND: Couldn't open the partition_table.bin file
 *          - ESP_ERR_INVALID_STATE: Failed to upload partition_table into the memory
 */
esp_err_t esp_partition_file_mmap(const uint8_t **part_desc_addr_start);

/**
 * @brief Releases the memory of emulated SPI FLASH device (Linux host)
 *
 * The function releases the memory block previously allocated by esp_partition_file_mmap().
 * The buffer is freed by calling munmap() with emulated_buffer, buffer_size
 *
 * @return
 *      - ESP_OK: Operation successful
 *      - ESP_ERR_NO_MEM: The memory buffer was not allocated
 *      - ESP_ERR_INVALID_SIZE: The buffer size was 0
 *      - ESP_ERR_INVALID_RESPONSE: Failed to munmap() the emulation file from memory
 */
esp_err_t esp_partition_file_munmap(void);

#ifdef __cplusplus
}
#endif
