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
#include "sdkconfig.h"

#include <string.h>
#include "esp_fault.h"
#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp32s2/rom/secure_boot.h"

static const char* TAG = "secure_boot";

#define DIGEST_LEN 32
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN];
    uint8_t verified_digest[DIGEST_LEN] = { 0 }; /* Note: this function doesn't do any anti-FI checks on this buffer */
    const uint8_t *data;

    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    /* Padding to round off the input to the nearest 4k boundary */
    int padded_length = ALIGN_UP(length, FLASH_SECTOR_SIZE);
    ESP_LOGD(TAG, "verifying src_addr 0x%x length", src_addr, padded_length);

    data = bootloader_mmap(src_addr, length + sizeof(struct ets_secure_boot_sig_block));
    if (data == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr, length+sizeof(ets_secure_boot_signature_t));
        return ESP_FAIL;
    }

    /* Calculate digest of main image */
    esp_err_t err = bootloader_sha256_flash_contents(src_addr, padded_length, digest);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Digest calculation failed 0x%x, 0x%x", src_addr, padded_length);
        bootloader_munmap(data);
        return err;
    }

    const ets_secure_boot_signature_t *sig = (const ets_secure_boot_signature_t *)(data + length);
    int r = esp_secure_boot_verify_rsa_signature_block(sig, digest, verified_digest);
    bootloader_munmap(data);

    return (r == ETS_OK) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_secure_boot_verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, uint8_t *verified_digest)
{
    ets_secure_boot_key_digests_t trusted_keys;
    ets_secure_boot_key_digests_t trusted_key_copies[2];
    ETS_STATUS r;
    ets_secure_boot_status_t sb_result;

    memset(&trusted_keys, 0, sizeof(ets_secure_boot_key_digests_t));
    memset(trusted_key_copies, 0, 2 * sizeof(ets_secure_boot_key_digests_t));

    if (!esp_secure_boot_enabled()) {
        return ESP_OK;
    }

    r = ets_secure_boot_read_key_digests(&trusted_keys);
    if (r != ETS_OK) {
        ESP_LOGI(TAG, "Could not read secure boot digests!");
        return ESP_FAIL;
    }

    // Create the copies for FI checks (assuming result is ETS_OK, if it's not then it'll fail the fault check anyhow)
    ets_secure_boot_read_key_digests(&trusted_key_copies[0]);
    ets_secure_boot_read_key_digests(&trusted_key_copies[1]);
    ESP_FAULT_ASSERT(memcmp(&trusted_keys, &trusted_key_copies[0], sizeof(ets_secure_boot_key_digests_t)) == 0);
    ESP_FAULT_ASSERT(memcmp(&trusted_keys, &trusted_key_copies[1], sizeof(ets_secure_boot_key_digests_t)) == 0);

    ESP_LOGI(TAG, "Verifying with RSA-PSS boot...");
    sb_result = ets_secure_boot_verify_signature(sig_block, image_digest, &trusted_keys, verified_digest);
    return (sb_result == SB_SUCCESS) ? ESP_OK : ESP_FAIL;
}
