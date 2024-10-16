/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "diskio_rawflash.h"
#include "wear_levelling.h"
#include "diskio_wl.h"

// If the available sectors based on partition size are less than 128,
// the root directory sector should be set to 1.
#define MIN_REQ_SEC    128

static const char* TAG = "vfs_fat_spiflash";

static vfs_fat_spiflash_ctx_t *s_ctx[FF_VOLUMES] = {};

extern esp_err_t esp_vfs_set_readonly_flag(const char* base_path); // from vfs/vfs.c to set readonly flag in esp_vfs_t struct externally

static bool s_get_context_id_by_label(const char *label, uint32_t *out_id)
{
    vfs_fat_spiflash_ctx_t *p_ctx = NULL;
    for (int i = 0; i < FF_VOLUMES; i++) {
        p_ctx = s_ctx[i];
        if (p_ctx) {
            if (!label && !p_ctx->by_label) {
                *out_id = i;
                return true;
            }
            if (label && p_ctx->by_label && strncmp(label, p_ctx->partition->label, 20) == 0) {
                *out_id = i;
                return true;
            }
        }
    }
    return false;
}

static bool s_get_context_id_by_wl_handle(wl_handle_t wlhandle, uint32_t *out_id)
{
    vfs_fat_spiflash_ctx_t *p_ctx = NULL;
    for (int i = 0; i < FF_VOLUMES; i++) {
        p_ctx = s_ctx[i];
        if (p_ctx) {
            if (p_ctx->wlhandle == wlhandle) {
                *out_id = i;
                return true;
            }
        }
    }
    return false;
}

static uint32_t s_get_unused_context_id(void)
{
    for (uint32_t i = 0; i < FF_VOLUMES; i++) {
        if (!s_ctx[i]) {
            return i;
        }
    }
    return FF_VOLUMES;
}

vfs_fat_spiflash_ctx_t* get_vfs_fat_spiflash_ctx(wl_handle_t wlhandle)
{
    uint32_t id = FF_VOLUMES;
    if (s_get_context_id_by_wl_handle(wlhandle, &id)) {
        return s_ctx[id];
    }
    return NULL;
}

static esp_err_t s_f_mount_rw(FATFS *fs, const char *drv, const esp_vfs_fat_mount_config_t *mount_config, vfs_fat_x_ctx_flags_t *out_flags, size_t sec_num)
{
    FRESULT fresult = f_mount(fs, drv, 1);
    if (fresult != FR_OK) {
        ESP_LOGW(TAG, "f_mount failed (%d)", fresult);

        bool need_mount_again = (fresult == FR_NO_FILESYSTEM || fresult == FR_INT_ERR) && mount_config->format_if_mount_failed;
        if (!need_mount_again) {
            return ESP_FAIL;
        }

        const size_t workbuf_size = 4096;
        void *workbuf = ff_memalloc(workbuf_size);
        if (workbuf == NULL) {
            return ESP_ERR_NO_MEM;
        }

        size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(CONFIG_WL_SECTOR_SIZE, mount_config->allocation_unit_size);
        ESP_LOGI(TAG, "Formatting FATFS partition, allocation unit size=%d", alloc_unit_size);
        UINT root_dir_entries;
        if (CONFIG_WL_SECTOR_SIZE == 512) {
            root_dir_entries = 16;
        } else {
            root_dir_entries = 128;
        }
        const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), (mount_config->use_one_fat ? 1 : 2), 0, (sec_num <= MIN_REQ_SEC ? root_dir_entries : 0), alloc_unit_size};
        fresult = f_mkfs(drv, &opt, workbuf, workbuf_size);
        free(workbuf);
        workbuf = NULL;
        ESP_RETURN_ON_FALSE(fresult == FR_OK, ESP_FAIL, TAG, "f_mkfs failed (%d)", fresult);

        if (out_flags) {
            *out_flags |= FORMATTED_DURING_LAST_MOUNT; // set flag
        }

        ESP_LOGI(TAG, "Mounting again");
        fresult = f_mount(fs, drv, 1);
        ESP_RETURN_ON_FALSE(fresult == FR_OK, ESP_FAIL, TAG, "f_mount failed after formatting (%d)", fresult);
    } else {
        if (out_flags) {
            *out_flags  &= ~FORMATTED_DURING_LAST_MOUNT; // reset flag
        }
    }
    return ESP_OK;
}

