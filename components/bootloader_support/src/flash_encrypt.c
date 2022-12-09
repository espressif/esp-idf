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

#if CONFIG_IDF_TARGET_ESP32
#define CRYPT_CNT ESP_EFUSE_FLASH_CRYPT_CNT
#define WR_DIS_CRYPT_CNT ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT
#elif CONFIG_IDF_TARGET_ESP32S2
#define CRYPT_CNT ESP_EFUSE_SPI_BOOT_CRYPT_CNT
#define WR_DIS_CRYPT_CNT ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT
#endif

#ifndef BOOTLOADER_BUILD
static const char *TAG = "flash_encrypt";

void esp_flash_encryption_init_checks()
{
    esp_flash_enc_mode_t mode;

    // First check is: if Release mode flash encryption & secure boot are enabled then
    // FLASH_CRYPT_CNT *must* be write protected. This will have happened automatically
    // if bootloader is IDF V4.0 or newer but may not have happened for previous ESP-IDF bootloaders.
#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
#ifdef CONFIG_SECURE_BOOT
    if (esp_secure_boot_enabled() && esp_flash_encryption_enabled()) {
        bool flash_crypt_cnt_wr_dis = esp_efuse_read_field_bit(WR_DIS_CRYPT_CNT);
        if (!flash_crypt_cnt_wr_dis) {
            uint8_t flash_crypt_cnt = 0;
            esp_efuse_read_field_blob(CRYPT_CNT, &flash_crypt_cnt,  CRYPT_CNT[0]->bit_count);
            if (flash_crypt_cnt == (1<<(CRYPT_CNT[0]->bit_count))-1) {
                // If encryption counter is already max, no need to write protect it
                // (this distinction is important on ESP32 ECO3 where write-procted FLASH_CRYPT_CNT also write-protects UART_DL_DIS)
                return;
            }
            ESP_LOGE(TAG, "Flash encryption & Secure Boot together requires FLASH_CRYPT_CNT efuse to be write protected. Fixing now...");
            esp_flash_write_protect_crypt_cnt();
        }
    }
#endif // CONFIG_SECURE_BOOT
#endif // CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE

    // Second check is to print a warning or error if the current running flash encryption mode
    // doesn't match the expectation from project config (due to mismatched bootloader and app, probably)
    mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_DEVELOPMENT) {
#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
        ESP_LOGE(TAG, "Flash encryption settings error: app is configured for RELEASE but efuses are set for DEVELOPMENT");
        ESP_LOGE(TAG, "Mismatch found in security options in bootloader menuconfig and efuse settings. Device is not secure.");
#else
        ESP_LOGW(TAG, "Flash encryption mode is DEVELOPMENT (not secure)");
#endif
    } else if (mode == ESP_FLASH_ENC_MODE_RELEASE) {
        ESP_LOGI(TAG, "Flash encryption mode is RELEASE");
    }
}
#endif

void esp_flash_write_protect_crypt_cnt(void)
{
    esp_efuse_write_field_bit(WR_DIS_CRYPT_CNT);
}

esp_flash_enc_mode_t esp_get_flash_encryption_mode(void)
{
    bool flash_crypt_cnt_wr_dis = false;
#if CONFIG_IDF_TARGET_ESP32   
    uint8_t dis_dl_enc = 0, dis_dl_dec = 0, dis_dl_cache = 0;
#elif CONFIG_IDF_TARGET_ESP32S2
    uint8_t  dis_dl_enc = 0; 
    uint8_t dis_dl_icache = 0; 
    uint8_t dis_dl_dcache = 0; 

#endif

    esp_flash_enc_mode_t mode = ESP_FLASH_ENC_MODE_DEVELOPMENT;

    if (esp_flash_encryption_enabled()) {
        /* Check if FLASH CRYPT CNT is write protected */

        flash_crypt_cnt_wr_dis = esp_efuse_read_field_bit(WR_DIS_CRYPT_CNT);
        if (!flash_crypt_cnt_wr_dis) {
            uint8_t flash_crypt_cnt = 0;
            esp_efuse_read_field_blob(CRYPT_CNT, &flash_crypt_cnt, CRYPT_CNT[0]->bit_count);
            if (flash_crypt_cnt == (1 << (CRYPT_CNT[0]->bit_count)) - 1) {
                flash_crypt_cnt_wr_dis = true;
            }
        }

        if (flash_crypt_cnt_wr_dis) {

#if CONFIG_IDF_TARGET_ESP32
            dis_dl_cache = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_CACHE);
            dis_dl_enc = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_ENCRYPT);
            dis_dl_dec = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_DECRYPT);
            /* Check if DISABLE_DL_DECRYPT, DISABLE_DL_ENCRYPT & DISABLE_DL_CACHE are set */
            if ( dis_dl_cache && dis_dl_enc && dis_dl_dec ) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
            }
#elif CONFIG_IDF_TARGET_ESP32S2
            dis_dl_enc = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
            dis_dl_icache = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
            dis_dl_dcache = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);

            if (dis_dl_enc && dis_dl_icache && dis_dl_dcache) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
            }
#endif
        }
    } else {
        mode = ESP_FLASH_ENC_MODE_DISABLED;
    }

    return mode;
}
