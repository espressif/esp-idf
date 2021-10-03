/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#else
#define CRYPT_CNT ESP_EFUSE_SPI_BOOT_CRYPT_CNT
#define WR_DIS_CRYPT_CNT ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT
#endif

static const char *TAG = "flash_encrypt";
#ifndef BOOTLOADER_BUILD

void esp_flash_encryption_init_checks()
{
    esp_flash_enc_mode_t mode;

#ifdef CONFIG_SECURE_FLASH_CHECK_ENC_EN_IN_APP
    if (!esp_flash_encryption_enabled()) {
        ESP_LOGE(TAG, "Flash encryption eFuse bit was not enabled in bootloader but CONFIG_SECURE_FLASH_ENC_ENABLED is on");
        abort();
    }
#endif

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
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    uint8_t dis_dl_enc = 0;
    uint8_t dis_dl_icache = 0;
    uint8_t dis_dl_dcache = 0;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    uint8_t dis_dl_enc = 0;
    uint8_t dis_dl_icache = 0;
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
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
            dis_dl_enc = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
            dis_dl_icache = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
            dis_dl_dcache = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);

            if (dis_dl_enc && dis_dl_icache && dis_dl_dcache) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
            }
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
            dis_dl_enc = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
            dis_dl_icache = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);

            if (dis_dl_enc && dis_dl_icache) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
            }
#endif
        }
    } else {
        mode = ESP_FLASH_ENC_MODE_DISABLED;
    }

    return mode;
}

void esp_flash_encryption_set_release_mode(void)
{
    esp_flash_enc_mode_t mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_RELEASE) {
        return;
    }
    if (mode == ESP_FLASH_ENC_MODE_DISABLED) {
        ESP_LOGE(TAG, "Flash encryption eFuse is not enabled, abort..");
        abort();
        return;
    }
    // ESP_FLASH_ENC_MODE_DEVELOPMENT -> ESP_FLASH_ENC_MODE_RELEASE
    esp_efuse_batch_write_begin();
    if (!esp_efuse_read_field_bit(WR_DIS_CRYPT_CNT)) {
        size_t flash_crypt_cnt = 0;
        esp_efuse_read_field_cnt(CRYPT_CNT, &flash_crypt_cnt);
        if (flash_crypt_cnt != CRYPT_CNT[0]->bit_count) {
            esp_efuse_write_field_cnt(CRYPT_CNT, CRYPT_CNT[0]->bit_count - flash_crypt_cnt);
        }
    }
#if CONFIG_IDF_TARGET_ESP32
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_DL_CACHE);
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_DL_ENCRYPT);
    esp_efuse_write_field_bit(ESP_EFUSE_DISABLE_DL_DECRYPT);
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
#else
    ESP_LOGE(TAG, "Flash Encryption support not added, abort..");
    abort();
#endif
    esp_efuse_disable_rom_download_mode();
    esp_efuse_batch_write_commit();

    if (esp_get_flash_encryption_mode() != ESP_FLASH_ENC_MODE_RELEASE) {
        ESP_LOGE(TAG, "Flash encryption mode is DEVELOPMENT, abort..");
        abort();
    }
    ESP_LOGI(TAG, "Flash encryption mode is RELEASE");
}
