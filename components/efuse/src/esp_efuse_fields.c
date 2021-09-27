/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_efuse_table.h"
#include "esp_types.h"
#include "assert.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/efuse_periph.h"
#include "bootloader_random.h"
#include "soc/syscon_reg.h"
#include "sys/param.h"

static __attribute__((unused)) const char *TAG = "efuse";

#ifdef CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD
#define APP_SEC_VER_SIZE_EFUSE_FIELD CONFIG_BOOTLOADER_APP_SEC_VER_SIZE_EFUSE_FIELD
#else
#define APP_SEC_VER_SIZE_EFUSE_FIELD 16 // smallest possible size for all chips
#endif

// Reset efuse write registers
void esp_efuse_reset(void)
{
    esp_efuse_utility_reset();
}

uint32_t esp_efuse_read_secure_version(void)
{
    uint32_t secure_version = 0;
    size_t size = MIN(APP_SEC_VER_SIZE_EFUSE_FIELD, esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION));
    esp_efuse_read_field_blob(ESP_EFUSE_SECURE_VERSION, &secure_version, size);
    return __builtin_popcount(secure_version & ((1ULL << size) - 1));
}

bool esp_efuse_check_secure_version(uint32_t secure_version)
{
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    return secure_version >= sec_ver_hw;
}

esp_err_t esp_efuse_update_secure_version(uint32_t secure_version)
{
    size_t size = MIN(APP_SEC_VER_SIZE_EFUSE_FIELD, esp_efuse_get_field_size(ESP_EFUSE_SECURE_VERSION));
    if (size < secure_version) {
        ESP_LOGE(TAG, "Max secure version is %d. Given %d version can not be written.", size, secure_version);
        return ESP_ERR_INVALID_ARG;
    }
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(ESP_EFUSE_SECURE_VERSION_NUM_BLOCK);
    if (coding_scheme != EFUSE_CODING_SCHEME_NONE) {
        ESP_LOGE(TAG, "Anti rollback is not supported with any coding scheme.");
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint32_t sec_ver_hw = esp_efuse_read_secure_version();
    // If secure_version is the same as in eFuse field than it is ok just go out.
    if (sec_ver_hw < secure_version) {
        uint32_t num_bit_hw = (1ULL << sec_ver_hw) - 1;
        uint32_t num_bit_app = (1ULL << secure_version) - 1;
        // Repeated programming of programmed bits is strictly forbidden
        uint32_t new_bits = num_bit_app - num_bit_hw; // get only new bits
        esp_efuse_write_field_blob(ESP_EFUSE_SECURE_VERSION, &new_bits, size);
        ESP_LOGI(TAG, "Anti-rollback is set. eFuse field is updated(%d).", secure_version);
    } else if (sec_ver_hw > secure_version) {
        ESP_LOGE(TAG, "Anti-rollback is not set. secure_version of app is lower that eFuse field(%d).", sec_ver_hw);
        return ESP_FAIL;
    }
    return ESP_OK;
}
