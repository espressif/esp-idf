/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY || SOC_KEY_MANAGER_FE_KEY_DEPLOY
#include "hal/key_mgr_ll.h"
#endif

__attribute__((unused)) static const char *TAG = "esp_security";

static void esp_key_mgr_init(void)
{
    // The following code initializes the key manager.
#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY || SOC_KEY_MANAGER_FE_KEY_DEPLOY
    // Enable key manager clock
    // Using ll APIs which do not require critical section
    _key_mgr_ll_enable_bus_clock(true);
    _key_mgr_ll_enable_peripheral_clock(true);
    _key_mgr_ll_reset_register();
    while (key_mgr_ll_get_state() != ESP_KEY_MGR_STATE_IDLE) {
    };
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY || SOC_KEY_MANAGER_FE_KEY_DEPLOY */
}

ESP_SYSTEM_INIT_FN(esp_security_init, SECONDARY, BIT(0), 103)
{
    esp_crypto_clk_init();
    esp_key_mgr_init();
#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_AT_STARTUP
    esp_crypto_dpa_protection_startup();
#endif

#ifdef CONFIG_ESP_CRYPTO_FORCE_ECC_CONSTANT_TIME_POINT_MUL
    if (!esp_efuse_read_field_bit(ESP_EFUSE_ECC_FORCE_CONST_TIME)) {
        ESP_EARLY_LOGD(TAG, "Forcefully enabling ECC constant time operations");
        esp_err_t err = esp_efuse_write_field_bit(ESP_EFUSE_ECC_FORCE_CONST_TIME);
        if (err != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "Enabling ECC constant time operations forcefully failed.");
            return err;
        }
    }
#endif
    return ESP_OK;
}

void esp_security_init_include_impl(void)
{
    // Linker hook, exists for no other purpose
}
