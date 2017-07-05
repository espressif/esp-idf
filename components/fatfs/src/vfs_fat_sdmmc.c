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
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "diskio.h"

static const char* TAG = "vfs_fat_sdmmc";
static sdmmc_card_t* s_card = NULL;
static uint8_t s_pdrv = 0;
static char * s_base_path = NULL;

esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
    const sdmmc_host_t* host_config,
    const sdmmc_slot_config_t* slot_config,
    const esp_vfs_fat_sdmmc_mount_config_t* mount_config,
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

    // enable SDMMC
    sdmmc_host_init();

    // enable card slot
    esp_err_t err = sdmmc_host_init_slot(host_config->slot, slot_config);
    if (err != ESP_OK) {
        return err;
    }

    s_card = malloc(sizeof(sdmmc_card_t));
    if (s_card == NULL) {
        err = ESP_ERR_NO_MEM;
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
        DWORD plist[] = {100, 0, 0, 0};
        workbuf = malloc(workbuf_size);
        res = f_fdisk(s_pdrv, plist, workbuf);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_fdisk failed (%d)", res);
            goto fail;
        }
        ESP_LOGW(TAG, "formatting card");
        res = f_mkfs(drv, FM_ANY, s_card->csd.sector_size, workbuf, workbuf_size);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_mkfs failed (%d)", res);
            goto fail;
        }
        free(workbuf);
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
    sdmmc_host_deinit();
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
    ff_diskio_unregister(s_pdrv);
    free(s_card);
    s_card = NULL;
    sdmmc_host_deinit();
    esp_err_t err = esp_vfs_fat_unregister_path(s_base_path);
    free(s_base_path);
    s_base_path = NULL;
    return err;
}
