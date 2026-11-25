/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
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
#include "diskio_bdl.h"

static const char *TAG = "vfs_fat_bdl";

/* `work` (one sector at least) is used for formatting if not NULL, otherwise a buffer is allocated */
static esp_err_t try_mount_rw(FATFS *fs, const char *drv,
                               const esp_vfs_fat_mount_config_t *mount_config,
                               vfs_fat_x_ctx_flags_t *out_flags,
                               size_t sec_num, size_t sec_size,
                               void *work, size_t work_size)
{
    FRESULT fresult = f_mount(fs, drv, 1);
    if (fresult == FR_OK) {
        if (out_flags) {
            *out_flags &= ~FORMATTED_DURING_LAST_MOUNT;
        }
        return ESP_OK;
    }

    bool recoverable = (fresult == FR_NO_FILESYSTEM || fresult == FR_INT_ERR);
    if (!recoverable || !mount_config->format_if_mount_failed) {
        ESP_LOGE(TAG, "f_mount failed (%d)", fresult);
        return ESP_FAIL;
    }

    ESP_LOGW(TAG, "f_mount failed (%d), formatting...", fresult);

    size_t workbuf_size = work_size;
    void *workbuf = work;
    if (workbuf == NULL) {
        workbuf_size = 4096;
        workbuf = ff_memalloc(workbuf_size);
        if (workbuf == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
            sec_size, mount_config->allocation_unit_size);
    ESP_LOGI(TAG, "Formatting FATFS partition, allocation unit size=%d", alloc_unit_size);

    UINT root_dir_entries = (sec_size == 512) ? 16 : 128;
    const MKFS_PARM opt = {
        (BYTE)(FM_ANY | FM_SFD),
        (mount_config->use_one_fat ? 1 : 2),
        0,
        (sec_num <= 128 ? root_dir_entries : 0),
        alloc_unit_size
    };
    fresult = f_mkfs(drv, &opt, workbuf, workbuf_size);
    if (work == NULL) {
        ff_memfree(workbuf);
    }
    ESP_RETURN_ON_FALSE(fresult == FR_OK, ESP_FAIL, TAG, "f_mkfs failed (%d)", fresult);

    if (out_flags) {
        *out_flags |= FORMATTED_DURING_LAST_MOUNT;
    }

    ESP_LOGI(TAG, "Mounting again");
    fresult = f_mount(fs, drv, 1);
    ESP_RETURN_ON_FALSE(fresult == FR_OK, ESP_FAIL, TAG, "f_mount failed after formatting (%d)", fresult);

    return ESP_OK;
}

size_t esp_vfs_fat_get_sector_size(esp_blockdev_handle_t bdl_handle)
{
    return ff_diskio_bdl_get_sector_size(bdl_handle);
}

esp_err_t esp_vfs_fat_init(esp_vfs_fat_ctx_t *ctx, const esp_vfs_fat_resources_t *resources,
                           esp_blockdev_handle_t bdl_handle, const esp_vfs_fat_mount_config_t *mount_config)
{
    ESP_RETURN_ON_FALSE(ctx && resources, ESP_ERR_INVALID_ARG, TAG, "ctx or resources is NULL");
    ESP_RETURN_ON_FALSE(resources->files && resources->max_files > 0, ESP_ERR_INVALID_ARG, TAG, "invalid file slots");
    ESP_RETURN_ON_FALSE(bdl_handle != ESP_BLOCKDEV_HANDLE_INVALID, ESP_ERR_INVALID_ARG, TAG, "invalid BDL handle");
    ESP_RETURN_ON_FALSE(mount_config, ESP_ERR_INVALID_ARG, TAG, "mount_config is NULL");
    ESP_RETURN_ON_FALSE(!(mount_config->read_only && mount_config->format_if_mount_failed),
                        ESP_ERR_INVALID_ARG, TAG, "read_only and format_if_mount_failed are mutually exclusive");
    /* Also keeps ff_diskio_clear_pdrv_bdl(), which matches by handle, from touching another instance */
    ESP_RETURN_ON_FALSE(ff_diskio_get_pdrv_bdl(bdl_handle) == 0xFF, ESP_ERR_INVALID_STATE, TAG,
                        "BDL is already used by another FATFS instance");

    const size_t max_files = resources->max_files;
    const size_t sec_size = esp_vfs_fat_get_sector_size(bdl_handle);
    ESP_RETURN_ON_FALSE(sec_size != 0, ESP_ERR_INVALID_ARG, TAG, "BDL geometry is incompatible with FATFS");

    const bool need_sectors = ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sec_size) > 0;
    const bool need_lfn = ESP_VFS_FAT_LFN_BUFFER_SIZE > 0;
    const bool has_sectors = resources->sector_buffers != NULL;
    const bool has_lfn = resources->lfn_buffer != NULL;
    /* Caller supplies every buffer this configuration uses, or none; the wrapper then allocates them all */
    ESP_RETURN_ON_FALSE(!(need_sectors && need_lfn && has_sectors != has_lfn), ESP_ERR_INVALID_ARG, TAG,
                        "pass sector_buffers and lfn_buffer together, or leave both NULL");

    uint8_t *sector_buffers = NULL;
    const size_t sector_buffers_needed = ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sec_size);
    if (has_sectors && need_sectors) {
        ESP_RETURN_ON_FALSE(resources->sector_buffers_size >= sector_buffers_needed, ESP_ERR_INVALID_SIZE, TAG,
                            "sector_buffers needs %u bytes", (unsigned)sector_buffers_needed);
        sector_buffers = resources->sector_buffers;
    }
    void *lfn_buffer = NULL;
