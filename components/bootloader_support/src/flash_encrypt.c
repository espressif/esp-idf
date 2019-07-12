// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <strings.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"

#ifndef BOOTLOADER_BUILD
static const char *TAG = "flash_encrypt";

void esp_flash_encryption_init_checks()
{
    esp_flash_enc_mode_t mode;

    // First check is: if Release mode flash encryption & secure boot are enabled then
    // FLASH_CRYPT_CNT *must* be write protected. This will have happened automatically
    // if bootloader is IDF V4.0 or newer but may not have happened for previous ESP-IDF bootloaders.
#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
#ifdef CONFIG_SECURE_BOOT_ENABLED
    if (esp_secure_boot_enabled() && esp_flash_encryption_enabled()) {
        uint8_t flash_crypt_cnt_wr_dis = 0;
        esp_efuse_read_field_blob(ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT, &flash_crypt_cnt_wr_dis, 1);
        if (!flash_crypt_cnt_wr_dis) {
            ESP_EARLY_LOGE(TAG, "Flash encryption & Secure Boot together requires FLASH_CRYPT_CNT efuse to be write protected. Fixing now...");
            esp_flash_write_protect_crypt_cnt();
        }
    }
#endif // CONFIG_SECURE_BOOT_ENABLED
#endif // CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE

    // Second check is to print a warning or error if the current running flash encryption mode
    // doesn't match the expectation from project config (due to mismatched bootloader and app, probably)
    mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_DEVELOPMENT) {
#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
        ESP_EARLY_LOGE(TAG, "Flash encryption settings error: app is configured for RELEASE but efuses are set for DEVELOPMENT");
        ESP_EARLY_LOGE(TAG, "Mismatch found in security options in bootloader menuconfig and efuse settings. Device is not secure.");
#else
        ESP_EARLY_LOGW(TAG, "Flash encryption mode is DEVELOPMENT (not secure)");
#endif
    } else if (mode == ESP_FLASH_ENC_MODE_RELEASE) {
        ESP_EARLY_LOGI(TAG, "Flash encryption mode is RELEASE");
    }
}
#endif

void esp_flash_write_protect_crypt_cnt()
{
    uint8_t flash_crypt_cnt_wr_dis = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT, &flash_crypt_cnt_wr_dis, 1);
    if (!flash_crypt_cnt_wr_dis) {
        esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT, 1);
    }
}

esp_flash_enc_mode_t esp_get_flash_encryption_mode()
{
    uint8_t efuse_flash_crypt_cnt_wr_protected = 0;
    uint8_t dis_dl_enc = 0, dis_dl_dec = 0, dis_dl_cache = 0;
    esp_flash_enc_mode_t mode = ESP_FLASH_ENC_MODE_DEVELOPMENT;

    if (esp_flash_encryption_enabled()) {
        /* Check if FLASH CRYPT CNT is write protected */
        esp_efuse_read_field_blob(ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT, &efuse_flash_crypt_cnt_wr_protected, 1);
        if (efuse_flash_crypt_cnt_wr_protected) {
            esp_efuse_read_field_blob(ESP_EFUSE_DISABLE_DL_CACHE, &dis_dl_cache, 1);
            esp_efuse_read_field_blob(ESP_EFUSE_DISABLE_DL_ENCRYPT, &dis_dl_enc, 1);
            esp_efuse_read_field_blob(ESP_EFUSE_DISABLE_DL_DECRYPT, &dis_dl_dec, 1);
            /* Check if DISABLE_DL_DECRYPT, DISABLE_DL_ENCRYPT & DISABLE_DL_CACHE are set */
            if ( dis_dl_cache && dis_dl_enc && dis_dl_dec ) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
            }
        }
    } else {
        mode = ESP_FLASH_ENC_MODE_DISABLED;
    }

    return mode;
}
