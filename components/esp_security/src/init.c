/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/startup_internal.h"
#include "sdkconfig.h"
#include "esp_crypto_clk.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_security_priv.h"
#include "esp_err.h"

__attribute__((unused)) static const char *TAG = "esp_security";

ESP_SYSTEM_INIT_FN(esp_security_init, SECONDARY, BIT(0), 103)
{
    esp_crypto_clk_init();
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