esp_err_t esp_vfs_fat_spiflash_mount_rw_wl(const char* base_path,
    const char* partition_label,
    const esp_vfs_fat_mount_config_t* mount_config,
    wl_handle_t* wl_handle)
{
    esp_err_t ret = ESP_OK;
    vfs_fat_spiflash_ctx_t *ctx = NULL;
    uint32_t ctx_id = FF_VOLUMES;

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
    esp_vfs_fat_conf_t conf = {
        .base_path = base_path,
        .fat_drive = drv,
        .max_files = mount_config->max_files,
    };
    ret = esp_vfs_fat_register_cfg(&conf, &fs);
    if (ret == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (ret != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register_cfg failed 0x(%x)", ret);
        goto fail;
    }

    vfs_fat_x_ctx_flags_t flags = 0;

    size_t sec_num = wl_size(*wl_handle) / wl_sector_size(*wl_handle);
    // Try to mount partition
    ret = s_f_mount_rw(fs, drv, mount_config, &flags, sec_num);
    if (ret != ESP_OK) {
        goto fail;
    }

    ctx = calloc(1, sizeof(vfs_fat_spiflash_ctx_t));
    ESP_GOTO_ON_FALSE(ctx, ESP_ERR_NO_MEM, fail, TAG, "no mem");
    ctx->partition = data_partition;
    ctx->by_label = (partition_label != NULL);
    ctx->pdrv = pdrv;
    ctx->fs = fs;
    ctx->wlhandle = *wl_handle;
    ctx->flags = flags;
    memcpy(&ctx->mount_config, mount_config, sizeof(esp_vfs_fat_mount_config_t));
    ctx_id = s_get_unused_context_id();
    //At this stage, we should always get a free context, otherwise program should return already
    assert(ctx_id != FF_VOLUMES);
    s_ctx[ctx_id] = ctx;

    if (data_partition->readonly) {
        esp_vfs_set_readonly_flag(base_path);
    }

    return ESP_OK;

fail:
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    free(ctx);
    return ret;
}

esp_err_t esp_vfs_fat_spiflash_unmount_rw_wl(const char* base_path, wl_handle_t wl_handle)
{
    BYTE pdrv = ff_diskio_get_pdrv_wl(wl_handle);
    ESP_RETURN_ON_FALSE(pdrv != 0xff, ESP_ERR_INVALID_STATE, TAG, "partition isn't registered, call esp_vfs_fat_spiflash_mount_rw_wl first");

    uint32_t id = FF_VOLUMES;
    ESP_RETURN_ON_FALSE(s_get_context_id_by_wl_handle(wl_handle, &id), ESP_ERR_INVALID_STATE, TAG, "partition isn't registered, call esp_vfs_fat_spiflash_mount_rw_wl first");
    //At this stage, as the wl_handle is valid, we should always get its context id, otherwise program should return already
    assert(id != FF_VOLUMES);

    char drv[3] = {(char)('0' + pdrv), ':', 0};
    f_mount(0, drv, 0);
    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_wl(wl_handle);
    // release partition driver
    esp_err_t err_drv = wl_unmount(wl_handle);
    esp_err_t err = esp_vfs_fat_unregister_path(base_path);
    if (err == ESP_OK) {
        err = err_drv;
    }

    free(s_ctx[id]);
    s_ctx[id] = NULL;

    return err;
}

esp_err_t esp_vfs_fat_spiflash_format_cfg_rw_wl(const char* base_path, const char* partition_label, esp_vfs_fat_mount_config_t *cfg)
{
    esp_err_t ret = ESP_OK;
    bool partition_was_mounted = false;

    wl_handle_t temp_handle = WL_INVALID_HANDLE;
    uint32_t id = FF_VOLUMES;
    size_t sec_num = 0;

    bool found = s_get_context_id_by_label(partition_label, &id);
    if (!found) {
        esp_vfs_fat_mount_config_t default_mount_config = {
            .max_files = 1,
            .format_if_mount_failed = true,
        };
        esp_vfs_fat_mount_config_t *mount_cfg = NULL;
        if (cfg) {
            mount_cfg = cfg;
        } else {
            mount_cfg = &default_mount_config;
        }
        ESP_RETURN_ON_ERROR(esp_vfs_fat_spiflash_mount_rw_wl(base_path, partition_label, mount_cfg, &temp_handle), TAG, "Failed to mount");
        found = s_get_context_id_by_label(partition_label, &id);
        sec_num = wl_size(temp_handle) / wl_sector_size(temp_handle);
        assert(found);
        if (s_ctx[id]->flags & FORMATTED_DURING_LAST_MOUNT) {
            ESP_LOGD(TAG, "partition was formatted during mounting, skipping another format");
            ret = ESP_OK;
            goto mount_back;
        }
    } else {
        partition_was_mounted = true;
        if (cfg) {
            s_ctx[id]->mount_config = *cfg;
        }
        temp_handle = s_ctx[id]->wlhandle;
        sec_num = wl_size(temp_handle) / wl_sector_size(temp_handle);
    }

    //unmount
    char drv[3] = {(char)('0' + s_ctx[id]->pdrv), ':', 0};
    FRESULT fresult = f_mount(0, drv, 0);
    ESP_RETURN_ON_FALSE(fresult != FR_INVALID_DRIVE, ESP_FAIL, TAG, "f_mount unmount failed (%d) - the logical drive number is invalid", fresult);
    ESP_GOTO_ON_FALSE(fresult == FR_OK, ESP_FAIL, recycle, TAG, "f_mount unmount failed (%d), go to recycle", fresult);

    const size_t workbuf_size = 4096;
    void *workbuf = ff_memalloc(workbuf_size);
    if (workbuf == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto mount_back;
    }
    size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(CONFIG_WL_SECTOR_SIZE, s_ctx[id]->mount_config.allocation_unit_size);
    ESP_LOGI(TAG, "Formatting FATFS partition, allocation unit size=%d", alloc_unit_size);
    UINT root_dir_entries;
    if (CONFIG_WL_SECTOR_SIZE == 512) {
        root_dir_entries = 16;
    } else {
        root_dir_entries = 128;
    }
    const MKFS_PARM opt = {(BYTE)(FM_ANY | FM_SFD), (s_ctx[id]->mount_config.use_one_fat ? 1 : 2), 0, (sec_num <= MIN_REQ_SEC ? root_dir_entries : 0), alloc_unit_size};
    fresult = f_mkfs(drv, &opt, workbuf, workbuf_size);
    free(workbuf);
    workbuf = NULL;
    ESP_GOTO_ON_FALSE(fresult == FR_OK, ESP_FAIL, mount_back, TAG, "f_mkfs failed (%d)", fresult);

mount_back:
    if (partition_was_mounted) {
        esp_err_t err = s_f_mount_rw(s_ctx[id]->fs, drv, &s_ctx[id]->mount_config, NULL, sec_num);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to mount back, go to recycle");
            goto recycle;
        }
    } else {
        esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_ctx[id]->wlhandle);
    }
    return ret;