#if FF_USE_LFN == 3
    if (has_lfn) {
        ESP_RETURN_ON_FALSE(resources->lfn_buffer_size >= ESP_VFS_FAT_LFN_BUFFER_SIZE, ESP_ERR_INVALID_SIZE, TAG,
                            "lfn_buffer needs %u bytes", (unsigned)ESP_VFS_FAT_LFN_BUFFER_SIZE);
        ESP_RETURN_ON_FALSE(((uintptr_t)resources->lfn_buffer & 1) == 0, ESP_ERR_INVALID_ARG, TAG,
                            "lfn_buffer must be 2-byte aligned");
        lfn_buffer = resources->lfn_buffer;
    }
#endif

    BYTE pdrv = 0xFF;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK) {
        ESP_LOGD(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "using pdrv=%i", pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};

    vfs_fat_ctx_t *fat_ctx = &ctx->priv;
    vfs_fat_ctx_setup(fat_ctx, resources->files, max_files, drv);
    esp_err_t ret = vfs_fat_attach_lfn_buffer(fat_ctx, lfn_buffer);
    if (ret != ESP_OK) {
        vfs_fat_ctx_teardown(fat_ctx);
        return ret;
    }
    if (sector_buffers) {
        vfs_fat_ctx_attach_buffers(fat_ctx, sector_buffers, sec_size);
    } else {
        ret = vfs_fat_alloc_sector_buffers(fat_ctx, sec_size);
        if (ret != ESP_OK) {
            vfs_fat_ctx_teardown(fat_ctx);
            return ret;
        }
    }
    fat_ctx->pdrv = pdrv;
    fat_ctx->bdl = bdl_handle;
    fat_ctx->mount_config = *mount_config;

    ret = ff_diskio_register_bdl(pdrv, bdl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ff_diskio_register_bdl failed pdrv=%i, error - 0x(%x)", pdrv, ret);
        vfs_fat_ctx_teardown(fat_ctx);
        return ret;
    }

    WORD sec_size_w;
    if (disk_ioctl(pdrv, GET_SECTOR_SIZE, &sec_size_w) != RES_OK || sec_size_w != sec_size) {
        ESP_LOGE(TAG, "failed to query sector size from diskio");
        ret = ESP_FAIL;
        goto fail;
    }
    size_t sec_num = (size_t)(bdl_handle->geometry.disk_size / sec_size);

    /* The volume window is idle while formatting, so it doubles as the f_mkfs work area */
    void *mkfs_work = NULL;
    size_t mkfs_work_size = 0;
#if FF_USE_DYN_BUFFER
    mkfs_work = fat_ctx->fs.win;
    mkfs_work_size = sec_size;
#else
    mkfs_work = fat_ctx->fs.win;
    mkfs_work_size = sizeof(fat_ctx->fs.win);
#endif

    if (bdl_handle->device_flags.read_only) {
        FRESULT fresult = f_mount(&fat_ctx->fs, drv, 1);
        if (fresult != FR_OK) {
            ESP_LOGW(TAG, "f_mount failed (%d)", fresult);
            ret = ESP_FAIL;
            goto fail;
        }
    } else {
        ret = try_mount_rw(&fat_ctx->fs, drv, mount_config, &fat_ctx->state, sec_num, sec_size,
                           mkfs_work, mkfs_work_size);
        if (ret != ESP_OK) {
            goto fail;
        }
    }

    if (bdl_handle->device_flags.read_only || mount_config->read_only) {
        fat_ctx->vfs_flags |= ESP_VFS_FLAG_READONLY_FS;
    }
    return ESP_OK;

fail:
    /* Clears FatFs[vol] so the volume does not keep pointing into this context */
    f_mount(NULL, drv, 0);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_bdl(bdl_handle);
    vfs_fat_ctx_teardown(fat_ctx);
    return ret;
}

