/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "esp_blockdev.h"
#include "esp_err.h"
#include "esp_check.h"

#include "esp_blockdev/generic_partition.h"

static const char *TAG = "esp_blockdev/generic_partition";

typedef struct {
    esp_blockdev_t dev;
    esp_blockdev_handle_t parent;
    size_t start_offset;
} esp_blockdev_generic_partition_t;

ssize_t esp_blockdev_generic_partition_translate_address_to_parent(esp_blockdev_handle_t dev_handle, size_t address)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, -1, TAG, "The dev_handle cannot be NULL");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;
    esp_blockdev_handle_t parent = dev->parent;

    ESP_RETURN_ON_FALSE(address <= dev->dev.geometry.disk_size, -1, TAG, "The address falls outside of the partition");

    uint64_t translated = (uint64_t)dev->start_offset + (uint64_t)address;
    ESP_RETURN_ON_FALSE(translated >= (uint64_t)address, -1, TAG, "Address translation overflowed");
    ESP_RETURN_ON_FALSE(translated <= parent->geometry.disk_size, -1, TAG, "The address range falls outside of the parent device");

    return (ssize_t)translated;
}

ssize_t esp_blockdev_generic_partition_translate_address_to_child(esp_blockdev_handle_t dev_handle, size_t address)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, -1, TAG, "The dev_handle cannot be NULL");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;

    uint64_t start_offset = dev->start_offset;
    ESP_RETURN_ON_FALSE((uint64_t)address >= start_offset, -1, TAG, "The parent address is below the partition base");

    uint64_t translated = (uint64_t)address - start_offset;
    ESP_RETURN_ON_FALSE(translated <= dev->dev.geometry.disk_size, -1, TAG, "The address falls outside of the partition");

    return (ssize_t)translated;
}

static esp_err_t bd_gp_read(esp_blockdev_handle_t dev_handle, uint8_t *dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");
    ESP_RETURN_ON_FALSE(dst_buf != NULL, ESP_ERR_INVALID_ARG, TAG, "The destination buffer cannot be NULL");
    ESP_RETURN_ON_FALSE(data_read_len <= dst_buf_size, ESP_ERR_INVALID_SIZE, TAG, "Destination buffer too small");
    ESP_RETURN_ON_FALSE(src_addr + data_read_len <= dev_handle->geometry.disk_size, ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;
    esp_blockdev_handle_t parent = dev->parent;
    assert(dev_handle->geometry.read_size > 0);
    assert(src_addr % dev_handle->geometry.read_size == 0);
    assert(data_read_len % dev_handle->geometry.read_size == 0);

    ssize_t addr_parent = esp_blockdev_generic_partition_translate_address_to_parent(dev_handle, src_addr);
    ESP_RETURN_ON_FALSE(addr_parent >= 0, ESP_ERR_INVALID_ARG, TAG, "Failed to translate address");

    return parent->ops->read(parent, dst_buf, dst_buf_size, (uint64_t)addr_parent, data_read_len);
}

static esp_err_t bd_gp_write(esp_blockdev_handle_t dev_handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");
    ESP_RETURN_ON_FALSE(src_buf != NULL, ESP_ERR_INVALID_ARG, TAG, "The source buffer cannot be NULL");
    ESP_RETURN_ON_FALSE(dst_addr + data_write_len <= dev_handle->geometry.disk_size, ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;
    esp_blockdev_handle_t parent = dev->parent;
    /*
     * These asserts are intentional: the contract makes violating these preconditions undefined,
     * so returning dedicated error codes here is not strictly necessary.
     * The underlying parent blockdev will most likely perform the same checks itself.
     */
    assert(!dev_handle->device_flags.read_only);
    assert(dev_handle->geometry.write_size > 0);
    assert(dst_addr % dev_handle->geometry.write_size == 0);
    assert(data_write_len % dev_handle->geometry.write_size == 0);

    ssize_t addr_parent = esp_blockdev_generic_partition_translate_address_to_parent(dev_handle, dst_addr);
    ESP_RETURN_ON_FALSE(addr_parent >= 0, ESP_ERR_INVALID_ARG, TAG, "Failed to translate address");

    return parent->ops->write(parent, src_buf, (uint64_t)addr_parent, data_write_len);
}

static esp_err_t bd_gp_erase(esp_blockdev_handle_t dev_handle, uint64_t start_addr, size_t erase_len)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;
    esp_blockdev_handle_t parent = dev->parent;
    assert(!dev_handle->device_flags.read_only);
    assert(dev_handle->geometry.erase_size > 0);
    assert(start_addr % dev_handle->geometry.erase_size == 0);
    assert(erase_len % dev_handle->geometry.erase_size == 0);

    if (start_addr + erase_len > dev->dev.geometry.disk_size) {
        return ESP_ERR_INVALID_ARG;
    }

    ssize_t addr_parent = esp_blockdev_generic_partition_translate_address_to_parent(dev_handle, start_addr);
    ESP_RETURN_ON_FALSE(addr_parent >= 0, ESP_ERR_INVALID_ARG, TAG, "Failed to translate address");

    return parent->ops->erase(parent, (uint64_t)addr_parent, erase_len);
}

static esp_err_t bd_gp_sync(esp_blockdev_handle_t dev_handle)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;
    esp_blockdev_handle_t parent = dev->parent;

    if (parent->ops->sync == NULL) {
        return ESP_OK;
    }

    return parent->ops->sync(parent);
}

