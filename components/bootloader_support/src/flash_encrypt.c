/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/efuse_hal.h"

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
#endif // CONFIG_SECURE_FLASH_CHECK_ENC_EN_IN_APP

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
            } else {
                ESP_LOGE(TAG, "Flash encryption & Secure Boot together requires FLASH_CRYPT_CNT efuse to be write protected. Fixing now...");
                esp_flash_write_protect_crypt_cnt();
            }
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
#endif // CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
    } else if (mode == ESP_FLASH_ENC_MODE_RELEASE) {
        ESP_LOGI(TAG, "Flash encryption mode is RELEASE");
    }
}
#endif // BOOTLOADER_BUILD

/**
 * This former inlined function must not be defined in the header file anymore.
 * As it depends on efuse component, any use of it outside of `bootloader_support`,
 * would require the caller component to include `efuse` as part of its `REQUIRES` or
 * `PRIV_REQUIRES` entries.
 * Attribute IRAM_ATTR must be specified for the app build.
 */
bool IRAM_ATTR esp_flash_encryption_enabled(void)
{
#ifndef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    return efuse_hal_flash_encryption_enabled();
#else
    uint32_t flash_crypt_cnt = 0;
#if CONFIG_IDF_TARGET_ESP32
    esp_efuse_read_field_blob(ESP_EFUSE_FLASH_CRYPT_CNT, &flash_crypt_cnt, ESP_EFUSE_FLASH_CRYPT_CNT[0]->bit_count);
#else
    esp_efuse_read_field_blob(ESP_EFUSE_SPI_BOOT_CRYPT_CNT, &flash_crypt_cnt, ESP_EFUSE_SPI_BOOT_CRYPT_CNT[0]->bit_count);
#endif
    /* __builtin_parity is in flash, so we calculate parity inline */
    bool enabled = false;
    while (flash_crypt_cnt) {
        if (flash_crypt_cnt & 1) {
            enabled = !enabled;
        }
        flash_crypt_cnt >>= 1;
    }
    return enabled;
#endif // CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
}

void esp_flash_write_protect_crypt_cnt(void)
{
    esp_efuse_write_field_bit(WR_DIS_CRYPT_CNT);
}

esp_flash_enc_mode_t esp_get_flash_encryption_mode(void)
{
    bool flash_crypt_cnt_wr_dis = false;
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
            bool dis_dl_cache = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_CACHE);
            bool dis_dl_enc = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_ENCRYPT);
            bool dis_dl_dec = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_DECRYPT);
            /* Check if DISABLE_DL_DECRYPT, DISABLE_DL_ENCRYPT & DISABLE_DL_CACHE are set */
            if ( dis_dl_cache && dis_dl_enc && dis_dl_dec ) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
            }
#else
            if (esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT)
#if CONFIG_IDF_TARGET_ESP32P4
                //TODO: IDF-7545
                && esp_efuse_read_field_bit(ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS)
#else
#if SOC_EFUSE_DIS_DOWNLOAD_ICACHE
                && esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE)
#endif
#if SOC_EFUSE_DIS_DOWNLOAD_DCACHE
                && esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE)
#endif
#endif
                ) {
                mode = ESP_FLASH_ENC_MODE_RELEASE;
#ifdef CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_128_DERIVED
                // This chip supports two types of key: AES128_DERIVED and AES128.
                // To be in RELEASE mode, it is important for the AES128_DERIVED key that XTS_KEY_LENGTH_256 be write-protected.
                bool xts_key_len_256_wr_dis = esp_efuse_read_field_bit(WR_DIS_CRYPT_CNT);
                mode = (xts_key_len_256_wr_dis) ? ESP_FLASH_ENC_MODE_RELEASE : ESP_FLASH_ENC_MODE_DEVELOPMENT;
#endif // CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_128_DERIVED
            }
#endif // !CONFIG_IDF_TARGET_ESP32
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
#else
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7545
    esp_efuse_write_field_bit(ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS);
