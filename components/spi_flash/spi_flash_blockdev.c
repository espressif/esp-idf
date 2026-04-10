/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_blockdev.h"

#include "esp_flash.h"
#include "esp_flash_chips/spi_flash_chip_driver.h"

static inline esp_err_t esp_flash_blockdev_read(esp_blockdev_handle_t handle, uint8_t* dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len)
{
    if (handle == NULL || dst_buf_size < data_read_len) {
        return ESP_ERR_INVALID_ARG;
    }
    if(handle->device_flags.encrypted) {
        return esp_flash_read_encrypted((esp_flash_t*) handle->ctx, (uint32_t) src_addr, dst_buf, (uint32_t) data_read_len);
    }
    return esp_flash_read((esp_flash_t*) handle->ctx, dst_buf, (uint32_t) src_addr, (uint32_t) data_read_len);
}

static inline esp_err_t esp_flash_blockdev_write(esp_blockdev_handle_t handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->device_flags.read_only) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if(handle->device_flags.encrypted) {
        return esp_flash_write_encrypted((esp_flash_t*) handle->ctx, (uint32_t) dst_addr, src_buf, (uint32_t) data_write_len);
    }
    return esp_flash_write((esp_flash_t*) handle->ctx, src_buf, (uint32_t) dst_addr, (uint32_t) data_write_len);
}

static inline esp_err_t esp_flash_blockdev_erase(esp_blockdev_handle_t handle, uint64_t start_addr, size_t erase_len)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->device_flags.read_only) {
        return ESP_ERR_NOT_ALLOWED;
    }
    return esp_flash_erase_region((esp_flash_t*) handle->ctx, (uint32_t) start_addr, (uint32_t) erase_len);
}

static esp_err_t esp_flash_blockdev_ioctl(esp_blockdev_handle_t handle, const uint8_t cmd, void* args)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    (void) cmd;
    (void) args;

    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t esp_flash_blockdev_sync_noop(esp_blockdev_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK; // NOOP, write operations are always synchronous
}

static esp_err_t esp_flash_release_blockdev(esp_blockdev_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    free(handle);
    return ESP_OK;
}

static const esp_blockdev_ops_t esp_flash_blockdev_ops = {
    .read = esp_flash_blockdev_read,
    .write = esp_flash_blockdev_write,
    .erase = esp_flash_blockdev_erase,
    .ioctl = esp_flash_blockdev_ioctl,
    .sync = esp_flash_blockdev_sync_noop,
    .release = esp_flash_release_blockdev,
};

esp_err_t esp_flash_get_blockdev(esp_flash_t* chip, esp_blockdev_handle_t* out_handle)
{
    if (chip == NULL || out_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *out_handle = ESP_BLOCKDEV_HANDLE_INVALID;

    esp_err_t err;
    uint32_t size;
    err = esp_flash_get_size(chip, &size);
    if (err != ESP_OK) {
        return err;
    }

    esp_blockdev_handle_t out = (esp_blockdev_handle_t) heap_caps_calloc(1, sizeof(esp_blockdev_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    if (out == NULL) {
        return ESP_ERR_NO_MEM;
    }
    out->ctx = (void*) chip;

    out->device_flags.read_only = 0;
    out->device_flags.erase_before_write = 1;
    out->device_flags.and_type_write = 1;
    out->device_flags.default_val_after_erase = 1;

    out->geometry.disk_size = (uint64_t) size;
    out->geometry.read_size = 1;
    out->geometry.write_size = 1;
    out->geometry.erase_size = (size_t) chip->chip_drv->sector_size;
    out->geometry.recommended_erase_size = (size_t) chip->chip_drv->block_erase_size;

    out->ops = &esp_flash_blockdev_ops;

    *out_handle = out;
    return ESP_OK;
}