static esp_err_t bd_gp_ioctl(esp_blockdev_handle_t dev_handle, const uint8_t cmd, void *args)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");

    esp_blockdev_generic_partition_t *dev = (esp_blockdev_generic_partition_t *)dev_handle;
    esp_blockdev_handle_t parent = dev->parent;

    if (cmd == ESP_BLOCKDEV_CMD_MARK_DELETED || cmd == ESP_BLOCKDEV_CMD_ERASE_CONTENTS) {
        ESP_RETURN_ON_FALSE(args != NULL, ESP_ERR_INVALID_ARG, TAG, "The ioctl arguments cannot be NULL");

        esp_blockdev_cmd_arg_erase_t *erase_args = (esp_blockdev_cmd_arg_erase_t *)args;
        ESP_RETURN_ON_FALSE(erase_args->start_addr <= dev->dev.geometry.disk_size,
                            ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");
        ESP_RETURN_ON_FALSE(erase_args->erase_len <= dev->dev.geometry.disk_size - erase_args->start_addr,
                            ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");
        assert(dev->dev.geometry.erase_size > 0);
        assert(erase_args->start_addr % dev->dev.geometry.erase_size == 0);
        assert(erase_args->erase_len % dev->dev.geometry.erase_size == 0);

        ssize_t addr_parent = esp_blockdev_generic_partition_translate_address_to_parent(dev_handle, erase_args->start_addr);
        ESP_RETURN_ON_FALSE(addr_parent >= 0, ESP_ERR_INVALID_ARG, TAG, "Failed to translate address");

        esp_blockdev_cmd_arg_erase_t translated_args = *erase_args;
        translated_args.start_addr = (uint64_t)addr_parent;

        ESP_RETURN_ON_FALSE(parent->ops->ioctl != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "Parent device does not implement ioctl");
        return parent->ops->ioctl(parent, cmd, &translated_args);
    }

    ESP_RETURN_ON_FALSE(parent->ops->ioctl != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "Parent device does not implement ioctl");
    return parent->ops->ioctl(parent, cmd, args);
}

static esp_err_t bd_gp_release(esp_blockdev_handle_t dev_handle)
{
    ESP_RETURN_ON_FALSE(dev_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "The dev_handle cannot be NULL");

    free(dev_handle);

    return ESP_OK;
}

static const esp_blockdev_ops_t g_generic_partition_ops = {
    .read = bd_gp_read,
    .write = bd_gp_write,
    .erase = bd_gp_erase,
    .sync = bd_gp_sync,
    .ioctl = bd_gp_ioctl,
    .release = bd_gp_release,
};

esp_err_t esp_blockdev_generic_partition_get(esp_blockdev_handle_t parent, size_t start_offset, size_t size, esp_blockdev_handle_t *out)
{
    ESP_RETURN_ON_FALSE(parent != NULL, ESP_ERR_INVALID_ARG, TAG, "The parent device handle cannot be NULL");
    ESP_RETURN_ON_FALSE(out != NULL, ESP_ERR_INVALID_ARG, TAG, "The out pointer cannot be NULL");
    *out = ESP_BLOCKDEV_HANDLE_INVALID;

    uint64_t partition_end = (uint64_t)start_offset + (uint64_t)size;
    ESP_RETURN_ON_FALSE(partition_end >= (uint64_t)start_offset, ESP_ERR_INVALID_ARG, TAG, "The requested range overflows");
    ESP_RETURN_ON_FALSE(partition_end <= parent->geometry.disk_size, ESP_ERR_INVALID_ARG, TAG, "The address range falls outside of the disk");

    esp_blockdev_generic_partition_t *part = calloc(1, sizeof(esp_blockdev_generic_partition_t));

    if (part == NULL) {
        return ESP_ERR_NO_MEM;
    }

    *part = (esp_blockdev_generic_partition_t) {
        .dev = {
            .device_flags = parent->device_flags,
            .geometry = {
                .disk_size = size,
                .read_size = parent->geometry.read_size,
                .write_size = parent->geometry.write_size,
                .erase_size = parent->geometry.erase_size,
                .recommended_write_size = parent->geometry.recommended_write_size,
                .recommended_read_size = parent->geometry.recommended_read_size,
                .recommended_erase_size = parent->geometry.recommended_erase_size,
            },
            .ops = &g_generic_partition_ops,
        },
        .parent = parent,
        .start_offset = start_offset,
    };

    part->dev.ctx = part;

    *out = (esp_blockdev_handle_t) part;

    return ESP_OK;
}
