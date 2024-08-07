/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/startup_internal.h"
#include "sdkconfig.h"
#include "esp_crypto_clk.h"
#include "esp_security_priv.h"

ESP_SYSTEM_INIT_FN(esp_security_init, SECONDARY, BIT(0), 103)
{
    esp_crypto_clk_init();
#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_AT_STARTUP
    esp_crypto_dpa_protection_startup();
#endif
    return ESP_OK;
}

void esp_security_init_include_impl(void)
{
    // Linker hook, exists for no other purpose
}
