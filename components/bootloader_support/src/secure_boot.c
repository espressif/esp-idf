/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <strings.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_secure_boot.h"
#include "hal/efuse_hal.h"

#ifndef BOOTLOADER_BUILD
static __attribute__((unused)) const char *TAG = "secure_boot";

#ifdef CONFIG_SECURE_BOOT
static void efuse_batch_write_begin(bool *need_fix)
{
    if (*need_fix == false) {
        esp_efuse_batch_write_begin();
    }
    *need_fix = true;
}

static void update_efuses(bool need_fix, esp_err_t err)
{
    if (need_fix) {
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Can not be fixed (err=0x%x).", err);
            esp_efuse_batch_write_cancel();
        } else {
            err = esp_efuse_batch_write_commit();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error programming eFuses (err=0x%x)", err);
                return;
            } else {
                ESP_LOGI(TAG, "Fixed");
            }
        }
    }
}
#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
static esp_err_t secure_boot_v1_check(bool *need_fix)
{
    esp_err_t err = ESP_OK;
    esp_efuse_block_t block = EFUSE_BLK_SECURE_BOOT;
    if (!esp_efuse_get_key_dis_read(block)) {
        efuse_batch_write_begin(need_fix);
        ESP_LOGW(TAG, "eFuse BLOCK%d should not be readable. Fixing..", block);
        err = esp_efuse_set_key_dis_read(block);
    }
    if (!esp_efuse_get_key_dis_write(block)) {
        efuse_batch_write_begin(need_fix);
        ESP_LOGW(TAG, "eFuse BLOCK%d should not be writeable. Fixing..", block);
        if (err == ESP_OK) {
            err = esp_efuse_set_key_dis_write(block);
        }
    }
    return err;
}
#elif SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS == 1 && CONFIG_SECURE_BOOT_V2_ENABLED
static esp_err_t secure_boot_v2_check(bool *need_fix)
{
    esp_err_t err = ESP_OK;
    esp_efuse_block_t block = EFUSE_BLK_SECURE_BOOT;
#ifndef CONFIG_SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK
    if (esp_efuse_get_key_dis_read(block)) {
        ESP_LOGE(TAG, "eFuse BLOCK%d should be readable", block);
        abort();
        // This code is not achievable because the bootloader will not boot an app in this state.
        // But we keep it here just in case (any unexpected behavior).
    }
#endif
    if (esp_efuse_block_is_empty(block)) {
        ESP_LOGE(TAG, "eFuse BLOCK%d should not be empty", block);
        abort();
        // This code is not achievable because the bootloader will not boot an app in this state.
        // But we keep it here just in case (any unexpected behavior).
    }
    if (!esp_efuse_get_key_dis_write(block)) {
        efuse_batch_write_begin(need_fix);
        ESP_LOGW(TAG, "eFuse BLOCK%d should not be writeable. Fixing..", block);
        err = esp_efuse_set_key_dis_write(block);
    }
    return err;
}
#elif SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS > 1 && CONFIG_SECURE_BOOT_V2_ENABLED
static esp_err_t secure_boot_v2_check(bool *need_fix)
{
    esp_err_t err = ESP_OK;
    esp_efuse_purpose_t purpose[SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS] = {
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2,
    };

    for (unsigned i = 0; i < SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS; ++i) {
        esp_efuse_block_t block;
        if (esp_efuse_find_purpose(purpose[i], &block)) {
            if (!esp_efuse_get_digest_revoke(i)) {
                if (esp_efuse_get_key_dis_read(block)) {
                    ESP_LOGE(TAG, "eFuse BLOCK%d should be readable", block);
                    abort();
                    // This state is not expected unless the eFuses have been manually misconfigured.
                }
                if (esp_efuse_block_is_empty(block)) {
                    ESP_LOGE(TAG, "eFuse BLOCK%d should not be empty", block);
                    abort();
                    // This state is not expected unless the eFuses have been manually misconfigured.
                }
                if (!esp_efuse_get_key_dis_write(block)) {
                    efuse_batch_write_begin(need_fix);
                    ESP_LOGW(TAG, "eFuse BLOCK%d should not be writeable. Fixing..", block);
                    if (err == ESP_OK) {
                        err = esp_efuse_set_key_dis_write(block);
                    }
                }
            }
            if (!esp_efuse_get_keypurpose_dis_write(block)) {
                efuse_batch_write_begin(need_fix);
                ESP_LOGW(TAG, "The KEY_PURPOSE_SECURE_BOOT_DIGEST%d should be write-protected. Fixing..", block);
                if (err == ESP_OK) {
                    err = esp_efuse_set_keypurpose_dis_write(block);
                }
            }
        } else {
            if (!esp_efuse_get_digest_revoke(i)) {
#ifndef CONFIG_SECURE_BOOT_ALLOW_UNUSED_DIGEST_SLOTS
                efuse_batch_write_begin(need_fix);
                ESP_LOGW(TAG, "Unused SECURE_BOOT_DIGEST%d should be revoked. Fixing..", i);
                if (err == ESP_OK) {
                    err = esp_efuse_set_digest_revoke(i);
                }
#else
                ESP_LOGW(TAG, "Unused SECURE_BOOT_DIGEST%d should be revoked. It will not be fixed due to the config", i);
#endif
            }
        }
    }
    return err;
}
#endif
#endif // CONFIG_SECURE_BOOT

