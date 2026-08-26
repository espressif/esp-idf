/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/startup_internal.h"
#include "psa/crypto.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

void mbedtls_psa_crypto_init_include_impl(void);

/**
 * @brief Initialize PSA Crypto library at system startup
 *
 * This function is called during the SECONDARY initialization stage with priority 104,
 * which ensures it runs after esp_security_init (priority 103). This ordering guarantees
 * that hardware crypto support is fully initialized before PSA crypto initialization.
 */
ESP_SYSTEM_INIT_FN(mbedtls_psa_crypto_init_fn, SECONDARY, BIT(0), 104)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

void mbedtls_psa_crypto_init_include_impl(void)
{
    // Linker hook, exists for no other purpose
}
