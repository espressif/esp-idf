/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bdl_ramdisk.hpp"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static esp_err_t ramdisk_dev_read(esp_blockdev_handle_t dev_handle, uint8_t* dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len)
{
    if (!dev_handle || !dst_buf) {
        return ESP_ERR_INVALID_ARG;
    }

    bdl_ramdisk_ctx_t* ctx = (bdl_ramdisk_ctx_t*)dev_handle->ctx;

    if (dst_buf_size < data_read_len) {
        return ESP_ERR_INVALID_ARG;
    }

    if (src_addr + data_read_len > ctx->total_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    // Check alignment for read_size
    if (dev_handle->geometry.read_size > 0) {
        if (src_addr % dev_handle->geometry.read_size != 0 ||
            data_read_len % dev_handle->geometry.read_size != 0) {
            return ESP_ERR_INVALID_SIZE;
        }
    }

    memcpy(dst_buf, ctx->data + src_addr, data_read_len);
    return ESP_OK;
}

static esp_err_t ramdisk_dev_write(esp_blockdev_handle_t dev_handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len)
{
    if (!dev_handle || !src_buf) {
        return ESP_ERR_INVALID_ARG;
    }

    if (dev_handle->device_flags.read_only) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    bdl_ramdisk_ctx_t* ctx = (bdl_ramdisk_ctx_t*)dev_handle->ctx;

    if (dst_addr + data_write_len > ctx->total_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    // Check alignment for write_size
    if (dev_handle->geometry.write_size > 0) {
        if (dst_addr % dev_handle->geometry.write_size != 0 ||
            data_write_len % dev_handle->geometry.write_size != 0) {
            return ESP_ERR_INVALID_SIZE;
        }
    }

    memcpy(ctx->data + dst_addr, src_buf, data_write_len);
    return ESP_OK;
}

static esp_err_t ramdisk_dev_erase(esp_blockdev_handle_t dev_handle, uint64_t start_addr, size_t erase_len)
{
    if (!dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    if (dev_handle->device_flags.read_only) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    bdl_ramdisk_ctx_t* ctx = (bdl_ramdisk_ctx_t*)dev_handle->ctx;

    if (start_addr + erase_len > ctx->total_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    // Check alignment for erase_size
    if (dev_handle->geometry.erase_size > 0) {
        if (start_addr % dev_handle->geometry.erase_size != 0 ||
            erase_len % dev_handle->geometry.erase_size != 0) {
            return ESP_ERR_INVALID_SIZE;
        }
    }

    // Fill with default erase value (0xFF for flash-like behavior)
    uint8_t erase_value = dev_handle->device_flags.default_val_after_erase ? 0xFF : 0x00;
    memset(ctx->data + start_addr, erase_value, erase_len);
    return ESP_OK;
}

static esp_err_t ramdisk_dev_sync(esp_blockdev_handle_t dev_handle)
{
    // RAM disk doesn't need sync - all operations are immediate
    (void)dev_handle;
    return ESP_OK;
}

static esp_err_t ramdisk_dev_ioctl(esp_blockdev_handle_t dev_handle, const uint8_t cmd, void* args)
{
    if (dev_handle == nullptr || args == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (cmd) {
        case ESP_BLOCKDEV_CMD_MARK_DELETED:
        case ESP_BLOCKDEV_CMD_ERASE_CONTENTS:
            // For RAM disk, mark as deleted is equivalent to erase
            if (args) {
                esp_blockdev_cmd_arg_erase_t* erase_args = (esp_blockdev_cmd_arg_erase_t*)args;
                return ramdisk_dev_erase(dev_handle, erase_args->start_addr, erase_args->erase_len);
            }
            return ESP_ERR_INVALID_ARG;

        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
}

esp_err_t ramdisk_dev_release(esp_blockdev_handle_t dev_handle)
{
    if (!dev_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    bdl_ramdisk_ctx_t* ctx = (bdl_ramdisk_ctx_t*)dev_handle->ctx;
    if (ctx && ctx->data) {
        free(ctx->data);
    }
    free(ctx);
    free(dev_handle);
    return ESP_OK;
}

// Const operations structure - shared by all instances
static const esp_blockdev_ops_t s_bdl_ramdisk_ops = {
    .read = ramdisk_dev_read,
    .write = ramdisk_dev_write,
    .erase = ramdisk_dev_erase,
    .sync = ramdisk_dev_sync,
    .ioctl = ramdisk_dev_ioctl,
    .release = ramdisk_dev_release
};

esp_err_t ramdisk_dev_get_blockdev(esp_blockdev_handle_t* blockdev_handle_out, const size_t total_size, const size_t erase_size)
{
    if (total_size == 0 || erase_size == 0 || total_size % erase_size != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    // Allocate the block device structure
    esp_blockdev_t* bdl = (esp_blockdev_t*)calloc(1, sizeof(esp_blockdev_t));
    if (!bdl) {
        return ESP_ERR_NO_MEM;
    }

    // Allocate the context
    bdl_ramdisk_ctx_t* ctx = (bdl_ramdisk_ctx_t*)calloc(1, sizeof(bdl_ramdisk_ctx_t));
    if (!ctx) {
        free(bdl);
        return ESP_ERR_NO_MEM;
    }

    // Allocate the RAM storage
    ctx->data = (uint8_t*)malloc(total_size);
    if (!ctx->data) {
        free(ctx);
        free(bdl);
        return ESP_ERR_NO_MEM;
    }

    // Initialize with erased state (0xFF)
    memset(ctx->data, 0xFF, total_size);

    ctx->total_size = total_size;
    ctx->erase_size = erase_size;

    // Setup block device structure
    bdl->ctx = ctx;
    bdl->ops = &s_bdl_ramdisk_ops;

    // Configure device flags for flash-like behavior
    ESP_BLOCKDEV_FLAGS_INST_CONFIG_DEFAULT(bdl->device_flags);

    // Configure geometry
    bdl->geometry.disk_size = total_size;
    bdl->geometry.read_size = 1;                    // Can read single bytes
    bdl->geometry.write_size = 1;                   // Can write single bytes
    bdl->geometry.erase_size = erase_size;          // Erase block size
    bdl->geometry.recommended_read_size = 32;       // Optimal read size
    bdl->geometry.recommended_write_size = 32;      // Optimal write size
    bdl->geometry.recommended_erase_size = erase_size;

    *blockdev_handle_out = bdl;
    return ESP_OK;
}
