/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
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
#include "hal/efuse_hal.h"
#include "bootloader_random.h"
#include "sys/param.h"
#include "soc/syscon_reg.h"

const static char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip version from efuse
uint8_t esp_efuse_get_chip_ver(void)
{
    return efuse_hal_get_chip_revision();
}

// Returns chip package from efuse
uint32_t esp_efuse_get_pkg_ver(void)
{
    uint32_t pkg_ver = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_PKG, &pkg_ver, 4);
    return pkg_ver;
}

// Disable BASIC ROM Console via efuse
void esp_efuse_disable_basic_rom_console(void)
{
    if (!esp_efuse_read_field_bit(ESP_EFUSE_CONSOLE_DEBUG_DISABLE)) {
        esp_efuse_write_field_cnt(ESP_EFUSE_CONSOLE_DEBUG_DISABLE, 1);
        ESP_LOGI(TAG, "Disable BASIC ROM Console fallback via efuse...");
    }
}

esp_err_t esp_efuse_disable_rom_download_mode(void)
{
#ifndef CONFIG_ESP32_REV_MIN_3
    /* Check if we support this revision at all */
    if(esp_efuse_get_chip_ver() < 3) {
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif

    if (esp_efuse_read_field_bit(ESP_EFUSE_UART_DOWNLOAD_DIS)) {
        return ESP_OK;
    }

    /* WR_DIS_FLASH_CRYPT_CNT also covers UART_DOWNLOAD_DIS on ESP32 */
    if(esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT)) {
        return ESP_ERR_INVALID_STATE;
    }

    return esp_efuse_write_field_bit(ESP_EFUSE_UART_DOWNLOAD_DIS);
}

esp_err_t esp_efuse_set_rom_log_scheme(esp_efuse_rom_log_scheme_t log_scheme)
{
    return ESP_ERR_NOT_SUPPORTED;
}
