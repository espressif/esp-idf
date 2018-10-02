// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_log.h"
#include <string.h>
#include "bootloader_random.h"

#define EFUSE_CONF_WRITE 0x5A5A /* efuse_pgm_op_ena, force no rd/wr disable */
#define EFUSE_CONF_READ 0x5AA5 /* efuse_read_op_ena, release force */

#define EFUSE_CMD_PGM 0x02
#define EFUSE_CMD_READ 0x01

static const char *TAG = "efuse";

void esp_efuse_burn_new_values(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_WRITE);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_PGM);
    while (REG_READ(EFUSE_CMD_REG) != 0) {
    }
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_READ);
    while (REG_READ(EFUSE_CMD_REG) != 0) {
    }
    esp_efuse_reset();
}

void esp_efuse_reset(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    const uint32_t block_start[4] = { EFUSE_BLK0_WDATA0_REG, EFUSE_BLK1_WDATA0_REG,
                                      EFUSE_BLK2_WDATA0_REG, EFUSE_BLK3_WDATA0_REG };
    const uint32_t block_end[4] = { EFUSE_BLK0_WDATA6_REG, EFUSE_BLK1_WDATA7_REG,
                                    EFUSE_BLK2_WDATA7_REG, EFUSE_BLK3_WDATA7_REG };
    for (int i = 0; i < 4; i++) {
      for (uint32_t r = block_start[i]; r <= block_end[i]; r+= 4) {
        REG_WRITE(r, 0);
      }
    }
}

void esp_efuse_disable_basic_rom_console(void)
{
    if ((REG_READ(EFUSE_BLK0_RDATA6_REG) & EFUSE_RD_CONSOLE_DEBUG_DISABLE) == 0) {
        ESP_EARLY_LOGI(TAG, "Disable BASIC ROM Console fallback via efuse...");
        esp_efuse_reset();
        REG_WRITE(EFUSE_BLK0_WDATA6_REG, EFUSE_RD_CONSOLE_DEBUG_DISABLE);
        esp_efuse_burn_new_values();
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
