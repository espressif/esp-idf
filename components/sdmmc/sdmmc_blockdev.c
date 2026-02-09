/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "esp_private/sdmmc_common.h"
#include "esp_blockdev.h"
#include "sdmmc_cmd.h"

static esp_err_t calculate_start_sector_num_and_sector_count(size_t sector_size, uint64_t addr, size_t data_len, size_t* out_start_sector_num, size_t* out_num_of_sectors)
{
    size_t offset_in_start_sector = (size_t) addr % sector_size;
    size_t offset_in_end_sector = (size_t) data_len % sector_size;

    // Has to be aligned to sector boundaries
    if (offset_in_start_sector != 0 || offset_in_end_sector != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t start_sector_num = (size_t) addr / sector_size;
    size_t last_byte_addr = (size_t) (addr + data_len - 1); // Address of the last accessed byte
    size_t end_sector_num = last_byte_addr / sector_size;

    if (out_start_sector_num) {
        *out_start_sector_num = start_sector_num;
    }

    if (out_num_of_sectors) {
        *out_num_of_sectors = end_sector_num - start_sector_num + 1;
    }

    return ESP_OK;
}

static esp_err_t sdmmc_blockdev_read(esp_blockdev_handle_t handle, uint8_t* dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len)
{
    if (handle == NULL || dst_buf == NULL || dst_buf_size < data_read_len) {
        return ESP_ERR_INVALID_ARG;
    }
    sdmmc_card_t* card = (sdmmc_card_t*) handle->ctx;
    size_t start_sector_num, num_of_sectors;
    esp_err_t err = calculate_start_sector_num_and_sector_count((size_t) card->csd.sector_size, src_addr, data_read_len, &start_sector_num, &num_of_sectors);
    if (err != ESP_OK) {
        return err;
    }

    return sdmmc_read_sectors(card, (void*) dst_buf, start_sector_num, num_of_sectors);
}

static esp_err_t sdmmc_blockdev_write(esp_blockdev_handle_t handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len)
{
    if (handle == NULL || src_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    sdmmc_card_t* card = (sdmmc_card_t*) handle->ctx;
    size_t start_sector_num, num_of_sectors;
    esp_err_t err = calculate_start_sector_num_and_sector_count((size_t) card->csd.sector_size, dst_addr, data_write_len, &start_sector_num, &num_of_sectors);
    if (err != ESP_OK) {
        return err;
    }

    return sdmmc_write_sectors(card, (const void*) src_buf, start_sector_num, num_of_sectors);
}

static esp_err_t sdmmc_blockdev_erase(esp_blockdev_handle_t handle, uint64_t start_addr, size_t erase_len)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    sdmmc_card_t* card = (sdmmc_card_t*) handle->ctx;
    size_t start_sector_num, num_of_sectors;
    esp_err_t err = calculate_start_sector_num_and_sector_count((size_t) card->csd.sector_size, start_addr, erase_len, &start_sector_num, &num_of_sectors);
    if (err != ESP_OK) {
        return err;
    }

    sdmmc_erase_arg_t arg = sdmmc_can_discard(card) == ESP_OK ? SDMMC_DISCARD_ARG : SDMMC_ERASE_ARG;
    return sdmmc_erase_sectors(card, start_sector_num, num_of_sectors, arg);
}

static esp_err_t sdmmc_blockdev_ioctl(esp_blockdev_handle_t handle, const uint8_t cmd, void* args)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    (void) cmd;
    (void) args;

    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t sdmmc_blockdev_sync_noop(esp_blockdev_handle_t handle)
{
    (void) handle;
    return ESP_OK; // NOOP, write operations are always synchronous
}


static esp_err_t sdmmc_release_blockdev(esp_blockdev_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    free(handle);
    return ESP_OK;
}

static const esp_blockdev_ops_t sdmmc_blockdev_ops = {
    .read = sdmmc_blockdev_read,
    .write = sdmmc_blockdev_write,
    .erase = sdmmc_blockdev_erase,
    .ioctl = sdmmc_blockdev_ioctl,
    .sync = sdmmc_blockdev_sync_noop,
    .release = sdmmc_release_blockdev,
};

esp_err_t sdmmc_get_blockdev(sdmmc_card_t* card, esp_blockdev_handle_t* out_handle)
{
    if (card == NULL || out_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_blockdev_handle_t out = (esp_blockdev_handle_t) calloc(1, sizeof(esp_blockdev_t));
    if (out == NULL) {
        return ESP_ERR_NO_MEM;
    }
    out->ctx = (void*) card;

    out->device_flags.default_val_after_erase = card->scr.erase_mem_state;

    out->geometry.disk_size = ((uint64_t) card->csd.capacity) * ((uint64_t) card->csd.sector_size);
    out->geometry.read_size = (size_t) card->csd.sector_size;
    out->geometry.write_size = (size_t) card->csd.sector_size;
    out->geometry.erase_size = (size_t) card->csd.sector_size;

    out->ops = &sdmmc_blockdev_ops;

    *out_handle = out;
    return ESP_OK;
}
