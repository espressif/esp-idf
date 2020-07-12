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

#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_table.h"
#include "esp_types.h"
#include "assert.h"
#include "esp_err.h"
#include "esp_fault.h"
#include "esp_log.h"
#include "soc/efuse_periph.h"
#include "bootloader_random.h"
#include "soc/apb_ctrl_reg.h"
#include "sys/param.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/efuse.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/efuse.h"
#endif

static __attribute__((unused)) const char *TAG = "efuse";
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

#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE

#include "../include_bootloader/bootloader_flash_priv.h"
#include "esp_flash_encrypt.h"

static uint32_t esp_efuse_flash_offset = 0;
static uint32_t esp_efuse_flash_size = 0;

void esp_efuse_init(uint32_t offset, uint32_t size)
{
    esp_efuse_flash_offset = offset;
    esp_efuse_flash_size = size;
}

static uint32_t emulate_secure_version_read(void)
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
#endif // CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE

uint32_t esp_efuse_read_secure_version(void)
{
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    uint32_t secure_version = 0;
    int size = esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION);
    size = MIN(CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD, size);
#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    secure_version = emulate_secure_version_read();
#else
    esp_efuse_read_field_blob(ESP_EFUSE_SECURE_VERSION, &secure_version, size);
#endif // CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE

    return __builtin_popcount(secure_version & ((1ULL << size) - 1));
#else
    return 0;
#endif
}

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
static void write_anti_rollback(uint32_t new_bits)
{
    int size = esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION);
    size = MIN(CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD, size);
#ifdef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    emulate_secure_version_write(new_bits);
#else
    esp_efuse_write_field_blob(ESP_EFUSE_SECURE_VERSION, &new_bits, size);
#endif
}
#endif

bool esp_efuse_check_secure_version(uint32_t secure_version)
{
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    /* Additional copies for Anti FI check */
    uint32_t sec_ver_hw_c1 = esp_efuse_read_secure_version();
    uint32_t sec_ver_hw_c2 = esp_efuse_read_secure_version();
    ESP_FAULT_ASSERT(sec_ver_hw == sec_ver_hw_c1);
    ESP_FAULT_ASSERT(sec_ver_hw == sec_ver_hw_c2);

    bool ret_status = (secure_version >= sec_ver_hw);
    /* Anti FI check */
    ESP_FAULT_ASSERT(ret_status == (secure_version >= sec_ver_hw));
    return ret_status;
}

esp_err_t esp_efuse_update_secure_version(uint32_t secure_version)
{
#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    if (CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD < secure_version) {
        ESP_LOGE(TAG, "Max secure version is %d. Given %d version can not be written.", CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD, secure_version);
        return ESP_ERR_INVALID_ARG;
    }
#ifndef CONFIG_BOOTLOADER_EFUSE_SECURE_VERSION_EMULATE
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(ESP_EFUSE_SECURE_VERSION_NUM_BLOCK);
    if (coding_scheme != EFUSE_CODING_SCHEME_NONE) {
        ESP_LOGE(TAG, "Anti rollback is not supported with any coding scheme.");
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

