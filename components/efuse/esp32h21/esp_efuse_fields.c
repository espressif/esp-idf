/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_table.h"
#include "stdlib.h"
#include "esp_types.h"
#include "assert.h"
#include "hal/efuse_ll.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/efuse_periph.h"
#include "sys/param.h"

static __attribute__((unused)) const char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip package from efuse
uint32_t esp_efuse_get_pkg_ver(void)
{
    uint32_t pkg_ver = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_PKG_VERSION, &pkg_ver, ESP_EFUSE_PKG_VERSION[0]->bit_count);
    return pkg_ver;
}


esp_err_t esp_efuse_set_rom_log_scheme(esp_efuse_rom_log_scheme_t log_scheme)
{
    int cur_log_scheme = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_UART_PRINT_CONTROL, &cur_log_scheme, 2);
    if (!cur_log_scheme) { // not burned yet
        return esp_efuse_write_field_blob(ESP_EFUSE_UART_PRINT_CONTROL, &log_scheme, 2);
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t esp_efuse_disable_rom_download_mode(void)
{
    return esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE);
}

esp_err_t esp_efuse_enable_rom_secure_download_mode(void)
{
    if (esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE)) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_efuse_write_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
}

#if SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED
bool esp_efuse_is_ecdsa_p192_curve_supported(void)
{
    uint32_t current_curve = efuse_ll_get_ecdsa_curve_mode();
    return (current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_BOTH_P192_P256_BIT || current_curve == ESP_EFUSE_ECDSA_CURVE_MODE_ALLOW_ONLY_P192_BIT);
}

esp_err_t esp_efuse_enable_ecdsa_p192_curve_mode(void)
{
    esp_err_t err;
    uint8_t current_curve, next_curve;

    current_curve = efuse_ll_get_ecdsa_curve_mode();
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

    return ESP_OK;
}
#endif /* SOC_ECDSA_P192_CURVE_DEFAULT_DISABLED */
