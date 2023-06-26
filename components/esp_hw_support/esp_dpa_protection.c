/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "soc/hp_system_reg.h"
#include "esp_dpa_protection.h"

static inline void esp_crypto_dpa_set_level(esp_crypto_dpa_sec_level_t level)
{
    assert(level >= ESP_CRYPTO_DPA_SEC_LEVEL_LOW && level <= ESP_CRYPTO_DPA_SEC_LEVEL_HIGH);
    REG_SET_BIT(HP_SYSTEM_SEC_DPA_CONF_REG, HP_SYSTEM_SEC_DPA_CFG_SEL);
    REG_SET_FIELD(HP_SYSTEM_SEC_DPA_CONF_REG, HP_SYSTEM_SEC_DPA_LEVEL, level);
}

#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_AT_STARTUP
static void __attribute__((constructor)) esp_crypto_dpa_protection_startup(void)
{
    esp_crypto_dpa_set_level(CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL);
}
#endif

void esp_crypto_dpa_protection_enable(esp_crypto_dpa_sec_level_t level)
{
    esp_crypto_dpa_set_level(level);
}

void esp_crypto_dpa_protection_disable(void)
{
    REG_CLR_BIT(HP_SYSTEM_SEC_DPA_CONF_REG, HP_SYSTEM_SEC_DPA_CFG_SEL);
}

void esp_crypto_dpa_prot_include_impl(void)
{
    // Linker hook, exists for no other purpose
}
