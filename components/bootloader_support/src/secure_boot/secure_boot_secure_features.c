/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Unified Secure Boot V2 secure-features implementation shared by all targets
 * except ESP32 (which additionally supports Secure Boot V1 and a distinct eFuse
 * family, handled in src/esp32/secure_boot_secure_features.c). All per-target
 * differences are expressed through SOC_EFUSE_* capabilities rather than
 * per-target files.
 */

#include <strings.h>
#include "esp_secure_boot.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

ESP_LOG_ATTR_TAG(TAG, "secure_boot");

esp_err_t esp_secure_boot_enable_secure_features(void)
{
#if SOC_EFUSE_DIS_BOOT_REMAP
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);
#else
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DIRECT_BOOT);
#endif

#ifdef CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    ESP_LOGI(TAG, "Enabling Security download mode...");
    esp_err_t err = esp_efuse_enable_rom_secure_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not enable Security download mode...");
        return err;
    }
#elif CONFIG_SECURE_DISABLE_ROM_DL_MODE
    ESP_LOGI(TAG, "Disable ROM Download mode...");
    esp_err_t err = esp_efuse_disable_rom_download_mode();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not disable ROM Download mode...");
        return err;
    }
#else
    ESP_LOGW(TAG, "UART ROM Download mode kept enabled - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable hardware & software JTAG...");
#if SOC_EFUSE_HARD_DIS_JTAG
    esp_efuse_write_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
#endif
#if SOC_EFUSE_DIS_PAD_JTAG
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
#endif
#if SOC_EFUSE_DIS_USB_JTAG
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_USB_JTAG);
#endif
#if SOC_EFUSE_SOFT_DIS_JTAG
    esp_efuse_write_field_cnt(ESP_EFUSE_SOFT_DIS_JTAG, ESP_EFUSE_SOFT_DIS_JTAG[0]->bit_count);
#endif
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE);
#endif
#endif

#if SOC_ECDSA_SUPPORT_CURVE_P384
#if CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS
    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_SHA384_EN);
#endif
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_SECURE_BOOT_EN);

#ifndef CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS
    bool rd_dis_now = true;
    // Secure boot and Flash encryption share one eFuse key block so they can not be set separately.
    // CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER option is used to burn SB and FE at the same time.
    // SB key is readable, the corresponding bit in RD_DIS is unset.
    //   We set write-protection for RD_DIS to ensure that the SB key is always readable.
    // FE key is read-protected, the corresponding bit in RD_DIS is set.
    // Hence RD_DIS is write-protected here unconditionally and the flash-encryption path
    // does not repeat this step.
#if !defined(CONFIG_SECURE_BOOT_FLASH_ENC_KEYS_BURN_TOGETHER) && defined(CONFIG_SECURE_FLASH_ENC_ENABLED)
    /* If flash encryption is not enabled yet then don't read-disable efuses yet, do it later in the boot
       when Flash Encryption is being enabled */
    rd_dis_now = esp_efuse_is_flash_encryption_enabled();
#endif
    if (rd_dis_now) {
        ESP_LOGI(TAG, "Prevent read disabling of additional efuses...");
        esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
    }
#else
    ESP_LOGW(TAG, "Allowing read disabling of additional efuses - SECURITY COMPROMISED");
#endif

    return ESP_OK;
}
