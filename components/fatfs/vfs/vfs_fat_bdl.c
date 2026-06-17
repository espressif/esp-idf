/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "vfs_fat_internal.h"
#include "diskio_impl.h"
#include "diskio_bdl.h"

static const char *TAG = "vfs_fat_bdl";

static vfs_fat_bdl_ctx_t *s_bdl_ctx[FF_VOLUMES] = {};

extern esp_err_t esp_vfs_set_readonly_flag(const char *base_path);

static bool get_ctx_id_by_bdl(esp_blockdev_handle_t bdl, uint32_t *out_id)
{
    for (int i = 0; i < FF_VOLUMES; i++) {
        if (s_bdl_ctx[i] && s_bdl_ctx[i]->bdl_handle == bdl) {
            *out_id = i;
            return true;
        }
    }
    return false;
}

static uint32_t get_unused_ctx_id(void)
{
    for (uint32_t i = 0; i < FF_VOLUMES; i++) {
        if (!s_bdl_ctx[i]) {
            return i;
        }
    }
    return FF_VOLUMES;
}

static esp_err_t try_mount_rw(FATFS *fs, const char *drv,
                               const esp_vfs_fat_mount_config_t *mount_config,
                               vfs_fat_x_ctx_flags_t *out_flags,
                               size_t sec_num, size_t sec_size)
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

    const size_t workbuf_size = 4096;
    void *workbuf = ff_memalloc(workbuf_size);
    if (workbuf == NULL) {
        return ESP_ERR_NO_MEM;
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
    free(workbuf);
    ESP_RETURN_ON_FALSE(fresult == FR_OK, ESP_FAIL, TAG, "f_mkfs failed (%d)", fresult);

    if (out_flags) {
        *out_flags |= FORMATTED_DURING_LAST_MOUNT;
    }

    ESP_LOGI(TAG, "Mounting again");
    fresult = f_mount(fs, drv, 1);
    ESP_RETURN_ON_FALSE(fresult == FR_OK, ESP_FAIL, TAG, "f_mount failed after formatting (%d)", fresult);

    return ESP_OK;
}

esp_err_t esp_vfs_fat_bdl_mount(const char *base_path,
                                esp_blockdev_handle_t bdl_handle,
                                const esp_vfs_fat_mount_config_t *mount_config)
{
    esp_err_t ret = ESP_OK;
    vfs_fat_bdl_ctx_t *ctx = NULL;

    ESP_RETURN_ON_FALSE(base_path, ESP_ERR_INVALID_ARG, TAG, "base_path is NULL");
    ESP_RETURN_ON_FALSE(bdl_handle != ESP_BLOCKDEV_HANDLE_INVALID, ESP_ERR_INVALID_ARG, TAG, "invalid BDL handle");
    ESP_RETURN_ON_FALSE(mount_config, ESP_ERR_INVALID_ARG, TAG, "mount_config is NULL");

    BYTE pdrv = 0xFF;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK) {
        ESP_LOGD(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "using pdrv=%i", pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};

    ESP_GOTO_ON_ERROR(ff_diskio_register_bdl(pdrv, bdl_handle), fail, TAG,
                      "ff_diskio_register_bdl failed pdrv=%i, error - 0x(%x)", pdrv, ret);

    FATFS *fs = NULL;
    esp_vfs_fat_conf_t conf = {
        .base_path = base_path,
        .fat_drive = drv,
        .max_files = mount_config->max_files,
    };
    ret = esp_vfs_fat_register(&conf, &fs);
    if (ret == ESP_ERR_INVALID_STATE) {
        // already registered with VFS
    } else if (ret != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register failed 0x(%x)", ret);
        goto fail;
    }

    WORD sec_size_w;
    if (disk_ioctl(pdrv, GET_SECTOR_SIZE, &sec_size_w) != RES_OK) {
        ESP_LOGE(TAG, "failed to query sector size from diskio");
        ret = ESP_FAIL;
        goto fail;
    }
    size_t sec_size = (size_t)sec_size_w;
    size_t sec_num = (size_t)(bdl_handle->geometry.disk_size / sec_size);

    if (bdl_handle->device_flags.read_only) {
        FRESULT fresult = f_mount(fs, drv, 1);
        if (fresult != FR_OK) {
            ESP_LOGW(TAG, "f_mount failed (%d)", fresult);
            ret = ESP_FAIL;
            goto fail;
        }
    } else {
        vfs_fat_x_ctx_flags_t flags = 0;
        ret = try_mount_rw(fs, drv, mount_config, &flags, sec_num, sec_size);
        if (ret != ESP_OK) {
            goto fail;
        }
    }

    ctx = calloc(1, sizeof(vfs_fat_bdl_ctx_t));
    ESP_GOTO_ON_FALSE(ctx, ESP_ERR_NO_MEM, fail, TAG, "no mem");
    ctx->bdl_handle = bdl_handle;
    ctx->pdrv = pdrv;
    ctx->fs = fs;
    memcpy(&ctx->mount_config, mount_config, sizeof(esp_vfs_fat_mount_config_t));

    uint32_t ctx_id = get_unused_ctx_id();
    assert(ctx_id != FF_VOLUMES);
    s_bdl_ctx[ctx_id] = ctx;

    if (bdl_handle->device_flags.read_only) {
        esp_vfs_set_readonly_flag(base_path);
    }

    return ESP_OK;

fail:
    f_mount(0, drv, 0);
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    free(ctx);
    return ret;
}

esp_err_t esp_vfs_fat_bdl_unmount(const char *base_path, esp_blockdev_handle_t bdl_handle)
{
    BYTE pdrv = ff_diskio_get_pdrv_bdl(bdl_handle);
    ESP_RETURN_ON_FALSE(pdrv != 0xff, ESP_ERR_INVALID_STATE, TAG,
                        "BDL device isn't registered, call esp_vfs_fat_bdl_mount first");

    uint32_t id = FF_VOLUMES;
    ESP_RETURN_ON_FALSE(get_ctx_id_by_bdl(bdl_handle, &id), ESP_ERR_INVALID_STATE, TAG,
                        "BDL device isn't registered, call esp_vfs_fat_bdl_mount first");
    assert(id != FF_VOLUMES);

    char drv[3] = {(char)('0' + pdrv), ':', 0};
    f_mount(0, drv, 0);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_bdl(bdl_handle);

    esp_err_t err = esp_vfs_fat_unregister_path(base_path);

    free(s_bdl_ctx[id]);
    s_bdl_ctx[id] = NULL;

    return err;
}
