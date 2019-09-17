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
#include "soc/apb_ctrl_reg.h"

const static char *TAG = "efuse";

// Contains functions that provide access to efuse fields which are often used in IDF.

// Returns chip version from efuse
uint8_t esp_efuse_get_chip_ver(void)
{
    uint8_t eco_bit0, eco_bit1, eco_bit2;
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_REV1, &eco_bit0, 1);
    esp_efuse_read_field_blob(ESP_EFUSE_CHIP_VER_REV2, &eco_bit1, 1);
    eco_bit2 = (REG_READ(APB_CTRL_DATE_REG) & 0x80000000) >> 31;
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
        (void) r;
        assert(r == ESP_OK);
    }

    ESP_LOGV(TAG, "Writing random values to address 0x%08x", blk_wdata0_reg);
    for (int i = 0; i < 8; i++) {
        ESP_LOGV(TAG, "EFUSE_BLKx_WDATA%d_REG = 0x%08x", i, buf[i]);
        REG_WRITE(blk_wdata0_reg + 4 * i, buf[i]);
    }
    bzero(buf, sizeof(buf));
    bzero(raw, sizeof(raw));
}

#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE

#include "../include_bootloader/bootloader_flash.h"
#include "esp_flash_encrypt.h"

static uint32_t esp_efuse_flash_offset = 0;
static uint32_t esp_efuse_flash_size = 0;
void esp_efuse_init(uint32_t offset, uint32_t size)
{
    esp_efuse_flash_offset = offset;
    esp_efuse_flash_size = size;
}

static uint32_t emulate_secure_version_read()
{
    uint32_t secure_version;
    uint32_t offset = esp_efuse_flash_offset;
    if (offset == 0) {
        ESP_LOGE(TAG, "emulate secure_version can not be used");
        return 0;
    }
    const uint32_t *efuse_place_in_flash = bootloader_mmap(offset, esp_efuse_flash_size);
    if (!efuse_place_in_flash) {
        ESP_LOGE(TAG, "secure_version can not be read from (0x%x, 0x%x) flash", offset, esp_efuse_flash_size);
        return 0;
    }
    memcpy(&secure_version, efuse_place_in_flash, sizeof(uint32_t));
    bootloader_munmap(efuse_place_in_flash);
    secure_version = ~secure_version;
    ESP_LOGV(TAG, "Read 0x%08x secure_version from flash", secure_version);
    return secure_version;
}

static void emulate_secure_version_write(uint32_t secure_version)
{
    uint32_t secure_version_wr = ~secure_version;
    uint32_t offset = esp_efuse_flash_offset;
    if (offset == 0) {
        ESP_LOGE(TAG, "emulate secure_version can not be used");
        return;
    }
    esp_err_t err = bootloader_flash_write(offset, &secure_version_wr, sizeof(secure_version_wr), false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "secure_version can not be written to flash. err = 0x%x", err);
    }
    ESP_LOGV(TAG, "Write 0x%08x secure_version into flash", secure_version);
}
#endif

// This efuse register is used whole for secure version (32 bits).
#define EFUSE_BLK_RD_ANTI_ROLLBACK EFUSE_BLK3_RDATA4_REG
#define EFUSE_BLK_WR_ANTI_ROLLBACK EFUSE_BLK3_WDATA4_REG

uint32_t esp_efuse_read_secure_version()
{
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    uint32_t secure_version;

#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    secure_version = emulate_secure_version_read();
#else
    secure_version = REG_READ(EFUSE_BLK_RD_ANTI_ROLLBACK);
#endif // CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE

    return __builtin_popcount(secure_version & ((1ULL << CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD) - 1));
#else
    return 0;
#endif
}

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
static void write_anti_rollback(uint32_t new_bits)
{
#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    emulate_secure_version_write(new_bits);
#else
    esp_efuse_reset();
    REG_WRITE(EFUSE_BLK_WR_ANTI_ROLLBACK, new_bits);
    esp_efuse_burn_new_values();
#endif
}
#endif

bool esp_efuse_check_secure_version(uint32_t secure_version)
{
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    return secure_version >= sec_ver_hw;
}

esp_err_t esp_efuse_update_secure_version(uint32_t secure_version)
{
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    if (CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD < secure_version) {
        ESP_LOGE(TAG, "Max secure version is %d. Given %d version can not be written.", CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD, secure_version);
        return ESP_ERR_INVALID_ARG;
    }
#ifndef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    uint32_t coding_scheme = REG_READ(EFUSE_BLK0_RDATA6_REG) & EFUSE_CODING_SCHEME_M;
    if (coding_scheme != EFUSE_CODING_SCHEME_VAL_NONE) {
        ESP_LOGE(TAG, "Anti rollback is not supported with a 3/4 coding scheme.");
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    // If secure_version is the same as in eFuse field than it is ok just go out.
    if (sec_ver_hw < secure_version) {
        uint32_t num_bit_hw = (1ULL << sec_ver_hw) - 1;
        uint32_t num_bit_app = (1ULL << secure_version) - 1;
        // Repeated programming of programmed bits is strictly forbidden
        uint32_t new_bits = num_bit_app - num_bit_hw; // get only new bits
        write_anti_rollback(new_bits);
        ESP_LOGI(TAG, "Anti-rollback is set. eFuse field is updated(%d).", secure_version);
    } else if (sec_ver_hw > secure_version) {
        ESP_LOGE(TAG, "Anti-rollback is not set. secure_version of app is lower that eFuse field(%d).", sec_ver_hw);
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

