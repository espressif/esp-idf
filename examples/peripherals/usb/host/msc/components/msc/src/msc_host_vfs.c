/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "msc_common.h"
#include "msc_host_vfs.h"
#include "diskio_impl.h"
#include "ffconf.h"
#include "ff.h"

#define DRIVE_STR_LEN 3

typedef struct msc_host_vfs {
    char drive[DRIVE_STR_LEN];
    char *base_path;
    uint8_t pdrv;
} msc_host_vfs_t;

static const char *TAG = "MSC VFS";

static esp_err_t msc_format_storage(size_t block_size, size_t allocation_size, const char *drv)
{
    void *workbuf = NULL;
    const size_t workbuf_size = 4096;

    MSC_RETURN_ON_FALSE( workbuf = ff_memalloc(workbuf_size), ESP_ERR_NO_MEM );

    // Valid value of cluster size is between sector_size and 128 * sector_size.
    size_t cluster_size = MIN(MAX(allocation_size, block_size), 128 * block_size);
    const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), 0, 0, 0, cluster_size};
    FRESULT err = f_mkfs(drv, &opt, workbuf, workbuf_size);
    if (err) {
        ESP_LOGE(TAG, "Formatting failed with error: %d", err);
        free(workbuf);
        return ESP_ERR_MSC_FORMAT_FAILED;
    }

    free(workbuf);
    return ESP_OK;
}

static void dealloc_msc_vfs(msc_host_vfs_t *vfs)
{
    free(vfs->base_path);
    free(vfs);
}

esp_err_t msc_host_vfs_register(msc_host_device_handle_t device,
                                const char *base_path,
                                const esp_vfs_fat_mount_config_t *mount_config,
                                msc_host_vfs_handle_t *vfs_handle)
{
    MSC_RETURN_ON_INVALID_ARG(device);
    MSC_RETURN_ON_INVALID_ARG(base_path);
    MSC_RETURN_ON_INVALID_ARG(mount_config);
    MSC_RETURN_ON_INVALID_ARG(vfs_handle);

    FATFS *fs = NULL;
    BYTE pdrv;
    bool diskio_registered = false;
    esp_err_t ret = ESP_ERR_MSC_MOUNT_FAILED;
    msc_device_t *dev = (msc_device_t *)device;
    size_t block_size = dev->disk.block_size;
    size_t alloc_size = mount_config->allocation_unit_size;

    msc_host_vfs_t *vfs = calloc(1, sizeof(msc_host_vfs_t));
    MSC_RETURN_ON_FALSE(vfs != NULL, ESP_ERR_NO_MEM);

    MSC_GOTO_ON_ERROR( ff_diskio_get_drive(&pdrv) );

    ff_diskio_register_msc(pdrv, &dev->disk);
    char drive[DRIVE_STR_LEN] = {(char)('0' + pdrv), ':', 0};
    diskio_registered = true;

    strncpy(vfs->drive, drive, DRIVE_STR_LEN);
    MSC_GOTO_ON_FALSE( vfs->base_path = strdup(base_path), ESP_ERR_NO_MEM );
    vfs->pdrv = pdrv;

    MSC_GOTO_ON_ERROR( esp_vfs_fat_register(base_path, drive, mount_config->max_files, &fs) );

    FRESULT fresult = f_mount(fs, drive, 1);

    if ( fresult != FR_OK) {
        if (mount_config->format_if_mount_failed &&
                (fresult == FR_NO_FILESYSTEM || fresult == FR_INT_ERR)) {
            MSC_GOTO_ON_ERROR( msc_format_storage(block_size, alloc_size, drive) );
            MSC_GOTO_ON_FALSE( f_mount(fs, drive, 0) == FR_OK, ESP_ERR_MSC_MOUNT_FAILED );
        } else {
            goto fail;
        }
    }

    *vfs_handle = vfs;
    return ESP_OK;

fail:
    if (diskio_registered) {
        ff_diskio_unregister(pdrv);
    }
    esp_vfs_fat_unregister_path(base_path);
    if(fs) {
        f_mount(NULL, drive, 0);
    }
    dealloc_msc_vfs(vfs);
    return ret;
}

esp_err_t msc_host_vfs_unregister(msc_host_vfs_handle_t vfs_handle)
{
    MSC_RETURN_ON_INVALID_ARG(vfs_handle);
    msc_host_vfs_t *vfs = (msc_host_vfs_t *)vfs_handle;

    f_mount(NULL, vfs->drive, 0);
    ff_diskio_unregister(vfs->pdrv);
    esp_vfs_fat_unregister_path(vfs->base_path);
    dealloc_msc_vfs(vfs);
    return ESP_OK;
}
