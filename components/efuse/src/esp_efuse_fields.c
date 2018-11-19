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
#include "rom/efuse.h"
#include "assert.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/efuse_reg.h"
#include "bootloader_random.h"

const static char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip version from efuse
uint8_t esp_efuse_get_chip_ver(void)
{
    uint8_t chip_ver;
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_REV1, &chip_ver, 1);
    return chip_ver;
}

// Returns chip package from efuse
uint32_t esp_efuse_get_pkg_ver(void)
{
    uint32_t pkg_ver;
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_PKG, &pkg_ver, 3);
    return pkg_ver;
}

// Permanently update values written to the efuse write registers
void esp_efuse_burn_new_values(void)
{
    esp_efuse_utility_burn_efuses();
}

// Reset efuse write registers
void esp_efuse_reset(void)
{
    esp_efuse_utility_reset();
}

// Disable BASIC ROM Console via efuse
void esp_efuse_disable_basic_rom_console(void)
{
    if (esp_efuse_write_field_cnt(ESP_EFUSE_CONSOLE_DEBUG_DISABLE, 1) == ESP_OK) {
        ESP_EARLY_LOGI(TAG, "Disable BASIC ROM Console fallback via efuse...");
    }
}

esp_err_t esp_efuse_apply_34_encoding(const uint8_t *in_bytes, uint32_t *out_words, size_t in_bytes_len)
{
    if (in_bytes == NULL || out_words == NULL || in_bytes_len % 6 != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    while (in_bytes_len > 0) {
        uint8_t out[8];
        uint8_t xor = 0;
        uint8_t mul = 0;
        for (int i = 0; i < 6; i++) {
            xor ^= in_bytes[i];
            mul += (i + 1) * __builtin_popcount(in_bytes[i]);
        }

        memcpy(out, in_bytes, 6); // Data bytes
        out[6] = xor;
        out[7] = mul;

        memcpy(out_words, out, 8);

        in_bytes_len -= 6;
        in_bytes += 6;
        out_words += 2;
    }

    return ESP_OK;
}

void esp_efuse_write_random_key(uint32_t blk_wdata0_reg)
{
    uint32_t buf[8];
    uint8_t raw[24];
    uint32_t coding_scheme = REG_READ(EFUSE_BLK0_RDATA6_REG) & EFUSE_CODING_SCHEME_M;

    if (coding_scheme == EFUSE_CODING_SCHEME_VAL_NONE) {
        bootloader_fill_random(buf, sizeof(buf));
    } else { // 3/4 Coding Scheme
        bootloader_fill_random(raw, sizeof(raw));
        esp_err_t r = esp_efuse_apply_34_encoding(raw, buf, sizeof(raw));
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
