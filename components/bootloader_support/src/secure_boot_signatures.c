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

#define DIGEST_LEN 32

esp_err_t esp_secure_boot_verify_signature(uint32_t src_addr, uint32_t length)
{
    uint8_t digest[DIGEST_LEN];
    const uint8_t *data;
    const esp_secure_boot_sig_block_t *sigblock;

    ESP_LOGD(TAG, "verifying signature src_addr 0x%x length 0x%x", src_addr, length);

    data = bootloader_mmap(src_addr, length + sizeof(esp_secure_boot_sig_block_t));
    if(data == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", src_addr, length+sizeof(esp_secure_boot_sig_block_t));
        return ESP_FAIL;
    }

    // Calculate digest of main image
#ifdef BOOTLOADER_BUILD
    bootloader_sha256_handle_t handle = bootloader_sha256_start();
    bootloader_sha256_data(handle, data, length);
    bootloader_sha256_finish(handle, digest);
#else
    /* Use thread-safe esp-idf SHA function */
    esp_sha(SHA2_256, data, length, digest);
#endif

    // Map the signature block and verify the signature
    sigblock = (const esp_secure_boot_sig_block_t *)(data + length);
    esp_err_t err = esp_secure_boot_verify_signature_block(sigblock, digest);
    bootloader_munmap(data);
    return err;
}

esp_err_t esp_secure_boot_verify_signature_block(const esp_secure_boot_sig_block_t *sig_block, const uint8_t *image_digest)
{
    ptrdiff_t keylen;
    bool is_valid;

    keylen = signature_verification_key_end - signature_verification_key_start;
    if(keylen != SIGNATURE_VERIFICATION_KEYLEN) {
        ESP_LOGE(TAG, "Embedded public verification key has wrong length %d", keylen);
        return ESP_FAIL;
    }

    if (sig_block->version != 0) {
        ESP_LOGE(TAG, "image has invalid signature version field 0x%08x", sig_block->version);
        return ESP_FAIL;
    }

    is_valid = uECC_verify(signature_verification_key_start,
                                image_digest,
                                DIGEST_LEN,
                                sig_block->signature,
                                uECC_secp256r1());
    return is_valid ? ESP_OK : ESP_ERR_IMAGE_INVALID;
}
