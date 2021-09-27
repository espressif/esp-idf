/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_table.h"
#include "stdlib.h"
#include "esp_types.h"
#include "assert.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/efuse_periph.h"
#include "bootloader_random.h"
#include "sys/param.h"

static __attribute__((unused)) const char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip version from efuse
uint8_t esp_efuse_get_chip_ver(void)
{
    // should return the same value as bootloader_common_get_chip_revision()
    uint32_t chip_ver = 0;
    // TODO: ESP32S2 does not have this field
    return chip_ver;
}

// Returns chip package from efuse
uint32_t esp_efuse_get_pkg_ver(void)
{
    uint32_t pkg_ver = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_PKG_VERSION, &pkg_ver, ESP_EFUSE_PKG_VERSION[0]->bit_count);
    return pkg_ver;
}

esp_err_t esp_efuse_disable_rom_download_mode(void)
{
    return esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE);
}

esp_err_t esp_efuse_set_rom_log_scheme(esp_efuse_rom_log_scheme_t log_scheme)
{
    int cur_log_scheme = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_UART_PRINT_CONTROL, &cur_log_scheme, ESP_EFUSE_UART_PRINT_CONTROL[0]->bit_count);
    if (!cur_log_scheme) { // not burned yet
        return esp_efuse_write_field_blob(ESP_EFUSE_UART_PRINT_CONTROL, &log_scheme, ESP_EFUSE_UART_PRINT_CONTROL[0]->bit_count);
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t esp_efuse_enable_rom_secure_download_mode(void)
{
    if (esp_efuse_read_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MODE)) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_efuse_write_field_bit(ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD);
}