#if (CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME) && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

static void check_signature_on_update_check(void)
{
    // We rely on the keys used to sign this app to verify the next app on OTA, so make sure there is at
    // least one to avoid a stuck firmware
    esp_image_sig_public_key_digests_t digests = { 0 };

    esp_err_t err = esp_secure_boot_get_signature_blocks_for_running_app(false, &digests);

    if (err != ESP_OK || digests.num_digests == 0) {
        ESP_LOGE(TAG, "This app is not signed, but check signature on update is enabled in config. It won't be possible to verify any update.");
        abort();
    }
#if CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT && SECURE_BOOT_NUM_BLOCKS > 1
    if (digests.num_digests > 1) {
        ESP_LOGW(TAG, "App has %d signatures. Only the first position of signature blocks is used to verify any update", digests.num_digests);
    }
#endif
}
#endif // (CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME) && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

void esp_secure_boot_init_checks(void)
{
#ifdef CONFIG_SECURE_BOOT

    if (esp_secure_boot_enabled()) {
        bool need_fix = false;
#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
        esp_err_t err = secure_boot_v1_check(&need_fix);
#else
        esp_err_t err = secure_boot_v2_check(&need_fix);
#endif
        update_efuses(need_fix, err);
    } else {
        ESP_LOGE(TAG, "Mismatch in secure boot settings: the app config is enabled but eFuse not");
    }
#endif // CONFIG_SECURE_BOOT


#if (CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME) && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
    check_signature_on_update_check();
#endif // (CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME) && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

}

#ifdef CONFIG_IDF_TARGET_ESP32
bool esp_secure_boot_cfg_verify_release_mode(void)
{
    bool result = false;
    bool secure;

    bool secure_boot_v1 = esp_efuse_read_field_bit(ESP_EFUSE_ABS_DONE_0);
    bool chip_supports_sbv2 = efuse_hal_chip_revision() >= 300;
    bool secure_boot_v2 = (chip_supports_sbv2) ? esp_efuse_read_field_bit(ESP_EFUSE_ABS_DONE_1) : false;
    result = secure_boot_v1 || secure_boot_v2;
    if (secure_boot_v1 && secure_boot_v2) {
        ESP_LOGI(TAG, "ABS_DONE_0=1 (V1) and ABS_DONE_1=1 (V2)");
        ESP_LOGI(TAG, "Secure boot V2 shall take the precedence");
    } else if (!secure_boot_v1 && !secure_boot_v2) {
        result = false;
        ESP_LOGE(TAG, "Not enabled Secure Boot V1 (set ABS_DONE_0->1)");
        if (chip_supports_sbv2) {
            ESP_LOGE(TAG, "Not enabled Secure Boot V2 (set ABS_DONE_1->1)");
        }
    }

    if (secure_boot_v1 && !secure_boot_v2) {
        secure = esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_BLK2);
        result &= secure;
        if (!secure) {
            ESP_LOGW(TAG, "Not read-protected secure boot key (set RD_DIS_BLK2->1)");
        }
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_BLK2);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not write-protected secure boot key (set WR_DIS_BLK2->1)");
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

    if (secure_boot_v2) {
        secure = esp_efuse_read_field_bit(ESP_EFUSE_UART_DOWNLOAD_DIS);
        result &= secure;
        if (!secure) {
            ESP_LOGW(TAG, "Not disabled UART ROM Download mode (set UART_DOWNLOAD_DIS->1)");
        }

        secure = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_EFUSE_RD_DISABLE);
        result &= secure;
        if (!secure) {
            ESP_LOGW(TAG, "Not disabled write-protection for read-protection (set WR_DIS_EFUSE_RD_DISABLE->1)");
        }
    }

    return result;
}
#else // not CONFIG_IDF_TARGET_ESP32
bool esp_secure_boot_cfg_verify_release_mode(void)
{
    bool result = false;
    bool secure;

    secure = esp_secure_boot_enabled();
    result = secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not enabled Secure Boot (SECURE_BOOT_EN->1)");
    }

    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE);
    bool en_secure_download = esp_efuse_read_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
    if (!secure && !en_secure_download) {
        result &= false;
        ESP_LOGW(TAG, "Download mode has not been changed, disable it or set security mode:");
        ESP_LOGW(TAG, "Not disabled ROM Download mode (DIS_DOWNLOAD_MODE->1)");
        ESP_LOGW(TAG, "Not enabled Security download mode (ENABLE_SECURITY_DOWNLOAD->1)");
    }

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

