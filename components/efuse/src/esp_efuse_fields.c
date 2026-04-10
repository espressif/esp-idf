/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_table.h"
#include "esp_types.h"
#include "assert.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_fault.h"
#include "esp_log.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "soc/efuse_periph.h"
#include "sys/param.h"
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"

#ifdef SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_ll.h"
#endif /* SOC_ECDSA_SUPPORTED */

ESP_LOG_ATTR_TAG(TAG, "efuse");

#ifdef CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD
#define APP_SEC_VER_SIZE_EFUSE_FIELD CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD
#else
#define APP_SEC_VER_SIZE_EFUSE_FIELD 4 // smallest possible size for all chips
#endif

// Reset efuse write registers
void esp_efuse_reset(void)
{
    esp_efuse_utility_reset();
}

uint32_t esp_efuse_read_secure_version(void)
{
    uint32_t secure_version = 0;
    size_t field_size = esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION);
    size_t size = MIN(APP_SEC_VER_SIZE_EFUSE_FIELD, field_size);
    esp_efuse_read_field_blob(ESP_EFUSE_SECURE_VERSION, &secure_version, size);
    return __builtin_popcount(secure_version & ((1ULL << size) - 1));
}

bool esp_efuse_check_secure_version(uint32_t secure_version)
{
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    /* Additional copies for Anti FI check */
    uint32_t sec_ver_hw_c1 = esp_efuse_read_secure_version();
    uint32_t sec_ver_hw_c2 = esp_efuse_read_secure_version();
    ESP_FAULT_ASSERT(sec_ver_hw == sec_ver_hw_c1);
    ESP_FAULT_ASSERT(sec_ver_hw == sec_ver_hw_c2);

    bool ret_status = (secure_version >= sec_ver_hw);
    /* Anti FI check */
    ESP_FAULT_ASSERT(ret_status == (secure_version >= sec_ver_hw));
    return ret_status;
}

esp_err_t esp_efuse_update_secure_version(uint32_t secure_version)
{
    size_t field_size = esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION);
    size_t size = MIN(APP_SEC_VER_SIZE_EFUSE_FIELD, field_size);
    if (size < secure_version) {
        ESP_LOGE(TAG, "Max secure version is %u. Given %"PRIu32" version can not be written.", (unsigned)size, secure_version);
        return ESP_ERR_INVALID_ARG;
    }
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(ESP_EFUSE_SECURE_VERSION_NUM_BLOCK);
    if (coding_scheme != EFUSE_CODING_SCHEME_NONE) {
        ESP_LOGE(TAG, "Anti rollback is not supported with any coding scheme.");
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    // If secure_version is the same as in eFuse field than it is ok just go out.
    if (sec_ver_hw < secure_version) {
        uint32_t num_bit_hw = (1ULL << sec_ver_hw) - 1;
        uint32_t num_bit_app = (1ULL << secure_version) - 1;
        // Repeated programming of programmed bits is strictly forbidden
        uint32_t new_bits = num_bit_app - num_bit_hw; // get only new bits
        esp_efuse_write_field_blob(ESP_EFUSE_SECURE_VERSION, &new_bits, size);
        ESP_LOGI(TAG, "Anti-rollback is set. eFuse field is updated(%"PRIu32").", secure_version);
    } else if (sec_ver_hw > secure_version) {
        ESP_LOGE(TAG, "Anti-rollback is not set. secure_version of app is lower that eFuse field(%"PRIu32").", sec_ver_hw);
        return ESP_FAIL;
    }
    return ESP_OK;
}

#if SOC_ECDSA_SUPPORTED
bool esp_efuse_is_ecdsa_p192_curve_supported(void)
{
#if SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED
    if (ecdsa_ll_is_configurable_curve_supported()) {
        uint32_t current_curve = efuse_hal_get_ecdsa_curve_mode();
        return (current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_BOTH_P192_P256_BIT || current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_ONLY_P192_BIT);
    } else {
        return true;
    }
#else
    return true;
#endif /* SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED */
}

