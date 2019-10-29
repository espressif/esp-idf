// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_secure_boot.h"
#include "esp_log.h"
#include "esp32s2beta/rom/secure_boot.h"

#define TAG "secure_boot"

esp_err_t esp_secure_boot_permanently_enable(void)
{
    uint8_t hash[32];

    if (ets_efuse_secure_boot_enabled())
    {
        ESP_LOGI(TAG, "secure boot is already enabled, continuing..");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Verifying bootloader signature...\n");
    int r = ets_secure_boot_verify_bootloader(hash, false);
    if (r != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify bootloader signature");
        return r;
    }

    ets_efuse_clear_program_registers();
    REG_SET_BIT(EFUSE_PGM_DATA3_REG, EFUSE_SECURE_BOOT_EN);
    ets_efuse_program(ETS_EFUSE_BLOCK0);

    assert(ets_efuse_secure_boot_enabled());
    ESP_LOGI(TAG, "Secure boot permanently enabled");

    return ESP_OK;
}
