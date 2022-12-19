/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

// DESCRIPTION:
// This file contains the code for accessing the storage medium in SD card.

#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "driver/sdmmc_host.h"
#include "diskio_sdmmc.h"
#include "tusb_msc_storage.h"

static sdmmc_card_t *s_card;
static bool s_fat_mounted;
static const char *s_base_path;

static const char *TAG = "msc_sdmmc";

esp_err_t storage_init(const tinyusb_config_storage_t *config)
{
    esp_err_t ret = ESP_OK;
    bool host_init = false;
    assert(config->storage_type == TINYUSB_STORAGE_SDMMC);

    ESP_LOGI(TAG, "Initializing SDCard");

    sdmmc_host_t host = config->sdmmc_config->host;
    sdmmc_slot_config_t slot_config = config->sdmmc_config->slot_config;

    // not using ff_memalloc here, as allocation in internal RAM is preferred
    s_card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    ESP_GOTO_ON_FALSE(s_card, ESP_ERR_NO_MEM, clean, TAG, "could not allocate new sdmmc_card_t");

    ESP_GOTO_ON_ERROR((*host.init)(), clean, TAG, "Host Config Init fail");
    host_init = true;

    ESP_GOTO_ON_ERROR(sdmmc_host_init_slot(host.slot, (const sdmmc_slot_config_t *) &slot_config),
                      clean, TAG, "Host init slot fail");

    while (sdmmc_card_init(&host, s_card)) {
        ESP_LOGE(TAG, "The detection pin of the slot is disconnected(Insert uSD card). Retrying...");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, s_card);

    return ESP_OK;

clean:
    if (host_init) {
        if (host.flags & SDMMC_HOST_FLAG_DEINIT_ARG) {
            host.deinit_p(host.slot);
        } else {
            (*host.deinit)();
        }
    }
    if (s_card) {
        free(s_card);
        s_card = NULL;
    }
    return ret;
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

static esp_err_t partition_card(const esp_vfs_fat_mount_config_t *mount_config,
                                const char *drv, sdmmc_card_t *card, BYTE pdrv)
{
    FRESULT res = FR_OK;
    esp_err_t err;
    const size_t workbuf_size = 4096;
    void *workbuf = NULL;
    ESP_LOGW(TAG, "partitioning card");

    workbuf = ff_memalloc(workbuf_size);
    if (workbuf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    LBA_t plist[] = {100, 0, 0, 0};
    res = f_fdisk(pdrv, plist, workbuf);
    if (res != FR_OK) {
        err = ESP_FAIL;
        ESP_LOGD(TAG, "f_fdisk failed (%d)", res);
        goto fail;
    }
    size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
                                 card->csd.sector_size,
                                 mount_config->allocation_unit_size);
    ESP_LOGW(TAG, "formatting card, allocation unit size=%d", alloc_unit_size);
    const MKFS_PARM opt = {(BYTE)FM_ANY, 0, 0, 0, alloc_unit_size};
    res = f_mkfs(drv, &opt, workbuf, workbuf_size);
    if (res != FR_OK) {
        err = ESP_FAIL;
        ESP_LOGD(TAG, "f_mkfs failed (%d)", res);
        goto fail;
    }

    free(workbuf);
    return ESP_OK;
fail:
    free(workbuf);
    return err;
}

static esp_err_t mount_to_vfs_fat(const esp_vfs_fat_mount_config_t *mount_config, sdmmc_card_t *card, uint8_t pdrv,
                                  const char *base_path)
{
    FATFS *fs = NULL;
    esp_err_t err;
    ff_diskio_register_sdmmc(pdrv, card);
    ff_sdmmc_set_disk_status_check(pdrv, mount_config->disk_status_check_enable);
    ESP_LOGD(TAG, "using pdrv=%i", pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};

    // connect FATFS to VFS
    err = esp_vfs_fat_register(base_path, drv, mount_config->max_files, &fs);
    if (err == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (err != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register failed 0x(%x)", err);
        goto fail;
    }

    // Try to mount partition
    FRESULT res = f_mount(fs, drv, 1);
    if (res != FR_OK) {
        err = ESP_FAIL;
        ESP_LOGW(TAG, "failed to mount card (%d)", res);
        if (!((res == FR_NO_FILESYSTEM || res == FR_INT_ERR)
                && mount_config->format_if_mount_failed)) {
            goto fail;
        }

        err = partition_card(mount_config, drv, card, pdrv);
        if (err != ESP_OK) {
            goto fail;
        }

        ESP_LOGW(TAG, "mounting again");
        res = f_mount(fs, drv, 0);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_mount failed after formatting (%d)", res);
            goto fail;
        }
    }
    return ESP_OK;

fail:
    if (fs) {
        f_mount(NULL, drv, 0);
    }
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    return err;
}

esp_err_t storage_mount(const char *base_path)
{
    if (s_fat_mounted) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing FAT");

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    // connect driver to FATFS
    BYTE pdrv = 0xFF;
    ESP_RETURN_ON_ERROR(ff_diskio_get_drive(&pdrv), TAG,
                        "The maximum count of volumes is already mounted");

    ESP_LOGI(TAG, "using pdrv=%i", pdrv);

    ESP_RETURN_ON_ERROR(mount_to_vfs_fat(&mount_config, s_card, pdrv, base_path), TAG,
                        "Failed to mount storage");

    s_fat_mounted = true;
    s_base_path = base_path;
    return ESP_OK;
}

static esp_err_t unmount_card_core(const char *base_path, sdmmc_card_t *card)
{
    BYTE pdrv = ff_diskio_get_pdrv_card(card);
    if (pdrv == 0xff) {
        return ESP_ERR_INVALID_ARG;
    }

    // unmount
    char drv[3] = {(char)('0' + pdrv), ':', 0};
    f_mount(0, drv, 0);
    // release SD driver
    ff_diskio_unregister(pdrv);

    return esp_vfs_fat_unregister_path(base_path);
}

esp_err_t storage_unmount(void)
{
    if (!s_fat_mounted) {
        return ESP_OK;
    }

    esp_err_t err = unmount_card_core(s_base_path, s_card);

    s_base_path = NULL;
    s_fat_mounted = false;

    return err;

}

uint32_t storage_get_sector_count(void)
{
    assert(s_card);

    return (uint32_t)s_card->csd.capacity;
}

uint32_t storage_get_sector_size(void)
{
    assert(s_card);

    return (uint32_t)s_card->csd.sector_size;
}

esp_err_t storage_read_sector(uint32_t lba, uint32_t offset, size_t size, void *dest)
{
    assert(s_card);
    return sdmmc_read_sectors(s_card, dest, lba, size / storage_get_sector_size());
}

esp_err_t storage_write_sector(uint32_t lba, uint32_t offset, size_t size, const void *src)
{
    assert(s_card);

    if (s_fat_mounted) {
        ESP_LOGE(TAG, "can't write, FAT mounted");
        return ESP_ERR_INVALID_STATE;
    }
    size_t addr = lba * storage_get_sector_size() + offset; // Address of the data to be read, relative to the beginning of the partition.
    size_t sector_size = s_card->csd.sector_size;
    if (addr % sector_size != 0 || size % sector_size != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(sdmmc_erase_sectors(s_card, lba, size / storage_get_sector_size(), SDMMC_ERASE_ARG),
                        TAG, "Failed to erase");

    return sdmmc_write_sectors(s_card, src, lba, size / storage_get_sector_size());
}