#if SOC_EFUSE_DIS_DIRECT_BOOT
    secure = esp_efuse_read_field_bit(ESP_EFUSE_DIS_DIRECT_BOOT);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled direct boot mode (set DIS_DIRECT_BOOT->1)");
    }
#endif

    bool soft_dis_jtag_complete = false;
#if SOC_EFUSE_SOFT_DIS_JTAG
    size_t soft_dis_jtag_cnt_val = 0;
    esp_efuse_read_field_cnt(ESP_EFUSE_SOFT_DIS_JTAG, &soft_dis_jtag_cnt_val);
    soft_dis_jtag_complete = (soft_dis_jtag_cnt_val == ESP_EFUSE_SOFT_DIS_JTAG[0]->bit_count);
    if (soft_dis_jtag_complete) {
        bool hmac_key_found = false;
            hmac_key_found = esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG, NULL);
            hmac_key_found |= esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL, NULL);
        if (!hmac_key_found) {
            ESP_LOGW(TAG, "SOFT_DIS_JTAG is set but HMAC key with respective purpose not found");
            soft_dis_jtag_complete = false;
        }
    }
#endif

    if (!soft_dis_jtag_complete) {
#if SOC_EFUSE_HARD_DIS_JTAG
        secure = esp_efuse_read_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
        result &= secure;
        if (!secure) {
            ESP_LOGW(TAG, "Not disabled JTAG (set HARD_DIS_JTAG->1)");
        }
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
    }

#ifdef CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE
    secure = esp_efuse_read_field_bit(ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not enabled AGGRESSIVE KEY REVOKE (set SECURE_BOOT_AGGRESSIVE_REVOKE->1)");
    }
#endif

    secure = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
    result &= secure;
    if (!secure) {
        ESP_LOGW(TAG, "Not disabled write-protection for read-protection (set WR_DIS_RD_DIS->1)");
    }

#if SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS == 1
    unsigned purpose = ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2;
#else
    unsigned purpose = ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0; // DIGEST0, DIGEST1 and DIGEST2
#endif
    secure = false;
    unsigned num_keys = 0;
    for (unsigned i = 0; i < SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS; ++i) {
        esp_efuse_block_t block;
        if (esp_efuse_find_purpose(purpose + i, &block)) {
            // if chip has a few secure boot slots then we check all
#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
            bool revoke = esp_efuse_get_digest_revoke(i);
            if (revoke) {
                continue;
            }
#endif
            ++num_keys;
#if SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK
            secure = !esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_KEY0_HI);
#else
            secure = !esp_efuse_get_key_dis_read(block);
#endif // !SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK
            result &= secure;
            if (!secure) {
                ESP_LOGE(TAG, "Secure boot key in BLOCK%d must NOT be read-protected (can not be used)", block);
#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
                ESP_LOGE(TAG, "Revoke this secure boot key (set SECURE_BOOT_KEY_REVOKE%d->1)", i);
#endif
            }
            secure = !esp_efuse_block_is_empty(block);
            result &= secure;
            if (!secure) {
                ESP_LOGE(TAG, "Secure boot key in BLOCK%d must NOT be empty (can not be used)", block);
#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
                ESP_LOGE(TAG, "Revoke this secure boot key (set SECURE_BOOT_KEY_REVOKE%d->1)", i);
#endif
            }
            secure = esp_efuse_get_key_dis_write(block);
            result &= secure;
            if (!secure) {
                ESP_LOGW(TAG, "Not write-protected secure boot key in BLOCK%d (set WR_DIS_KEY%d->1)", block, block - EFUSE_BLK_KEY0);
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

    secure = (num_keys != 0);
    result &= secure;
    if (!secure) {
        ESP_LOGE(TAG, "No secure boot key found");
    }

    return result;
}
#endif // not CONFIG_IDF_TARGET_ESP32

#endif // not BOOTLOADER_BUILD
