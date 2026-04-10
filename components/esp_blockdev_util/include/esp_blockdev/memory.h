/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_blockdev.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a memory-backed block device that wraps an existing buffer
 *
 * @param buffer Pointer to the storage backing the device
 * @param buffer_size Size of the storage buffer in bytes
 * @param geometry Desired geometry for the device; geometry->disk_size must not exceed buffer_size
 * @param read_only Set true to disallow write/erase operations on the mapped memory
 * @param out Output pointer that receives the created block device handle; unchanged on failure
 *
 * @return ESP_ERR_INVALID_ARG Invalid argument provided
 * @return ESP_ERR_INVALID_SIZE Provided buffer or geometry is incompatible
 * @return ESP_ERR_NO_MEM Memory allocation for device descriptor failed
 * @return ESP_OK on success
 */
esp_err_t esp_blockdev_memory_get_from_buffer(uint8_t *buffer, size_t buffer_size, const esp_blockdev_geometry_t *geometry, bool read_only, esp_blockdev_handle_t *out);

/**
 * @brief Create a memory-backed block device with internally allocated storage
 *
 * @param geometry Desired geometry for the device; geometry->disk_size determines the allocation size
 * @param caps Heap capability flags passed to heap_caps_malloc
 * @param out Output pointer that receives the created block device handle; unchanged on failure
 *
 * @return ESP_ERR_INVALID_ARG Invalid argument provided
 * @return ESP_ERR_INVALID_SIZE Allocation size exceeds supported limits
 * @return ESP_ERR_NO_MEM Memory allocation failed
 * @return ESP_OK on success
 */
esp_err_t esp_blockdev_memory_create_with_heap_caps(const esp_blockdev_geometry_t *geometry, uint32_t caps, esp_blockdev_handle_t *out);

#ifdef __cplusplus
}
#endif
