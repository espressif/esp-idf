/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_partition.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Find unallocated space in the partition table.
 *
 * This function searches for a contiguous space in the specified flash memory chip
 * that can accommodate a region of the given size and is not allocated to any partition.
 * It starts searching from the specifies start_offset and continues until the end of the flash memory.
 *
 * @param[in] flash_chip     Pointer to the flash chip structure. If NULL, the default flash chip (internal) is used.
 * @param[in] required_size  Size of the required unallocated space in bytes.
 * @param[in] start_offset   Starting offset in the flash memory from where the search should begin.
 * @param[out] found_offset  Pointer where the offset of the found unallocated space will be stored.
 * @param[out] found_size    Pointer where the actual size of the found unallocated space will be stored (optional, if it is NULL).
 *
 * @return
 *      - ESP_OK: Unallocated space of the required size was found.
 *      - ESP_ERR_INVALID_ARG: The found_offset parameter is NULL or the required_size is invalid (e.g., 0).
 *      - ESP_ERR_NOT_FOUND: No suitable unallocated space was found.
 */
esp_err_t partition_utils_find_unallocated(esp_flash_t *flash_chip, size_t required_size, uint32_t start_offset, uint32_t *found_offset, size_t *found_size);

#ifdef __cplusplus
}
#endif
