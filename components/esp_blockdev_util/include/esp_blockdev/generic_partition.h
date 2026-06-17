/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <sys/types.h>
#include <stddef.h>

#include "esp_blockdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new partition over a given blockdev
 *
 * @param parent The underlying device
 * @param start Address of the start of the new partition. (Behaviour is undefined when this is not a multiple of block size for underlying device)
 * @param size Size of the new partition. (Behaviour is undefined when this is not a multiple of block size for underlying device)
 * @param out Where to store handle to the newly created block device. Will be unchanged upon failure.
 *
 * @return ESP_ERR_INVALID_ARG - Invalid argument was passed
 *         ESP_ERR_NO_MEM - Failed to allocate the struct
 *         ESP_OK
 */
esp_err_t esp_blockdev_generic_partition_get(esp_blockdev_handle_t parent, size_t start, size_t size, esp_blockdev_handle_t *out);

/**
 * @brief Translate virtual partition address to parents address space
 *
 * @param device The device for which to do the translation
 * @param address Address to be translated, relative to the partition start
 *
 * @return Parent-space address on success
 *         -1 on error (invalid argument or overflow)
 */
ssize_t esp_blockdev_generic_partition_translate_address_to_parent(esp_blockdev_handle_t device, size_t address);

/**
 * @brief Translate virtual partition address from parents address space
 *
 * @param device The device for which to do the translation
 * @param address Address to be translated, in the parent address space
 *
 * @return Partition-relative address on success
 *         -1 on error (invalid argument or underflow)
 */
ssize_t esp_blockdev_generic_partition_translate_address_to_child(esp_blockdev_handle_t device, size_t address);

#ifdef __cplusplus
}
#endif