bool esp_efuse_is_ecdsa_p256_curve_supported(void)
{
#if SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED
    if (ecdsa_ll_is_configurable_curve_supported()) {
        uint32_t current_curve = efuse_hal_get_ecdsa_curve_mode();
        return (current_curve != ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_ONLY_P192_BIT);
    } else {
        return true;
    }
#else
    return true;
#endif /* SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED */
}
#endif /* SOC_ECDSA_SUPPORTED */

#if SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED
esp_err_t esp_efuse_enable_ecdsa_p192_curve_mode(void)
{

    if (ecdsa_ll_is_configurable_curve_supported()) {
        esp_err_t err;
        uint8_t current_curve, next_curve;

        current_curve = efuse_hal_get_ecdsa_curve_mode();
        // Check if already in desired state
        if (current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_BOTH_P192_P256_BIT || current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_ONLY_P192_BIT) {
            ESP_EARLY_LOGD(TAG, "ECDSA P-192 curve mode is already enabled");
            return ESP_OK;
        }

        // Check if write is disabled or already locked to P256
        if (esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_ECDSA_CURVE_MODE) || current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_ONLY_P256_BIT_LOCKED) {
            ESP_EARLY_LOGE(TAG, "ECDSA curve mode is locked, cannot enable P-192 curve");
            return ESP_FAIL;
        }

        // Attempt to write new curve mode
        next_curve = ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_BOTH_P192_P256_BIT;
        err = esp_efuse_write_field_blob(ESP_EFUSE_ECDSA_CURVE_MODE, &next_curve, ESP_EFUSE_ECDSA_CURVE_MODE[0]->bit_count);
        if (err != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Failed to enable ECDSA P-192 curve %d", err);
            return err;
        }
    }
    return ESP_OK;
}
#endif /* SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED */

#if SOC_RECOVERY_BOOTLOADER_SUPPORTED
esp_err_t esp_efuse_set_recovery_bootloader_offset(const uint32_t offset)
{
    // The eFuse field stores the sector number instead of the full address to conserve eFuse bits.
    if (efuse_ll_get_recovery_bootloader_sector() == 0) {
        ESP_LOGI(TAG, "Recovery bootloader offset has not been set yet.");
        uint32_t recovery_flash_sector = efuse_hal_convert_recovery_bootloader_address_to_flash_sectors(offset);
        ESP_RETURN_ON_FALSE((recovery_flash_sector & ((1U << EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_LEN) - 1)) == recovery_flash_sector, ESP_ERR_INVALID_ARG, TAG,
            "Given address exceeds the allowed range of the efuse field");

        size_t recovery_flash_sector_len = esp_efuse_get_field_size(ESP_EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR);
        assert(recovery_flash_sector_len == EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_LEN);
        ESP_RETURN_ON_ERROR(esp_efuse_write_field_blob(ESP_EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR, &recovery_flash_sector, recovery_flash_sector_len), TAG,
            "Failed to burn recovery bootloader offset to eFuse");

    } else if (!efuse_hal_recovery_bootloader_enabled()) {
        ESP_LOGE(TAG, "Recovery bootloader offset is disabled");
        return ESP_ERR_NOT_ALLOWED;
    }

    uint32_t programmed_offset = efuse_hal_get_recovery_bootloader_address();
    if (programmed_offset != offset) {
        ESP_LOGE(TAG, "Verification failed. eFuse recovery bootloader offset=0x%" PRIx32 ", expected=0x%" PRIx32, programmed_offset, offset);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Recovery bootloader offset in eFuse = 0x%" PRIx32, programmed_offset);
    return ESP_OK;
}
#endif // SOC_RECOVERY_BOOTLOADER_SUPPORTED

bool IRAM_ATTR esp_efuse_is_flash_encryption_enabled(void)
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