#else
#if SOC_EFUSE_DIS_DOWNLOAD_ICACHE
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
#endif
#if SOC_EFUSE_DIS_DOWNLOAD_DCACHE
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);
#endif
#endif
#ifdef CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_128_DERIVED
    // For AES128_DERIVED, FE key is 16 bytes and XTS_KEY_LENGTH_256 is 0.
    // It is important to protect XTS_KEY_LENGTH_256 from further changing it to 1. Set write protection for this bit.
    // Burning WR_DIS_CRYPT_CNT, blocks further changing of eFuses: DIS_DOWNLOAD_MANUAL_ENCRYPT, SPI_BOOT_CRYPT_CNT, [XTS_KEY_LENGTH_256], SECURE_BOOT_EN.
    esp_efuse_write_field_bit(WR_DIS_CRYPT_CNT);
#endif // CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES_128_DERIVED
#endif // !CONFIG_IDF_TARGET_ESP32

#ifdef CONFIG_IDF_TARGET_ESP32
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_DIS_CACHE);
#else
#if SOC_EFUSE_DIS_ICACHE
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_DIS_ICACHE);
#endif
#endif // !CONFIG_IDF_TARGET_ESP32

#if CONFIG_SOC_SUPPORTS_SECURE_DL_MODE
    esp_efuse_enable_rom_secure_download_mode();
#else
    esp_efuse_disable_rom_download_mode();
#endif
    esp_efuse_batch_write_commit();

    if (esp_get_flash_encryption_mode() != ESP_FLASH_ENC_MODE_RELEASE) {
        ESP_LOGE(TAG, "Flash encryption mode is DEVELOPMENT, abort..");
        abort();
    }
    ESP_LOGI(TAG, "Flash encryption mode is RELEASE");
}

#ifdef CONFIG_IDF_TARGET_ESP32
bool esp_flash_encryption_cfg_verify_release_mode(void)
{
    bool result = false;
    bool secure;

    secure = esp_flash_encryption_enabled();
    result = secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not enabled Flash Encryption (FLASH_CRYPT_CNT->1 or max)");
    }

    uint8_t crypt_config = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_ENCRYPT_CONFIG, &crypt_config, 4);
    if (crypt_config != EFUSE_FLASH_CRYPT_CONFIG) {
        result &= false;
        ESP_LOGW(TAG, "ENCRYPT_CONFIG must be set 0xF (set ENCRYPT_CONFIG->0xF)");
    }

    uint8_t flash_crypt_cnt = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_FLASH_CRYPT_CNT, &flash_crypt_cnt, ESP_EFUSE_FLASH_CRYPT_CNT[0]->bit_count);
    if (flash_crypt_cnt != (1 << (ESP_EFUSE_FLASH_CRYPT_CNT[0]->bit_count)) - 1) {
        if (!esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT)) {
            result &= false;
            ESP_LOGW(TAG, "Not release mode of Flash Encryption (set FLASH_CRYPT_CNT->max or WR_DIS_FLASH_CRYPT_CNT->1)");
        }
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_ENCRYPT);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader encryption (set DISABLE_DL_ENCRYPT->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_DECRYPT);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader decryption (set DISABLE_DL_DECRYPT->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_DL_CACHE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader MMU cache (set DISABLE_DL_CACHE->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_DISABLE_JTAG);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled JTAG (set DISABLE_JTAG->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_CONSOLE_DEBUG_DISABLE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled ROM BASIC interpreter fallback (set CONSOLE_DEBUG_DISABLE->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_DIS_CACHE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not write-protected DIS_CACHE (set WR_DIS_DIS_CACHE->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_BLK1);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not read-protected flash ecnryption key (set RD_DIS_BLK1->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_BLK1);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not write-protected flash ecnryption key (set WR_DIS_BLK1->1)");
    }
    return result;
}
#else // not CONFIG_IDF_TARGET_ESP32
bool esp_flash_encryption_cfg_verify_release_mode(void)
{
    bool result = false;
    bool secure;

    secure = esp_flash_encryption_enabled();
    result = secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not enabled Flash Encryption (SPI_BOOT_CRYPT_CNT->1 or max)");
    }

    uint8_t flash_crypt_cnt = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_SPI_BOOT_CRYPT_CNT, &flash_crypt_cnt, ESP_EFUSE_SPI_BOOT_CRYPT_CNT[0]->bit_count);
    if (flash_crypt_cnt != (1 << (ESP_EFUSE_SPI_BOOT_CRYPT_CNT[0]->bit_count)) - 1) {
        if (!esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT)) {
            result &= false;
            ESP_LOGW(TAG, "Not release mode of Flash Encryption (set SPI_BOOT_CRYPT_CNT->max or WR_DIS_SPI_BOOT_CRYPT_CNT->1)");
        }
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader encryption (set DIS_DOWNLOAD_MANUAL_ENCRYPT->1)");
    }

