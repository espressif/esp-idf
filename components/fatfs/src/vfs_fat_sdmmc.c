// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "vfs_fat_internal.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "diskio.h"

static const char* TAG = "vfs_fat_sdmmc";
static sdmmc_card_t* s_card = NULL;
static uint8_t s_pdrv = 0;
static char * s_base_path = NULL;

esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
    const sdmmc_host_t* host_config,
    const void* slot_config,
    const esp_vfs_fat_mount_config_t* mount_config,
    sdmmc_card_t** out_card)
{
    const size_t workbuf_size = 4096;
    void* workbuf = NULL;
    FATFS* fs = NULL;

    if (s_card != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    // connect SDMMC driver to FATFS
    BYTE pdrv = 0xFF;
    if (ff_diskio_get_drive(&pdrv) != ESP_OK || pdrv == 0xFF) {
        ESP_LOGD(TAG, "the maximum count of volumes is already mounted");
        return ESP_ERR_NO_MEM;
    }

    s_base_path = strdup(base_path);
    if(!s_base_path){
        ESP_LOGD(TAG, "could not copy base_path");
        return ESP_ERR_NO_MEM;
    }
    esp_err_t err = ESP_OK;
    s_card = malloc(sizeof(sdmmc_card_t));
    if (s_card == NULL) {
        err = ESP_ERR_NO_MEM;
        goto fail;
    }

    err = (*host_config->init)();
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "host init returned rc=0x%x", err);
        goto fail;
    }

    // configure SD slot
    if (host_config->flags == SDMMC_HOST_FLAG_SPI) {
        err = sdspi_host_init_slot(host_config->slot,
                (const sdspi_slot_config_t*) slot_config);
    } else {
        err = sdmmc_host_init_slot(host_config->slot,
                (const sdmmc_slot_config_t*) slot_config);
    }
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "slot_config returned rc=0x%x", err);
        goto fail;
    }

    // probe and initialize card
    err = sdmmc_card_init(host_config, s_card);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "sdmmc_card_init failed 0x(%x)", err);
        goto fail;
    }
    if (out_card != NULL) {
        *out_card = s_card;
    }

    ff_diskio_register_sdmmc(pdrv, s_card);
    s_pdrv = pdrv;
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
        if (!(res == FR_NO_FILESYSTEM && mount_config->format_if_mount_failed)) {
            goto fail;
        }
        ESP_LOGW(TAG, "partitioning card");
        workbuf = malloc(workbuf_size);
        if (workbuf == NULL) {
            err = ESP_ERR_NO_MEM;
            goto fail;
        }
        DWORD plist[] = {100, 0, 0, 0};
        res = f_fdisk(s_pdrv, plist, workbuf);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_fdisk failed (%d)", res);
            goto fail;
        }
        size_t alloc_unit_size = esp_vfs_fat_get_allocation_unit_size(
                s_card->csd.sector_size,
                mount_config->allocation_unit_size);
        ESP_LOGW(TAG, "formatting card, allocation unit size=%d", alloc_unit_size);
        res = f_mkfs(drv, FM_ANY, alloc_unit_size, workbuf, workbuf_size);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_mkfs failed (%d)", res);
            goto fail;
        }
        free(workbuf);
        workbuf = NULL;
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
    host_config->deinit();
    free(workbuf);
    if (fs) {
        f_mount(NULL, drv, 0);
    }
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(pdrv);
    free(s_card);
    s_card = NULL;
    return err;
}

esp_err_t esp_vfs_fat_sdmmc_unmount()
{
    if (s_card == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    // unmount
    char drv[3] = {(char)('0' + s_pdrv), ':', 0};
    f_mount(0, drv, 0);
    // release SD driver
    esp_err_t (*host_deinit)() = s_card->host.deinit;
    ff_diskio_unregister(s_pdrv);
    free(s_card);
    s_card = NULL;
    (*host_deinit)();
    esp_err_t err = esp_vfs_fat_unregister_path(s_base_path);
    free(s_base_path);
    s_base_path = NULL;
    return err;
}
