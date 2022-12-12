/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

// DESCRIPTION:
// This file contains the code for accessing the storage medium ie SPI Flash.

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "diskio_wl.h"
#include "wear_levelling.h"
#include "esp_partition.h"

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
static bool s_fat_mounted;
static const char *s_base_path;

static const char *TAG = "example_msc_storage";

esp_err_t storage_init(void)
{
    ESP_LOGI(TAG, "Initializing wear levelling");
    esp_err_t err;

    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    if (data_partition == NULL) {
        ESP_LOGE(TAG, "Failed to find FATFS partition. Check the partition table.");
        return ESP_ERR_NOT_FOUND;
    }

    err = wl_mount(data_partition, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to mount wear levelling layer (0x%x)", err);
        return err;
    }

    return ESP_OK;
}

static inline size_t esp_vfs_fat_get_allocation_unit_size(
    size_t sector_size, size_t requested_size)
{
    size_t alloc_unit_size = requested_size;
    const size_t max_sectors_per_cylinder = 128;
    const size_t max_size = sector_size * max_sectors_per_cylinder;
    alloc_unit_size = MAX(alloc_unit_size, sector_size);
    alloc_unit_size = MIN(alloc_unit_size, max_size);
    return alloc_unit_size;
}

esp_err_t storage_mount(const char *base_path)
{
    const size_t workbuf_size = 4096;
    void *workbuf = NULL;
    esp_err_t err;

    if (s_fat_mounted) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing FAT");

    // connect driver to FATFS
    BYTE pdrv = 0xFF;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK) {
        ESP_LOGE(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "using pdrv=%i", pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};

    err = ff_diskio_register_wl_partition(pdrv, s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ff_diskio_register_wl_partition failed pdrv=%d (0x%x)", pdrv, err);
        goto fail;
    }
    FATFS *fs;
    err = esp_vfs_fat_register(base_path, drv, 2, &fs);
    if (err == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_vfs_fat_register failed (0x%x)", err);
        goto fail;
    }

    // Try to mount partition
    FRESULT fresult = f_mount(fs, drv, 1);
    if (fresult != FR_OK) {
        ESP_LOGW(TAG, "f_mount failed (%d)", fresult);
        if (!((fresult == FR_NO_FILESYSTEM || fresult == FR_INT_ERR))) {
            err = ESP_FAIL;
            goto fail;
        }
        workbuf = ff_memalloc(workbuf_size);
        if (workbuf == NULL) {
            err = ESP_ERR_NO_MEM;
            goto fail;
        }
        size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
                                     CONFIG_WL_SECTOR_SIZE,
                                     4096);

        ESP_LOGI(TAG, "Formatting FATFS partition, allocation unit size=%d", alloc_unit_size);
        const MKFS_PARM opt = {(BYTE)FM_FAT, 0, 0, 0, 0};
        fresult = f_mkfs("", &opt, workbuf, workbuf_size); // Use default volume
        if (fresult != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGE(TAG, "f_mkfs failed (%d)", fresult);
            goto fail;
        }
        free(workbuf);
        workbuf = NULL;
        ESP_LOGI(TAG, "Mounting again");
        fresult = f_mount(fs, drv, 0);
        if (fresult != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGE(TAG, "f_mount failed after formatting (%d)", fresult);
            goto fail;
        }
    }
    s_fat_mounted = true;
    s_base_path = base_path;

    return ESP_OK;

fail:
    free(workbuf);
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    s_fat_mounted = false;
    ESP_LOGW(TAG, "Failed to mount storage (0x%x)", err);
    return err;
}

esp_err_t storage_unmount(void)
{
    if (!s_fat_mounted) {
        return ESP_OK;
    }

    BYTE pdrv = ff_diskio_get_pdrv_wl(s_wl_handle);
    if (pdrv == 0xff) {
        return ESP_ERR_INVALID_STATE;
    }
    char drv[3] = {(char)('0' + pdrv), ':', 0};

    f_mount(0, drv, 0);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_wl(s_wl_handle);
    esp_err_t err = esp_vfs_fat_unregister_path(s_base_path);
    s_base_path = NULL;
    s_fat_mounted = false;

    return err;

}

size_t storage_get_size(void)
{
    assert(s_wl_handle != WL_INVALID_HANDLE);

    return wl_size(s_wl_handle);
}

size_t storage_get_sector_size(void)
{
    assert(s_wl_handle != WL_INVALID_HANDLE);

    return wl_sector_size(s_wl_handle);
}

esp_err_t storage_read_sector(size_t addr, size_t size, void *dest)
{
    assert(s_wl_handle != WL_INVALID_HANDLE);

    return wl_read(s_wl_handle, addr, dest, size);
}

esp_err_t storage_write_sector(size_t addr, size_t size, const void *src)
{
    assert(s_wl_handle != WL_INVALID_HANDLE);

    if (s_fat_mounted) {
        ESP_LOGE(TAG, "can't write, FAT mounted");
        return ESP_ERR_INVALID_STATE;
    }
    size_t sector_size = wl_sector_size(s_wl_handle);
    if (addr % sector_size != 0 || size % sector_size != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = wl_erase_range(s_wl_handle, addr, size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wl_erase_range failed (0x%x)", err);
        return err;
    }
    return wl_write(s_wl_handle, addr, src, size);
}