recycle:
    ff_diskio_unregister(s_ctx[id]->pdrv);
    ff_diskio_clear_pdrv_wl(s_ctx[id]->wlhandle);
    wl_unmount(s_ctx[id]->wlhandle);
    esp_vfs_fat_unregister_path(base_path);
    free(s_ctx[id]);
    s_ctx[id] = NULL;
    ESP_LOGE(TAG, "failed to format, resources recycled, please mount again");
    return ret;
}

esp_err_t esp_vfs_fat_spiflash_format_rw_wl(const char* base_path, const char* partition_label)
{
    return esp_vfs_fat_spiflash_format_cfg_rw_wl(base_path, partition_label, NULL);
}

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
    esp_vfs_fat_conf_t conf = {
        .base_path = base_path,
        .fat_drive = drv,
        .max_files = mount_config->max_files,
    };
    ret = esp_vfs_fat_register_cfg(&conf, &fs);
    if (ret == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (ret != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register_cfg failed 0x(%x)", ret);
        goto fail;
    }

    // Try to mount partition
    FRESULT fresult = f_mount(fs, drv, 1);
    if (fresult != FR_OK) {
        ESP_LOGW(TAG, "f_mount failed (%d)", fresult);
        ret = ESP_FAIL;
        goto fail;
    }

    if (data_partition->readonly) {
        esp_vfs_set_readonly_flag(base_path);
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
