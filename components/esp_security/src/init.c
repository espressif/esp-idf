/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/startup_internal.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_crypto_clk.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_security_priv.h"
#include "esp_err.h"
#include "hal/efuse_hal.h"

#if SOC_HUK_MEM_NEEDS_RECHARGE
#include "hal/huk_hal.h"
#endif

#if SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT
#include "hal/key_mgr_ll.h"
#include "hal/key_mgr_types.h"
#endif /* SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT */

#if SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED
#include "hal/ecdsa_ll.h"
#endif /* SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED */

__attribute__((unused)) static const char *TAG = "esp_security";

#if SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT
static void esp_key_mgr_init(void)
{
    // The following code initializes the key manager.
    // When Flash Encryption is already enabled, Key Manager is initialized by the
    // ROM, and when Flash Encryption is enabled during boot up, Key Manager is
    // initialized by the bootloader.
    if (!efuse_hal_flash_encryption_enabled()) {
        // Enable key manager clock
        key_mgr_ll_power_up();
        // Using ll APIs which do not require critical section
        _key_mgr_ll_enable_bus_clock(true);
        _key_mgr_ll_enable_peripheral_clock(true);
        _key_mgr_ll_reset_register();

        while (key_mgr_ll_get_state() != ESP_KEY_MGR_STATE_IDLE) {
        };

        // Force Key Manager to use eFuse key by-default for an XTS-AES operation.
        key_mgr_ll_set_key_usage(ESP_KEY_MGR_FLASH_XTS_AES_KEY, ESP_KEY_MGR_USE_EFUSE_KEY);
    }
}
#endif /* SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT */

ESP_SYSTEM_INIT_FN(esp_security_init, SECONDARY, BIT(0), 103)
{
    esp_crypto_clk_init();

#if SOC_KEY_MANAGER_SUPPORT_KEY_DEPLOYMENT
    esp_key_mgr_init();
#endif

#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_AT_STARTUP
    esp_crypto_dpa_protection_startup();
#endif

    esp_err_t err = ESP_FAIL;

#if CONFIG_ESP_CRYPTO_FORCE_ECC_CONSTANT_TIME_POINT_MUL
    bool force_constant_time = true;
#if CONFIG_IDF_TARGET_ESP32H2
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
        force_constant_time = false;
    }
#endif
    if (!esp_efuse_read_field_bit(ESP_EFUSE_ECC_FORCE_CONST_TIME) && force_constant_time) {
        ESP_EARLY_LOGD(TAG, "Forcefully enabling ECC constant time operations");
        err = esp_efuse_write_field_bit(ESP_EFUSE_ECC_FORCE_CONST_TIME);
        if (err != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Enabling ECC constant time operations forcefully failed.");
            return err;
        }
    }
#endif

#if CONFIG_ESP_ECDSA_ENABLE_P192_CURVE
    err = esp_efuse_enable_ecdsa_p192_curve_mode();
    if (err != ESP_OK) {
        return err;
    }
#endif

#if CONFIG_SECURE_BOOT_V2_ENABLED
// H2, H21
#if SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED
    // Also write protects the ECC_FORCE_CONST_TIME efuse bit.
    if (ecdsa_ll_is_configurable_curve_supported()) {
        err = esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_ECDSA_CURVE_MODE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write protect the ECDSA_CURVE_MODE efuse bit.");
            return err;
        }
    }
#endif

#if !CONFIG_SECURE_BOOT_SKIP_WRITE_PROTECTION_SCA
// C5
#if SOC_ECDSA_SUPPORT_CURVE_P384 && !CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS && !CONFIG_IDF_TARGET_ESP32P4
    // Since SECURE_BOOT_SHA384_EN, XTS_DPA_PSEUDO_LEVEL, and ECC_FORCE_CONST_TIME share the
    // same write-protection bit, these efuses should only be write-protected after all of
    // them have been programmed.
    // Note: ESP32-P4 lacks WR_DIS_SECURE_BOOT_SHA384_EN bit, so it relies on software protection
    // in the efuse write APIs (see esp_efuse_api.c) to prevent unauthorized programming of
    // SECURE_BOOT_SHA384_EN when Secure Boot using SHA-256 is enabled.
    err = esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_SECURE_BOOT_SHA384_EN);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write protect the SECURE_BOOT_SHA384_EN efuse bit.");
        return err;
    }
#endif
#endif
#endif

    err = ESP_OK;
    return err;
}

void esp_security_init_include_impl(void)
{
    // Linker hook, exists for no other purpose
}
