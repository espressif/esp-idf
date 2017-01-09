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
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "diskio.h"

static const char* TAG = "vfs_fat_sdmmc";
static sdmmc_card_t* s_card = NULL;

esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
    const sdmmc_host_t* host_config,
    const sdmmc_slot_config_t* slot_config,
    const esp_vfs_fat_sdmmc_mount_config_t* mount_config,
    sdmmc_card_t** out_card)
{
    const size_t workbuf_size = 4096;
    void* workbuf = NULL;

    if (s_card != NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    // enable SDMMC
    sdmmc_host_init();

    // enable card slot
    sdmmc_host_init_slot(host_config->slot, slot_config);
    s_card = malloc(sizeof(sdmmc_card_t));
    if (s_card == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // probe and initialize card
    esp_err_t err = sdmmc_card_init(host_config, s_card);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "sdmmc_card_init failed 0x(%x)", err);
        goto fail;
    }
    if (out_card != NULL) {
        *out_card = s_card;
    }

    // connect SDMMC driver to FATFS
    ff_diskio_register_sdmmc(0, s_card);

    // connect FATFS to VFS
    FATFS* fs;
    err = esp_vfs_fat_register(base_path, "", mount_config->max_files, &fs);
    if (err == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (err != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register failed 0x(%x)", err);
        goto fail;
    }

    // Try to mount partition
    FRESULT res = f_mount(fs, "", 1);
    if (res != FR_OK) {
        err = ESP_FAIL;
        ESP_LOGW(TAG, "failed to mount card (%d)", res);
        if (!(res == FR_NO_FILESYSTEM && mount_config->format_if_mount_failed)) {
            goto fail;
        }
        ESP_LOGW(TAG, "partitioning card");
        DWORD plist[] = {100, 0, 0, 0};
        workbuf = malloc(workbuf_size);
        res = f_fdisk(0, plist, workbuf);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_fdisk failed (%d)", res);
            goto fail;
        }
        ESP_LOGW(TAG, "formatting card");
        res = f_mkfs("", FM_ANY, s_card->csd.sector_size, workbuf, workbuf_size);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_mkfs failed (%d)", res);
            goto fail;
        }
        free(workbuf);
        ESP_LOGW(TAG, "mounting again");
        res = f_mount(fs, "", 0);
        if (res != FR_OK) {
            err = ESP_FAIL;
            ESP_LOGD(TAG, "f_mount failed after formatting (%d)", res);
            goto fail;
        }
    }
    return ESP_OK;

fail:
    free(workbuf);
    esp_vfs_unregister(base_path);
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
    f_mount(0, "", 0);
    // release SD driver
    free(s_card);
    s_card = NULL;
    sdmmc_host_deinit();
    return esp_vfs_fat_unregister();
}
