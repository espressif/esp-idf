/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "vfs_fat_internal.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "diskio_impl.h"
#include "diskio_sdmmc.h"
#include "soc/soc_caps.h"
#include "driver/sdmmc_defs.h"

#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif

static const char* TAG = "vfs_fat_sdmmc";
static sdmmc_card_t* s_card = NULL;
static uint8_t s_pdrv = FF_DRV_NOT_USED;
static char * s_base_path = NULL;

#define CHECK_EXECUTE_RESULT(err, str) do { \
    if ((err) !=ESP_OK) { \
        ESP_LOGE(TAG, str" (0x%x).", err); \
        goto cleanup; \
    } \
    } while(0)

static void call_host_deinit(const sdmmc_host_t *host_config);
static esp_err_t partition_card(const esp_vfs_fat_mount_config_t *mount_config,
                                const char *drv, sdmmc_card_t *card, BYTE pdrv);

static esp_err_t mount_prepare_mem(const char *base_path,
        BYTE *out_pdrv,
        char **out_dup_path,
        sdmmc_card_t** out_card)
{
    esp_err_t err = ESP_OK;
    char* dup_path = NULL;
    sdmmc_card_t* card = NULL;

    // connect SDMMC driver to FATFS
    BYTE pdrv = FF_DRV_NOT_USED;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK || pdrv == FF_DRV_NOT_USED) {
        ESP_LOGD(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;

    }

    // not using ff_memalloc here, as allocation in internal RAM is preferred
    card = (sdmmc_card_t*)malloc(sizeof(sdmmc_card_t));
    if (card == NULL) {
        ESP_LOGD(TAG, "could not locate new sdmmc_card_t");
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    dup_path = strdup(base_path);
    if(!dup_path){
        ESP_LOGD(TAG, "could not copy base_path");
        err = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    *out_card = card;
    *out_pdrv = pdrv;
    *out_dup_path = dup_path;
    return ESP_OK;
cleanup:
    free(card);
    free(dup_path);
    return err;
}

static esp_err_t mount_to_vfs_fat(const esp_vfs_fat_mount_config_t *mount_config, sdmmc_card_t *card, uint8_t pdrv,
                                  const char *base_path)
{
    FATFS* fs = NULL;
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

static esp_err_t partition_card(const esp_vfs_fat_mount_config_t *mount_config,
                                const char *drv, sdmmc_card_t *card, BYTE pdrv)
{
    FRESULT res = FR_OK;
    esp_err_t err;
    const size_t workbuf_size = 4096;
    void* workbuf = NULL;
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

#if SOC_SDMMC_HOST_SUPPORTED
static esp_err_t init_sdmmc_host(int slot, const void *slot_config, int *out_slot)
{
    *out_slot = slot;
    return sdmmc_host_init_slot(slot, (const sdmmc_slot_config_t*) slot_config);
}


esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
                                  const sdmmc_host_t* host_config,
                                  const void* slot_config,
                                  const esp_vfs_fat_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card)
{
    esp_err_t err;
    int card_handle = -1;   //uninitialized
    sdmmc_card_t* card = NULL;
    BYTE pdrv = FF_DRV_NOT_USED;
    char* dup_path = NULL;
    bool host_inited = false;

    err = mount_prepare_mem(base_path, &pdrv, &dup_path, &card);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mount_prepare failed");
        return err;
    }

    err = (*host_config->init)();
    CHECK_EXECUTE_RESULT(err, "host init failed");
    //deinit() needs to be called to revert the init
    host_inited = true;
    //If this failed (indicated by card_handle != -1), slot deinit needs to called()
    //leave card_handle as is to indicate that (though slot deinit not implemented yet.
    err = init_sdmmc_host(host_config->slot, slot_config, &card_handle);
    CHECK_EXECUTE_RESULT(err, "slot init failed");

    // probe and initialize card
    err = sdmmc_card_init(host_config, card);
    CHECK_EXECUTE_RESULT(err, "sdmmc_card_init failed");

    err = mount_to_vfs_fat(mount_config, card, pdrv, dup_path);
    CHECK_EXECUTE_RESULT(err, "mount_to_vfs failed");

    if (out_card != NULL) {
        *out_card = card;
    }
    if (s_card == NULL) {
        //store the ctx locally to be back-compatible
        s_card = card;
        s_pdrv = pdrv;
        s_base_path = dup_path;
    } else {
        free(dup_path);
    }
    return ESP_OK;
cleanup:
    if (host_inited) {
        call_host_deinit(host_config);
    }
    free(card);
    free(dup_path);
    return err;
}
#endif

static esp_err_t init_sdspi_host(int slot, const void *slot_config, int *out_slot)
{
    esp_err_t err = sdspi_host_init_device((const sdspi_device_config_t*)slot_config, out_slot);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
"Failed to attach sdspi device onto an SPI bus (rc=0x%x), please initialize the \
bus first and check the device parameters."
            , err);
    }
    return err;
}