#if SOC_EFUSE_DIS_DOWNLOAD_DCACHE
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader Dcache (set DIS_DOWNLOAD_DCACHE->1)");
    }
#endif

#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7545
    secure = esp_efuse_read_field_bit(ESP_EFUSE_SPI_DOWNLOAD_MSPI_DIS);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader download mspi (set DIS_DOWNLOAD_MSPI->1)");
    }
#else
#if SOC_EFUSE_DIS_DOWNLOAD_ICACHE
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled UART bootloader cache (set DIS_DOWNLOAD_ICACHE->1)");
    }
#endif
#endif

#if SOC_EFUSE_DIS_PAD_JTAG
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled JTAG PADs (set DIS_PAD_JTAG->1)");
    }
#endif

#if SOC_EFUSE_DIS_USB_JTAG
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_USB_JTAG);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled USB JTAG (set DIS_USB_JTAG->1)");
    }
#endif

#if SOC_EFUSE_DIS_DIRECT_BOOT
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DIRECT_BOOT);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled direct boot mode (set DIS_DIRECT_BOOT->1)");
    }
#endif

#if SOC_EFUSE_HARD_DIS_JTAG
    secure = esp_efuse_read_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled JTAG (set HARD_DIS_JTAG->1)");
    }
#endif

#if SOC_EFUSE_DIS_BOOT_REMAP
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled boot from RAM (set DIS_BOOT_REMAP->1)");
    }
#endif

#if SOC_EFUSE_DIS_LEGACY_SPI_BOOT
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled Legcy SPI boot (set DIS_LEGACY_SPI_BOOT->1)");
    }
#endif

#if SOC_EFUSE_DIS_ICACHE
    secure = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_DIS_ICACHE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not write-protected DIS_ICACHE (set WR_DIS_DIS_ICACHE->1)");
    }
#endif

    esp_efuse_purpose_t purposes[] = {
#if SOC_FLASH_ENCRYPTION_XTS_AES_256
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1,
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2,
#endif
#if SOC_FLASH_ENCRYPTION_XTS_AES_128
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY,
#endif
    };
    // S2 and S3 chips have both XTS_AES_128_KEY and XTS_AES_256_KEY_1/2.
    // The check below does not take into account that XTS_AES_128_KEY and XTS_AES_256_KEY_1/2
    // are mutually exclusive because this will make the chip not functional.
    // Only one type key must be configured in eFuses.
    secure = false;
    for (unsigned i = 0; i < sizeof(purposes) / sizeof(esp_efuse_purpose_t); i++) {
        esp_efuse_block_t block;
        if (esp_efuse_find_purpose(purposes[i], &block)) {
            secure = esp_efuse_get_key_dis_read(block);
            result &= secure;
            if (!secure) {
                ESP_LOGW(TAG, "Not read-protected Flash encryption key in BLOCK%d (set RD_DIS_KEY%d->1)", block, block - EFUSE_BLK_KEY0);
            }
            secure = esp_efuse_get_key_dis_write(block);
            result &= secure;
            if (!secure) {
                ESP_LOGW(TAG, "Not write-protected Flash encryption key in BLOCK%d (set WR_DIS_KEY%d->1)", block, block - EFUSE_BLK_KEY0);
            }

#if SOC_EFUSE_KEY_PURPOSE_FIELD
            secure = esp_efuse_get_keypurpose_dis_write(block);
            result &= secure;
            if (!secure) {
                ESP_LOGW(TAG, "Not write-protected KEY_PURPOSE for BLOCK%d (set WR_DIS_KEY_PURPOSE%d->1)", block, block - EFUSE_BLK_KEY0);
            }
#endif
        }
    }
    result &= secure;

    return result;
}
#endif // not CONFIG_IDF_TARGET_ESP32