esp_err_t esp_vfs_fat_deinit(esp_vfs_fat_ctx_t *ctx)
{
    ESP_RETURN_ON_FALSE(ctx, ESP_ERR_INVALID_ARG, TAG, "ctx is NULL");
    vfs_fat_ctx_t *fat_ctx = &ctx->priv;
    ESP_RETURN_ON_FALSE(fat_ctx->base_path[0] == '\0', ESP_ERR_INVALID_STATE, TAG,
                        "instance is still registered at %s", fat_ctx->base_path);

    char drv[3] = {(char)('0' + fat_ctx->pdrv), ':', 0};
    f_mount(NULL, drv, 0);
    ff_diskio_unregister(fat_ctx->pdrv);
    ff_diskio_clear_pdrv_bdl(fat_ctx->bdl);
    vfs_fat_ctx_teardown(fat_ctx);
    return ESP_OK;
}

esp_err_t esp_vfs_fat_create(esp_blockdev_handle_t bdl_handle, const esp_vfs_fat_mount_config_t *mount_config,
                             esp_vfs_fat_ctx_t **out_ctx)
{
    ESP_RETURN_ON_FALSE(mount_config && out_ctx, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    *out_ctx = NULL;

    size_t max_files = mount_config->max_files > 0 ? (size_t)mount_config->max_files : 1;
    esp_vfs_fat_file_t *files = NULL;
    esp_vfs_fat_ctx_t *ctx = vfs_fat_alloc(max_files, &files);
    ESP_RETURN_ON_FALSE(ctx, ESP_ERR_NO_MEM, TAG, "no mem");

    const esp_vfs_fat_resources_t resources = {
        .files = files,
        .max_files = max_files,
    };
    esp_err_t ret = esp_vfs_fat_init(ctx, &resources, bdl_handle, mount_config);
    if (ret != ESP_OK) {
        vfs_fat_free(ctx);
        return ret;
    }
    *out_ctx = ctx;
    return ESP_OK;
}

esp_err_t esp_vfs_fat_destroy(esp_vfs_fat_ctx_t *ctx)
{
    ESP_RETURN_ON_ERROR(esp_vfs_fat_deinit(ctx), TAG, "deinit failed");
    vfs_fat_free(ctx);
    return ESP_OK;
}

esp_err_t esp_vfs_fat_mount(const char *base_path, esp_blockdev_handle_t bdl_handle,
                            const esp_vfs_fat_mount_config_t *mount_config)
{
    ESP_RETURN_ON_FALSE(base_path, ESP_ERR_INVALID_ARG, TAG, "base_path is NULL");

    esp_vfs_fat_ctx_t *ctx = NULL;
    ESP_RETURN_ON_ERROR(esp_vfs_fat_create(bdl_handle, mount_config, &ctx), TAG, "failed to create FATFS instance");

    esp_err_t ret = vfs_fat_register_path(&ctx->priv, base_path);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to register %s (0x%x)", base_path, ret);
        esp_vfs_fat_destroy(ctx);
        return ret;
    }
    return ESP_OK;
}

esp_err_t esp_vfs_fat_unmount(const char *base_path)
{
    vfs_fat_ctx_t *fat_ctx = base_path ? vfs_fat_find_by_path(base_path) : NULL;
    ESP_RETURN_ON_FALSE(fat_ctx && !fat_ctx->legacy, ESP_ERR_INVALID_STATE, TAG,
                        "no instance mounted at this path with esp_vfs_fat_mount");

    esp_err_t err = vfs_fat_unregister_path(fat_ctx);
    esp_err_t err_destroy = esp_vfs_fat_destroy(vfs_fat_outer(fat_ctx));
    return err != ESP_OK ? err : err_destroy;
}

esp_err_t esp_vfs_fat_bdl_mount(const char *base_path,
                                esp_blockdev_handle_t bdl_handle,
                                const esp_vfs_fat_mount_config_t *mount_config)
{
    return esp_vfs_fat_mount(base_path, bdl_handle, mount_config);
}

esp_err_t esp_vfs_fat_bdl_unmount(const char *base_path, esp_blockdev_handle_t bdl_handle)
{
    vfs_fat_ctx_t *fat_ctx = base_path ? vfs_fat_find_by_path(base_path) : NULL;
    ESP_RETURN_ON_FALSE(fat_ctx && !fat_ctx->legacy && fat_ctx->bdl == bdl_handle, ESP_ERR_INVALID_STATE, TAG,
                        "BDL device isn't mounted at this path, call esp_vfs_fat_bdl_mount first");
    return esp_vfs_fat_unmount(base_path);
}
