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
#include "esp_log.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"

#include "uECC.h"

#ifdef BOOTLOADER_BUILD
#include "rom/sha.h"
typedef SHA_CTX sha_context;
#else
#include "hwcrypto/sha.h"
#endif

static const char* TAG = "secure_boot";

extern const uint8_t signature_verification_key_start[] asm("_binary_signature_verification_key_bin_start");
extern const uint8_t signature_verification_key_end[] asm("_binary_signature_verification_key_bin_end");

#define SIGNATURE_VERIFICATION_KEYLEN 64

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
#ifdef BOOTLOADER_BUILD
    SHA_CTX sha;
#endif
    uint8_t digest[32];
    ptrdiff_t keylen;
    const uint8_t *data;
    const esp_secure_boot_sig_block_t *sigblock;
    bool is_valid;
#ifdef BOOTLOADER_BUILD
    const uint8_t *digest_data;
    uint32_t digest_len;
#endif

    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    data = bootloader_mmap(src_addr, length + sizeof(esp_secure_boot_sig_block_t));
    if(data == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr, length+sizeof(esp_secure_boot_sig_block_t));
        return ESP_FAIL;
    }

    sigblock = (const esp_secure_boot_sig_block_t *)(data + length);

    if (sigblock->version != 0) {
        ESP_LOGE(TAG, "src 0x%x has invalid signature version field 0x%08x", src_addr, sigblock->version);
        goto unmap_and_fail;
    }

#ifdef BOOTLOADER_BUILD
    /* Use ROM SHA functions directly */
    ets_sha_enable();
    ets_sha_init(&sha);
    digest_len = length * 8;
    digest_data = data;
    while (digest_len > 0) {
        uint32_t chunk_len = (digest_len > 64) ? 64 : digest_len;
        ets_sha_update(&sha, SHA2_256, digest_data, chunk_len);
        digest_len -= chunk_len;
        digest_data += chunk_len / 8;
    }
    ets_sha_finish(&sha, SHA2_256, digest);
    ets_sha_disable();
#else
    /* Use thread-safe esp-idf SHA function */
    esp_sha(SHA2_256, data, length, digest);
#endif

    keylen = signature_verification_key_end - signature_verification_key_start;
    if(keylen != SIGNATURE_VERIFICATION_KEYLEN) {
        ESP_LOGE(TAG, "Embedded public verification key has wrong length %d", keylen);
        goto unmap_and_fail;
    }

    is_valid = uECC_verify(signature_verification_key_start,
                           digest, sizeof(digest), sigblock->signature,
                           uECC_secp256r1());

    bootloader_munmap(data);
    return is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID;

 unmap_and_fail:
    bootloader_munmap(data);
    return ESP_FAIL;
}
