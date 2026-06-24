/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_heap_caps.h"

#include "esp_blockdev.h"
#include "esp_blockdev/memory.h"

static const char *TAG = "esp_blockdev/memory";

typedef struct {
    esp_blockdev_t dev;
    uint8_t *buffer;
    size_t capacity;
    bool owns_buffer;
} esp_blockdev_memory_t;

static esp_err_t bd_memory_read(esp_blockdev_handle_t dev_handle, uint8_t *dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");
    ESP_RETURN_ON_FALSE(dst_buf != NULL, ESP_ERR_INVALID_ARG, TAG, "The destination buffer cannot be NULL");
    ESP_RETURN_ON_FALSE(data_read_len <= dst_buf_size, ESP_ERR_INVALID_SIZE, TAG, "Destination buffer too small");
    ESP_RETURN_ON_FALSE(src_addr + data_read_len <= dev_handle->geometry.disk_size, ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");

    esp_blockdev_memory_t *dev = (esp_blockdev_memory_t *)dev_handle;
    size_t offset = (size_t)src_addr;

    memcpy(dst_buf, dev->buffer + offset, data_read_len);

    return ESP_OK;
}

static esp_err_t bd_memory_write(esp_blockdev_handle_t dev_handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");
    ESP_RETURN_ON_FALSE(src_buf != NULL, ESP_ERR_INVALID_ARG, TAG, "The source buffer cannot be NULL");
    ESP_RETURN_ON_FALSE(!dev_handle->device_flags.read_only, ESP_ERR_INVALID_STATE, TAG, "The device is read-only");
    ESP_RETURN_ON_FALSE(dst_addr + data_write_len <= dev_handle->geometry.disk_size, ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");

    esp_blockdev_memory_t *dev = (esp_blockdev_memory_t *)dev_handle;

    size_t offset = (size_t)dst_addr;

    memcpy(dev->buffer + offset, src_buf, data_write_len);

    return ESP_OK;
}

static esp_err_t bd_memory_erase(esp_blockdev_handle_t dev_handle, uint64_t start_addr, size_t erase_len)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");
    ESP_RETURN_ON_FALSE(!dev_handle->device_flags.read_only, ESP_ERR_INVALID_STATE, TAG, "The device is read-only");
    ESP_RETURN_ON_FALSE(start_addr + erase_len <= dev_handle->geometry.disk_size, ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");

    esp_blockdev_memory_t *dev = (esp_blockdev_memory_t *)dev_handle;

    size_t offset = (size_t)start_addr;
    uint8_t erase_value = dev_handle->device_flags.default_val_after_erase ? 0xFF : 0;

    memset(dev->buffer + offset, erase_value, erase_len);

    return ESP_OK;
}

static esp_err_t bd_memory_sync(esp_blockdev_handle_t dev_handle)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");

    return ESP_OK;
}

static esp_err_t bd_memory_ioctl(esp_blockdev_handle_t dev_handle, const uint8_t cmd, void *args)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");
    (void)cmd;
    (void)args;

    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t bd_memory_release(esp_blockdev_handle_t dev_handle)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");

    esp_blockdev_memory_t *dev = (esp_blockdev_memory_t *)dev_handle;

    if (dev->owns_buffer && dev->buffer != NULL) {
        heap_caps_free(dev->buffer);
    }

    free(dev);

    return ESP_OK;
}

static const esp_blockdev_ops_t g_memory_blockdev_ops = {
    .read = bd_memory_read,
    .write = bd_memory_write,
    .erase = bd_memory_erase,
    .sync = bd_memory_sync,
    .ioctl = bd_memory_ioctl,
    .release = bd_memory_release,
};

static esp_err_t esp_blockdev_memory_create_internal(uint8_t *buffer, size_t buffer_size, const esp_blockdev_geometry_t *geometry, bool owns_buffer, bool read_only, esp_blockdev_handle_t *out)
{
    ESP_RETURN_ON_FALSE(out != NULL, ESP_ERR_INVALID_ARG, TAG, "The out pointer cannot be NULL");
    *out = ESP_BLOCKDEV_HANDLE_INVALID;
    ESP_RETURN_ON_FALSE(buffer != NULL, ESP_ERR_INVALID_ARG, TAG, "The buffer cannot be NULL");
    ESP_RETURN_ON_FALSE(geometry != NULL, ESP_ERR_INVALID_ARG, TAG, "Geometry pointer cannot be NULL");
    ESP_RETURN_ON_FALSE(geometry->disk_size > 0, ESP_ERR_INVALID_ARG, TAG, "Disk size must be greater than zero");
    ESP_RETURN_ON_FALSE(geometry->disk_size <= SIZE_MAX, ESP_ERR_INVALID_SIZE, TAG, "Disk size exceeds addressable memory span");
    ESP_RETURN_ON_FALSE(geometry->disk_size <= buffer_size, ESP_ERR_INVALID_SIZE, TAG, "Buffer smaller than requested disk size");

    esp_blockdev_memory_t *dev = calloc(1, sizeof(esp_blockdev_memory_t));
    ESP_RETURN_ON_FALSE(dev != NULL, ESP_ERR_NO_MEM, TAG, "Failed to allocate device structure");

    *dev = (esp_blockdev_memory_t) {
        .dev = {
            .ctx = NULL,
            .device_flags = {
                .read_only = read_only,
                .encrypted = false,
                .erase_before_write = 0,
                .and_type_write = 0,
                .default_val_after_erase = false,
            },
            .geometry = *geometry,
            .ops = &g_memory_blockdev_ops,
        },
        .buffer = buffer,
        .capacity = buffer_size,
        .owns_buffer = owns_buffer,
    };

    *out = (esp_blockdev_handle_t)dev;

    return ESP_OK;
}

esp_err_t esp_blockdev_memory_get_from_buffer(uint8_t *buffer, size_t buffer_size, const esp_blockdev_geometry_t *geometry, bool read_only, esp_blockdev_handle_t *out)
{
    return esp_blockdev_memory_create_internal(buffer, buffer_size, geometry, false, read_only, out);
}

esp_err_t esp_blockdev_memory_create_with_heap_caps(const esp_blockdev_geometry_t *geometry, uint32_t caps, esp_blockdev_handle_t *out)
{
    ESP_RETURN_ON_FALSE(geometry != NULL, ESP_ERR_INVALID_ARG, TAG, "Geometry pointer cannot be NULL");

    size_t alloc_size = (size_t)geometry->disk_size;
    uint8_t *buffer = heap_caps_malloc(alloc_size, caps);
    ESP_RETURN_ON_FALSE(buffer != NULL, ESP_ERR_NO_MEM, TAG, "Failed to allocate device buffer");

    memset(buffer, 0x00, alloc_size);

    esp_err_t err = esp_blockdev_memory_create_internal(buffer, alloc_size, geometry, true, false, out);
    if (err != ESP_OK) {
        heap_caps_free(buffer);
        return err;
    }

    return ESP_OK;
}
