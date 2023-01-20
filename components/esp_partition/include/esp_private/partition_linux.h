/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
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

/** @brief emulated whole flash size for the partition API on Linux */
#define ESP_PARTITION_EMULATED_FLASH_SIZE 0x400000 //4MB fixed

/**
 * @brief Partition type to string conversion routine
 *
 * @param type Partition type, see esp_partition_type_t
 *
 * @return string equivalent of given partition type or "unknown" on mismatch
 */
const char *esp_partition_type_to_str(const uint32_t type);

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

/**
 * Functions for host tests
*/

/**
 * @brief Clears statistics gathered by emulated partition read/write/erase operations
 *
 */
void esp_partition_clear_stats(void);

/**
 * @brief Returns number of read operations called
 *
 * Function returns number of calls to the function esp_partition_read
 *
 * @return
 *      - number of calls to esp_partition_read since recent esp_partition_clear_stats
 */
size_t esp_partition_get_read_ops(void);

/**
 * @brief Returns number of write operations called
 *
 * Function returns number of calls to the function esp_partition_write
 *
 * @return
 *      - number of calls to esp_partition_write since recent esp_partition_clear_stats
 */
size_t esp_partition_get_write_ops(void);

/**
 * @brief Returns number of erase operations performed on behalf of calls to esp_partition_erase_range
 *
 * Function returns accumulated number of sectors erased on behalf of esp_partition_erase_range
 *
 * @return
 *      - total number of emulated sector erase operations on behalf of esp_partition_erase_range since recent esp_partition_clear_stats
 */
size_t esp_partition_get_erase_ops(void);

/**
 * @brief Returns total number of bytes read on behalf of esp_partition_read
 *
 * Function returns number of bytes read by esp_partition_read
 *
 * @return
 *      - total number of bytes read on behalf of esp_partition_read since recent esp_partition_clear_stats
 */
size_t esp_partition_get_read_bytes(void);

/**
 * @brief Returns total number of bytes written on behalf of esp_partition_write
 *
 * Function returns number of bytes written by esp_partition_write
 *
 * @return
 *      - total number of bytes written on behalf of esp_partition_write since recent esp_partition_clear_stats
 */
size_t esp_partition_get_write_bytes(void);

/**
 * @brief Returns estimated total time spent on partition operations.
 *
 * Function returns estimated total time spent in esp_partition_read,
 * esp_partition_write and esp_partition_erase_range operations.
 *
 * @return
 *      - estimated total time spent in read/write/erase operations in miliseconds
 */
size_t esp_partition_get_total_time(void);

/**
 * @brief Initializes emulation of failure caused by wear on behalf of write/erase operations
 *
 * Function initializes down counter emulating remaining write / erase cycles.
 * Once this counter reaches 0, emulation of all subsequent write / erase operations fails
 * Initial state of down counter is disabled.
 *
 * @param[in] count Number of remaining write / erase cycles before failure. Call with SIZE_MAX to disable simulation of flash wear.
 *
*/
void esp_partition_fail_after(size_t count);

/**
 * @brief Returns count of erase operations performed on virtual emulated sector
 *
 * Function returns number of erase operatinos performed on virtual sector specified by the parameter sector.
 * The esp_parttion mapped address space is virtually split into sectors of the size ESP_PARTITION_EMULATED_SECTOR_SIZE.
 * Calls to the esp_partition_erase_range are impacting one or multiple virtual sectors, for each of them, the respective
 * count is incremented.
 *
 * @param[in] sector Virtual sector number to return erase count for
 *
 * @return
 *      - count of erase operations performed on virtual emulated sector
 *
*/
size_t esp_partition_get_sector_erase_count(size_t sector);

#ifdef __cplusplus
}
#endif
