/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "vfs_fat_internal.h"
#include "diskio_impl.h"

#include "diskio_rawflash.h"

#include "wear_levelling.h"
#include "diskio_wl.h"

static const char* TAG = "vfs_fat_spiflash";

esp_err_t esp_vfs_fat_spiflash_mount_rw_wl(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config,
    wl_handle_t* wl_handle)
{
    esp_err_t ret = ESP_OK;
    const size_t workbuf_size = 4096;
    void *workbuf = NULL;

    esp_partition_subtype_t subtype = partition_label ?
            ESP_PARTITION_SUBTYPE_ANY : ESP_PARTITION_SUBTYPE_DATA_FAT;
    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                subtype, partition_label);
    ESP_RETURN_ON_FALSE(data_partition, ESP_ERR_NOT_FOUND, TAG, "Failed to find FATFS partition (type='data', subtype='fat', partition_label='%s'). Check the partition table.", partition_label);

    ESP_RETURN_ON_ERROR(wl_mount(data_partition, wl_handle), TAG, "failed to mount wear levelling layer. ret = %i", ret);

    // connect driver to FATFS
    BYTE pdrv = 0xFF;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK) {
        ESP_LOGD(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "using pdrv=%i", pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};
    ESP_GOTO_ON_ERROR(ff_diskio_register_wl_partition(pdrv, *wl_handle), fail, TAG, "ff_diskio_register_wl_partition failed pdrv=%i, error - 0x(%x)", pdrv, ret);

    FATFS *fs;
    ret = esp_vfs_fat_register(base_path, drv, mount_config->max_files, &fs);
    if (ret == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (ret != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register failed 0x(%x)", ret);
        goto fail;
    }

    // Try to mount partition
    FRESULT fresult = f_mount(fs, drv, 1);
    if (fresult != FR_OK) {
        ESP_LOGW(TAG, "f_mount failed (%d)", fresult);
        if (!((fresult == FR_NO_FILESYSTEM || fresult == FR_INT_ERR)
              && mount_config->format_if_mount_failed)) {
            ret = ESP_FAIL;
            goto fail;
        }
        workbuf = ff_memalloc(workbuf_size);
        if (workbuf == NULL) {
            ret = ESP_ERR_NO_MEM;
            goto fail;
        }
        size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
                CONFIG_WL_SECTOR_SIZE,
                mount_config->allocation_unit_size);
        ESP_LOGI(TAG, "Formatting FATFS partition, allocation unit size=%d", alloc_unit_size);
        const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), 0, 0, 0, alloc_unit_size};
        fresult = f_mkfs(drv, &opt, workbuf, workbuf_size);
        ESP_GOTO_ON_FALSE(fresult == FR_OK, ESP_FAIL, fail, TAG, "f_mkfs failed (%d)", fresult);
        free(workbuf);
        workbuf = NULL;
        ESP_LOGI(TAG, "Mounting again");
        fresult = f_mount(fs, drv, 0);
        ESP_GOTO_ON_FALSE(fresult == FR_OK, ESP_FAIL, fail, TAG, "f_mount failed after formatting (%d)", fresult);
    }
    return ESP_OK;

fail:
    free(workbuf);
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    return ret;
}

esp_err_t esp_vfs_fat_spiflash_unmount_rw_wl(const char* base_path, wl_handle_t wl_handle)
{
    BYTE pdrv = ff_diskio_get_pdrv_wl(wl_handle);
    ESP_RETURN_ON_FALSE(pdrv != 0xff, ESP_ERR_INVALID_STATE, TAG, "partition isn't registered, call esp_vfs_fat_spiflash_mount_rw_wl first");

    char drv[3] = {(char)('0' + pdrv), ':', 0};
    f_mount(0, drv, 0);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_wl(wl_handle);
    // release partition driver
    esp_err_t err_drv = wl_unmount(wl_handle);
    esp_err_t err = esp_vfs_fat_unregister_path(base_path);
    if (err == ESP_OK) err = err_drv;
    return err;
}

