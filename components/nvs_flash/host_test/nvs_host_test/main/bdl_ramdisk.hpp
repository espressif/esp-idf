/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_blockdev.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RAM disk context structure
 */
typedef struct {
    uint8_t *data;          /*!< Pointer to RAM storage */
    size_t total_size;      /*!< Total size of the RAM disk */
    size_t erase_size;      /*!< Erase block size */
} bdl_ramdisk_ctx_t;

/**
 * @brief Create a new RAM disk block device
 *
 * @param blockdev_handle_out Pointer to store the created block device handle
 * @param total_size Total size of the RAM disk in bytes
 * @param erase_size Erase block size in bytes
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
 esp_err_t ramdisk_dev_get_blockdev(esp_blockdev_handle_t* blockdev_handle_out, const size_t total_size, const size_t erase_size);

/**
 * @brief Release a RAM disk block device
 *
 * @param handle Handle to the RAM disk to release
 * @return esp_err_t ESP_OK on success
 */
esp_err_t ramdisk_dev_release(esp_blockdev_handle_t handle);

#ifdef __cplusplus
}

/**
 * @brief C++ wrapper class for RAM disk
 */
class bdl_ramdisk {
public:
    bdl_ramdisk(const size_t total_size, const size_t erase_size) {
        handle = nullptr;
        ramdisk_dev_get_blockdev(&handle, total_size, erase_size);
    }

    ~bdl_ramdisk() {
        if (handle) {
            ramdisk_dev_release(handle);
        }
    }

    operator esp_blockdev_handle_t() const {
        return handle;
    }

    esp_blockdev_handle_t get_handle() const {
        return handle;
    }

private:
    esp_blockdev_handle_t handle;
};

#endif
