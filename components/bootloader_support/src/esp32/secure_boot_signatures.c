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

#include "bootloader_flash.h"
#include "bootloader_sha.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp32/rom/sha.h"
#include "uECC.h"

#include <sys/param.h>

static const char *TAG = "secure_boot";

extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

#define DIGEST_LEN 32

/* Mmap source address mask */
#define MMAP_ALIGNED_MASK 0x0000FFFF

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN];
    const uint8_t *data;
    const esp_secure_boot_sig_block_t *sigblock;

    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    bootloader_sha256_handle_t handle = bootloader_sha256_start();

    uint32_t free_page_count = bootloader_mmap_get_free_pages();
    ESP_LOGD(TAG, "free data page_count 0x%08x", free_page_count);

    int32_t data_len_remain = length;
    uint32_t data_addr = src_addr;
    while (data_len_remain > 0) {
        uint32_t offset_page = ((data_addr & MMAP_ALIGNED_MASK) != 0) ? 1 : 0;
        /* Data we could map in case we are not aligned to PAGE boundary is one page size lesser. */
        uint32_t data_len = MIN(data_len_remain, ((free_page_count - offset_page) * SPI_FLASH_MMU_PAGE_SIZE));
        data = (const uint8_t *) bootloader_mmap(data_addr, data_len);
        if(!data) {
            ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", data_addr, data_len);
            bootloader_sha256_finish(handle, NULL);
            return ESP_FAIL;
        }
        bootloader_sha256_data(handle, data, data_len);
        bootloader_munmap(data);

        data_addr += data_len;
        data_len_remain -= data_len;
    }

    /* Done! Get the digest */
    bootloader_sha256_finish(handle, digest);

    // Map the signature block
    sigblock = (const esp_secure_boot_sig_block_t *) bootloader_mmap(src_addr + length, sizeof(esp_secure_boot_sig_block_t));
    if(!sigblock) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr + length, sizeof(esp_secure_boot_sig_block_t));
        return ESP_FAIL;
    }
    // Verify the signature
    esp_err_t err = esp_secure_boot_verify_signature_block(sigblock, digest);
    // Unmap
    bootloader_munmap(sigblock);

    return err;
}

esp_err_t esp_secure_boot_verify_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest)
{
    ptrdiff_t keylen;

    keylen = signature_verification_key_end - signature_verification_key_start;
    if (keylen != SIGNATURE_VERIFICATION_KEYLEN) {
        ESP_LOGE(TAG, "Embedded public verification key has wrong length %d", keylen);
        return ESP_FAIL;
    }

    if (sig_block->version != 0) {
        ESP_LOGE(TAG, "image has invalid signature version field 0x%08x", sig_block->version);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Verifying secure boot signature");

    bool is_valid;
    is_valid = uECC_verify(signature_verification_key_start,
                           image_digest,
                           DIGEST_LEN,
                           sig_block->signature,
                           uECC_secp256r1());
    ESP_LOGD(TAG, "Verification result %d", is_valid);
    return is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID;
}