// esp_err_t esp_vfs_fat_spiflash_format_rw_wl(const char* base_path,const char* partition_label, wl_handle_t wl_handle)
// {
//     esp_err_t err = ESP_FAIL;
//     BYTE pdrv = ff_diskio_get_pdrv_wl(wl_handle);
//     if (pdrv != 0xff) {
//         ESP_LOGD(TAG, "handle ID: pdrv", pdrv);
//         err = esp_vfs_fat_spiflash_unmount_rw_wl(base_path, wl_handle);
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "unmounting fail");
//             return err;
//         }
//     }

//     const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
//                                                                 ESP_PARTITION_SUBTYPE_DATA_FAT,
//                                                                 partition_label);
//     if (!partition) {
//         ESP_LOGE(TAG, "Failed to find FATFS partition (type='data', subtype='fat', partition_label='%s'). Check the partition table.", partition_label);
//         return ESP_ERR_NOT_FOUND;
//     }
//     err = esp_partition_erase_range(partition, 0, partition->size);
//     if (err != ESP_OK) {

//     }

// }


esp_err_t esp_vfs_fat_spiflash_mount_ro(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config)
{
    esp_err_t ret = ESP_OK;

    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_DATA_FAT, partition_label);
    ESP_RETURN_ON_FALSE(data_partition, ESP_ERR_NOT_FOUND, TAG, "Failed to find FATFS partition (type='data', subtype='fat', partition_label='%s'). Check the partition table.", partition_label);

    // connect driver to FATFS
    BYTE pdrv = 0xFF;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK) {
        ESP_LOGD(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "using pdrv=%i", pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};
    ESP_GOTO_ON_ERROR(ff_diskio_register_raw_partition(pdrv, data_partition), fail, TAG, "ff_diskio_register_raw_partition failed pdrv=%i, error - 0x(%x)", pdrv, ret);

    FATFS *fs;
    ret = esp_vfs_fat_register(base_path, drv, mount_config->max_files, &fs);
    if (ret == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (ret != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register failed 0x(%x)", ret);
        goto fail;
    }

    // Try to mount partition
    FRESULT fresult = f_mount(fs, drv, 1);
    if (fresult != FR_OK) {
        ESP_LOGW(TAG, "f_mount failed (%d)", fresult);
        ret = ESP_FAIL;
        goto fail;
    }
    return ESP_OK;

fail:
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    return ret;
}

esp_err_t esp_vfs_fat_spiflash_unmount_ro(const char* base_path, const char* partition_label)
{
    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_DATA_FAT, partition_label);
    ESP_RETURN_ON_FALSE(data_partition, ESP_ERR_NOT_FOUND, TAG, "Failed to find FATFS partition (type='data', subtype='fat', partition_label='%s'). Check the partition table.", partition_label);

    BYTE pdrv = ff_diskio_get_pdrv_raw(data_partition);
    ESP_RETURN_ON_FALSE(pdrv != 0xff, ESP_ERR_INVALID_STATE, TAG, "partition isn't registered, call esp_vfs_fat_spiflash_mount_ro first");

    char drv[3] = {(char)('0' + pdrv), ':', 0};
    f_mount(0, drv, 0);
    ff_diskio_unregister(pdrv);
    esp_err_t err = esp_vfs_fat_unregister_path(base_path);
    return err;
}


esp_err_t esp_vfs_fat_spiflash_mount(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config,
    wl_handle_t* wl_handle)
    __attribute__((alias("esp_vfs_fat_spiflash_mount_rw_wl")));
esp_err_t esp_vfs_fat_spiflash_unmount(const char* base_path, wl_handle_t wl_handle)
    __attribute__((alias("esp_vfs_fat_spiflash_unmount_rw_wl")));
esp_err_t esp_vfs_fat_rawflash_mount(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config)
    __attribute__((alias("esp_vfs_fat_spiflash_mount_ro")));
esp_err_t esp_vfs_fat_rawflash_unmount(const char* base_path, const char* partition_label)
    __attribute__((alias("esp_vfs_fat_spiflash_unmount_ro")));
