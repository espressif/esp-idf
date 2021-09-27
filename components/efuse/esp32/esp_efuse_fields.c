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
#include "soc/syscon_reg.h"

const static char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip version from efuse
uint8_t esp_efuse_get_chip_ver(void)
{
    uint8_t eco_bit0, eco_bit1, eco_bit2;
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_REV1, &eco_bit0, 1);
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_REV2, &eco_bit1, 1);
    eco_bit2 = (REG_READ(SYSCON_DATE_REG) & 0x80000000) >> 31;
    uint32_t combine_value = (eco_bit2 << 2) | (eco_bit1 << 1) | eco_bit0;
    uint8_t chip_ver = 0;
    switch (combine_value) {
    case 0:
        chip_ver = 0;
        break;
    case 1:
        chip_ver = 1;
        break;
    case 3:
        chip_ver = 2;
        break;
    case 7:
        chip_ver = 3;
        break;
    default:
        chip_ver = 0;
        break;
    }
    return chip_ver;
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

void esp_efuse_write_random_key(uint32_t blk_wdata0_reg)
{
    uint32_t buf[8];
    uint8_t raw[24];

    if (esp_efuse_get_coding_scheme(EFUSE_BLK2) == EFUSE_CODING_SCHEME_NONE) {
        bootloader_fill_random(buf, sizeof(buf));
    } else { // 3/4 Coding Scheme
        bootloader_fill_random(raw, sizeof(raw));
        esp_err_t r = esp_efuse_utility_apply_34_encoding(raw, buf, sizeof(raw));
        (void) r;
        assert(r == ESP_OK);
    }

    ESP_LOGV(TAG, "Writing random values to address 0x%08x", blk_wdata0_reg);
    for (int i = 0; i < 8; i++) {
        ESP_LOGV(TAG, "EFUSE_BLKx_WDATA%d_REG = 0x%08x", i, buf[i]);
        REG_WRITE(blk_wdata0_reg + 4*i, buf[i]);
    }
    bzero(buf, sizeof(buf));
    bzero(raw, sizeof(raw));
}

// Permanently update values written to the efuse write registers
void esp_efuse_burn_new_values(void)
{
    esp_efuse_utility_burn_efuses();
}
