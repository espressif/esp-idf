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
#include "esp_secure_boot.h"

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

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

static void rsa_check_signature_on_update_check(void)
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
#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

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


#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
    rsa_check_signature_on_update_check();
#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

}
#endif // not BOOTLOADER_BUILD