esp_err_t esp_vfs_fat_sdspi_mount(const char* base_path,
                                  const sdmmc_host_t* host_config_input,
                                  const sdspi_device_config_t* slot_config,
                                  const esp_vfs_fat_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card)
{
    const sdmmc_host_t* host_config = host_config_input;
    esp_err_t err;
    int card_handle = -1;   //uninitialized
    bool host_inited = false;
    BYTE pdrv = FF_DRV_NOT_USED;
    sdmmc_card_t* card = NULL;
    char* dup_path = NULL;

    err = mount_prepare_mem(base_path, &pdrv, &dup_path, &card);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mount_prepare failed");
        return err;
    }

    //the init() function is usually empty, doesn't require any deinit to revert it
    err = (*host_config->init)();
    CHECK_EXECUTE_RESULT(err, "host init failed");

    err = init_sdspi_host(host_config->slot, slot_config, &card_handle);
    CHECK_EXECUTE_RESULT(err, "slot init failed");
    //Set `host_inited` to true to indicate that host_config->deinit() needs
    //to be called to revert `init_sdspi_host`
    host_inited = true;

    /*
     * The `slot` argument inside host_config should be replaced by the SD SPI handled returned
     * above. But the input pointer is const, so create a new variable.
     */
    sdmmc_host_t new_config;
    if (card_handle != host_config->slot) {
        new_config = *host_config_input;
        host_config = &new_config;
        new_config.slot = card_handle;
    }

    // probe and initialize card
    err = sdmmc_card_init(host_config, card);
    CHECK_EXECUTE_RESULT(err, "sdmmc_card_init failed");

    err = mount_to_vfs_fat(mount_config, card, pdrv, dup_path);
    CHECK_EXECUTE_RESULT(err, "mount_to_vfs failed");

    if (out_card != NULL) {
        *out_card = card;
    }
    if (s_card == NULL) {
        //store the ctx locally to be back-compatible
        s_card = card;
        s_pdrv = pdrv;
        s_base_path = dup_path;
    } else {
        free(dup_path);
    }
    return ESP_OK;

cleanup:
    if (host_inited) {
        call_host_deinit(host_config);
    }
    free(card);
    free(dup_path);
    return err;

}

static void local_card_remove(void)
{
    s_card = NULL;
    free(s_base_path);
    s_base_path = NULL;
    s_pdrv = FF_DRV_NOT_USED;
}

static void call_host_deinit(const sdmmc_host_t *host_config)
{
    if (host_config->flags & SDMMC_HOST_FLAG_DEINIT_ARG) {
        host_config->deinit_p(host_config->slot);
    } else {
        host_config->deinit();
    }
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

    call_host_deinit(&card->host);
    free(card);

    esp_err_t err = esp_vfs_fat_unregister_path(base_path);
    return err;
}

esp_err_t esp_vfs_fat_sdmmc_unmount(void)
{
    sdmmc_card_t* card = s_card;
    esp_err_t err = unmount_card_core(s_base_path, card);
    local_card_remove();
    return err;
}

esp_err_t esp_vfs_fat_sdcard_unmount(const char *base_path, sdmmc_card_t *card)
{
    esp_err_t err = unmount_card_core(base_path, card);
    if (s_card == card) {
        local_card_remove();
    }
    return err;
}
