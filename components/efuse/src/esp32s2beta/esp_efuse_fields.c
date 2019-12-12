// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_table.h"
#include "stdlib.h"
#include "esp_types.h"
#include "esp32/rom/efuse.h"
#include "assert.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/efuse_periph.h"
#include "bootloader_random.h"
#include "sys/param.h"

const static char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip version from efuse
uint8_t esp_efuse_get_chip_ver(void)
{
    uint32_t chip_ver = 0;
    // TODO: ESP32S2BETA does not have this field
    return chip_ver;
}

// Returns chip package from efuse
uint32_t esp_efuse_get_pkg_ver(void)
{
    uint32_t pkg_ver = 0;
    // TODO: ESP32S2BETA does not have this field
    return pkg_ver;
}

// Disable BASIC ROM Console via efuse
void esp_efuse_disable_basic_rom_console(void)
{
    uint8_t dis_tiny_basic = 0;
    uint8_t dis_legacy_spi_boot = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_DIS_TINY_BASIC, &dis_tiny_basic, 1);
    esp_efuse_read_field_blob(ESP_EFUSE_DIS_LEGACY_SPI_BOOT, &dis_legacy_spi_boot, 1);
    if (dis_tiny_basic == 0 || dis_legacy_spi_boot == 0) {
        esp_efuse_write_field_cnt(ESP_EFUSE_DIS_TINY_BASIC, 1);
        esp_efuse_write_field_cnt(ESP_EFUSE_DIS_LEGACY_SPI_BOOT, 1);
        ESP_EARLY_LOGI(TAG, "Disable tiny basic console in ROM and Disable_Legcy_SPI_boot mode...");
    }
}

void esp_efuse_write_random_key(uint32_t blk_wdata0_reg)
{
    uint32_t buf[8];
    uint8_t raw[24];

    bootloader_fill_random(buf, sizeof(buf));

    ESP_LOGV(TAG, "Writing random values to address 0x%08x", blk_wdata0_reg);
    for (int i = 0; i < 8; i++) {
        ESP_LOGV(TAG, "EFUSE_BLKx_WDATA%d_REG = 0x%08x", i, buf[i]);
        REG_WRITE(blk_wdata0_reg + 4*i, buf[i]);
    }
    bzero(buf, sizeof(buf));
    bzero(raw, sizeof(raw));
}
